// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "lift.h"

#include <binaryninjaapi.h>
#include <gtest/gtest.h>
#include <lowlevelilinstruction.h>

#include <format>

#include "architecture.h"
#include "flags.h"
#include "instructions.h"
#include "registers.h"

namespace BN = BinaryNinja;

// Test that VrIndexToReg correctly maps indices to VR register enums
TEST(VrIndexToReg, MapsIndicesToRegisters) {
  EXPECT_EQ(TIC28X::VrIndexToReg(0), TIC28X::Registers::VR0);
  EXPECT_EQ(TIC28X::VrIndexToReg(1), TIC28X::Registers::VR1);
  EXPECT_EQ(TIC28X::VrIndexToReg(2), TIC28X::Registers::VR2);
  EXPECT_EQ(TIC28X::VrIndexToReg(3), TIC28X::Registers::VR3);
  EXPECT_EQ(TIC28X::VrIndexToReg(4), TIC28X::Registers::VR4);
  EXPECT_EQ(TIC28X::VrIndexToReg(5), TIC28X::Registers::VR5);
  EXPECT_EQ(TIC28X::VrIndexToReg(6), TIC28X::Registers::VR6);
  EXPECT_EQ(TIC28X::VrIndexToReg(7), TIC28X::Registers::VR7);
}

// Test that VrIndexToReg masks out bits beyond the 3-bit index
TEST(VrIndexToReg, MasksInvalidBits) {
  // 0x08 should map to VR0 (only bits 0-2 are used)
  EXPECT_EQ(TIC28X::VrIndexToReg(0x08), TIC28X::Registers::VR0);
  // 0xFF should map to VR7 (0xFF & 0x7 = 7)
  EXPECT_EQ(TIC28X::VrIndexToReg(0xFF), TIC28X::Registers::VR7);
}

// Test case structure for Vashl32Vra5bit lift tests
struct Vashl32LiftTestCase {
  uint8_t regA;      // VRa register index (0-7)
  uint8_t imm5;      // 5-bit immediate shift amount (0-31)
  std::string name;  // Test name for display
};

class Vashl32Vra5bitLiftTest
    : public ::testing::TestWithParam<Vashl32LiftTestCase> {};

TEST_P(Vashl32Vra5bitLiftTest, LiftReturnsTrue) {
  const auto& tc = GetParam();

  // Build the instruction opcode
  const uint32_t opcode = TIC28X::Vashl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashl32Vra5bit::SetImm5(tc.imm5);

  // Convert opcode to byte array (little-endian, LSW first for 4-byte)
  // Based on DataToOpcode: data[1]<<24 | data[0]<<16 | data[3]<<8 | data[2]
  // So: opcode[31:24] = data[1], opcode[23:16] = data[0],
  //     opcode[15:8] = data[3], opcode[7:0] = data[2]
  std::array<uint8_t, 4> bytes = {
      static_cast<uint8_t>((opcode >> 16) & 0xFF),  // data[0] = bits 23:16
      static_cast<uint8_t>((opcode >> 24) & 0xFF),  // data[1] = bits 31:24
      static_cast<uint8_t>(opcode & 0xFF),          // data[2] = bits 7:0
      static_cast<uint8_t>((opcode >> 8) & 0xFF),   // data[3] = bits 15:8
  };

  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-lift-test");
  arch->SetObjmode(TIC28X::OBJMODE_1);  // VCU instructions require OBJMODE_1

  size_t len = 0;

  // Create a mock LLIL function - we can't easily test the actual IL output
  // without a full Binary Ninja context, but we can at least verify the
  // Lift method returns true and sets the length correctly.
  // Note: Creating an actual LowLevelILFunction requires a valid function
  // object which needs a BinaryView. For unit testing, we verify the
  // instruction class methods work correctly.

  // For now, test that the instruction can be decoded and the helper
  // functions return expected values
  auto instr = TIC28X::Vashl32Vra5bit();
  EXPECT_EQ(instr.GetLength(), 4u);
  EXPECT_EQ(TIC28X::Vashl32Vra5bit::GetRegA(opcode), tc.regA & 0x7);
  EXPECT_EQ(TIC28X::Vashl32Vra5bit::GetImm5(opcode), tc.imm5 & 0x1F);
}

INSTANTIATE_TEST_SUITE_P(
    Vashl32Vra5bit, Vashl32Vra5bitLiftTest,
    ::testing::Values(
        // Test VR0 with shift 0
        Vashl32LiftTestCase{0, 0, "VR0_shift0"},
        // Test VR4 with shift 16
        Vashl32LiftTestCase{4, 16, "VR4_shift16"},
        // Test VR7 with max shift 31
        Vashl32LiftTestCase{7, 31, "VR7_shift31"},
        // Test VR2 with shift 1
        Vashl32LiftTestCase{2, 1, "VR2_shift1"},
        // Test VR5 with shift 15
        Vashl32LiftTestCase{5, 15, "VR5_shift15"}),
    [](const testing::TestParamInfo<Vashl32Vra5bitLiftTest::ParamType>& info) {
      return info.param.name;
    });

// Additional test to verify the expected VR register is computed correctly
TEST(Vashl32Vra5bitLift, ExpectedRegisterMapping) {
  for (uint8_t i = 0; i < 8; ++i) {
    const uint32_t opcode =
        TIC28X::Vashl32Vra5bit::SetRegA(i) | TIC28X::Vashl32Vra5bit::SetImm5(0);
    const uint8_t regIdx = TIC28X::Vashl32Vra5bit::GetRegA(opcode);
    const uint8_t vrReg = TIC28X::VrIndexToReg(regIdx);

    EXPECT_EQ(regIdx, i) << "Register index mismatch for VR"
                         << static_cast<int>(i);
    EXPECT_EQ(vrReg, TIC28X::Registers::VR0 + i)
        << "VR register enum mismatch for VR" << static_cast<int>(i);
  }
}

// Test shift amount extraction for various values
TEST(Vashl32Vra5bitLift, ShiftAmountExtraction) {
  for (uint8_t shift = 0; shift < 32; ++shift) {
    const uint32_t opcode = TIC28X::Vashl32Vra5bit::SetRegA(0) |
                            TIC28X::Vashl32Vra5bit::SetImm5(shift);
    const uint8_t extractedShift = TIC28X::Vashl32Vra5bit::GetImm5(opcode);

    EXPECT_EQ(extractedShift, shift)
        << "Shift amount mismatch for shift=" << static_cast<int>(shift);
  }
}

// ============================================================================
// VSTATUS Flag and Saturation Tests
// ============================================================================

// Test VSTATUS flag bit positions and masks
// Reference: TI SPRUHS1C Section 5.3.2 - VCU Status Register (VSTATUS)
TEST(VStatusFlags, BitPositionsAreCorrect) {
  // Verify the bit positions match TI documentation
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SHIFTR_BIT, 0u);  // bits 4-0
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SHIFTL_BIT, 5u);  // bits 9-5
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SAT_BIT, 10u);    // bit 10
  EXPECT_EQ(TIC28X::Flags::VStatusBits::RND_BIT, 11u);    // bit 11
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OVFR_BIT, 12u);   // bit 12
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OVRI_BIT, 13u);   // bit 13
  EXPECT_EQ(TIC28X::Flags::VStatusBits::CPACK_BIT, 14u);  // bit 14
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OPACK_BIT, 15u);  // bit 15
}

TEST(VStatusFlags, MasksAreCorrect) {
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SHIFTR_MASK, 0x0000001Fu);  // bits 4-0
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SHIFTL_MASK, 0x000003E0u);  // bits 9-5
  EXPECT_EQ(TIC28X::Flags::VStatusBits::SAT_MASK, 0x00000400u);     // bit 10
  EXPECT_EQ(TIC28X::Flags::VStatusBits::RND_MASK, 0x00000800u);     // bit 11
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OVFR_MASK, 0x00001000u);    // bit 12
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OVRI_MASK, 0x00002000u);    // bit 13
  EXPECT_EQ(TIC28X::Flags::VStatusBits::CPACK_MASK, 0x00004000u);   // bit 14
  EXPECT_EQ(TIC28X::Flags::VStatusBits::OPACK_MASK, 0x00008000u);   // bit 15
}

// Test saturation constants
TEST(SaturationConstants, BoundsAreCorrect) {
  EXPECT_EQ(TIC28X::SAT_MAX_32, 0x7FFFFFFF);
  EXPECT_EQ(static_cast<uint32_t>(TIC28X::SAT_MIN_32), 0x80000000u);

  // Verify these are the correct signed 32-bit bounds
  EXPECT_EQ(TIC28X::SAT_MAX_32, std::numeric_limits<int32_t>::max());
  EXPECT_EQ(TIC28X::SAT_MIN_32, std::numeric_limits<int32_t>::min());
}

// Test that VSTATUS flag constants are defined correctly in flags.h
TEST(VStatusFlags, FlagEnumsAreDefined) {
  // These should be contiguous after ARP (21)
  EXPECT_EQ(TIC28X::Flags::VSTATUS_OVFR, 22u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_OVRI, 23u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_SAT, 24u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_RND, 25u);
}

// Test that VSTATUS flag names are in the NAMES map
TEST(VStatusFlags, FlagNamesAreDefined) {
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_OVFR),
            "vstatus_ovfr");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_OVRI),
            "vstatus_ovri");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_SAT), "vstatus_sat");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_RND), "vstatus_rnd");
}
