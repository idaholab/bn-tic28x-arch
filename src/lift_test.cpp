// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "lift.h"

#include <gtest/gtest.h>

#include <format>

#include "architecture.h"
#include "flags.h"
#include "instructions.h"
#include "registers.h"

// ============================================================================
// VrIndexToReg Helper Tests
// ============================================================================

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

TEST(VrIndexToReg, MasksInvalidBits) {
  // 0x08 should map to VR0 (only bits 0-2 are used)
  EXPECT_EQ(TIC28X::VrIndexToReg(0x08), TIC28X::Registers::VR0);
  // 0xFF should map to VR7 (0xFF & 0x7 = 7)
  EXPECT_EQ(TIC28X::VrIndexToReg(0xFF), TIC28X::Registers::VR7);
}

// ============================================================================
// VSTATUS Flag and Saturation Tests
// Reference: TI SPRUHS1C Section 5.3.2 - VCU Status Register (VSTATUS)
// ============================================================================

TEST(VStatusFlags, BitPositionsAreCorrect) {
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

TEST(VStatusFlags, FlagEnumsAreDefined) {
  // These should be contiguous after ARP (21)
  EXPECT_EQ(TIC28X::Flags::VSTATUS_OVFR, 22u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_OVRI, 23u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_SAT, 24u);
  EXPECT_EQ(TIC28X::Flags::VSTATUS_RND, 25u);
}

TEST(VStatusFlags, FlagNamesAreDefined) {
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_OVFR),
            "vstatus_ovfr");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_OVRI),
            "vstatus_ovri");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_SAT), "vstatus_sat");
  EXPECT_EQ(TIC28X::Flags::NAMES.at(TIC28X::Flags::VSTATUS_RND), "vstatus_rnd");
}

TEST(SaturationConstants, BoundsAreCorrect) {
  EXPECT_EQ(TIC28X::SAT_MAX_32, 0x7FFFFFFF);
  EXPECT_EQ(static_cast<uint32_t>(TIC28X::SAT_MIN_32), 0x80000000u);

  // Verify these are the correct signed 32-bit bounds
  EXPECT_EQ(TIC28X::SAT_MAX_32, std::numeric_limits<int32_t>::max());
  EXPECT_EQ(TIC28X::SAT_MIN_32, std::numeric_limits<int32_t>::min());
}

// ============================================================================
// Vashl32Vra5bit Lift Tests
// ============================================================================

struct Vashl32LiftTestCase {
  uint8_t regA;
  uint8_t imm5;
  std::string name;
};

class Vashl32Vra5bitLiftTest
    : public ::testing::TestWithParam<Vashl32LiftTestCase> {};

TEST_P(Vashl32Vra5bitLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vashl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashl32Vra5bit::SetImm5(tc.imm5);

  EXPECT_EQ(TIC28X::Vashl32Vra5bit().GetLength(), 4u);
  EXPECT_EQ(TIC28X::Vashl32Vra5bit::GetRegA(opcode), tc.regA & 0x7);
  EXPECT_EQ(TIC28X::Vashl32Vra5bit::GetImm5(opcode), tc.imm5 & 0x1F);
}

INSTANTIATE_TEST_SUITE_P(
    Vashl32Vra5bit, Vashl32Vra5bitLiftTest,
    ::testing::Values(Vashl32LiftTestCase{0, 0, "VR0_shift0"},
                      Vashl32LiftTestCase{4, 16, "VR4_shift16"},
                      Vashl32LiftTestCase{7, 31, "VR7_shift31"},
                      Vashl32LiftTestCase{2, 1, "VR2_shift1"},
                      Vashl32LiftTestCase{5, 15, "VR5_shift15"}),
    [](const testing::TestParamInfo<Vashl32Vra5bitLiftTest::ParamType>& info) {
      return info.param.name;
    });

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
// Vashr32Vra5bit Lift Tests
// ============================================================================

struct Vashr32LiftTestCase {
  uint8_t regA;
  uint8_t imm5;
  std::string name;
};

class Vashr32Vra5bitLiftTest
    : public ::testing::TestWithParam<Vashr32LiftTestCase> {};

TEST_P(Vashr32Vra5bitLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vashr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashr32Vra5bit::SetImm5(tc.imm5);

  EXPECT_EQ(TIC28X::Vashr32Vra5bit().GetLength(), 4u);
  EXPECT_EQ(TIC28X::Vashr32Vra5bit::GetRegA(opcode), tc.regA & 0x7);
  EXPECT_EQ(TIC28X::Vashr32Vra5bit::GetImm5(opcode), tc.imm5 & 0x1F);
}

INSTANTIATE_TEST_SUITE_P(
    Vashr32Vra5bit, Vashr32Vra5bitLiftTest,
    ::testing::Values(Vashr32LiftTestCase{0, 0, "VR0_shift0"},
                      Vashr32LiftTestCase{4, 16, "VR4_shift16"},
                      Vashr32LiftTestCase{7, 31, "VR7_shift31"},
                      Vashr32LiftTestCase{2, 1, "VR2_shift1"},
                      Vashr32LiftTestCase{5, 15, "VR5_shift15"}),
    [](const testing::TestParamInfo<Vashr32Vra5bitLiftTest::ParamType>& info) {
      return info.param.name;
    });

TEST(Vashr32Vra5bitLift, ExpectedRegisterMapping) {
  for (uint8_t i = 0; i < 8; ++i) {
    const uint32_t opcode =
        TIC28X::Vashr32Vra5bit::SetRegA(i) | TIC28X::Vashr32Vra5bit::SetImm5(0);
    const uint8_t regIdx = TIC28X::Vashr32Vra5bit::GetRegA(opcode);
    const uint8_t vrReg = TIC28X::VrIndexToReg(regIdx);

    EXPECT_EQ(regIdx, i) << "Register index mismatch for VR"
                         << static_cast<int>(i);
    EXPECT_EQ(vrReg, TIC28X::Registers::VR0 + i)
        << "VR register enum mismatch for VR" << static_cast<int>(i);
  }
}

TEST(Vashr32Vra5bitLift, ShiftAmountExtraction) {
  for (uint8_t shift = 0; shift < 32; ++shift) {
    const uint32_t opcode = TIC28X::Vashr32Vra5bit::SetRegA(0) |
                            TIC28X::Vashr32Vra5bit::SetImm5(shift);
    const uint8_t extractedShift = TIC28X::Vashr32Vra5bit::GetImm5(opcode);

    EXPECT_EQ(extractedShift, shift)
        << "Shift amount mismatch for shift=" << static_cast<int>(shift);
  }
}

// ============================================================================
// VbitflipVra Lift Tests
// ============================================================================

struct VbitflipVraLiftTestCase {
  uint8_t regA;
  std::string name;
};

class VbitflipVraLiftTest
    : public ::testing::TestWithParam<VbitflipVraLiftTestCase> {};

TEST_P(VbitflipVraLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VbitflipVra::SetRegA(tc.regA);

  EXPECT_EQ(TIC28X::VbitflipVra().GetLength(), 2u);
  EXPECT_EQ(TIC28X::VbitflipVra::GetRegA(opcode), tc.regA & 0xF);
}

INSTANTIATE_TEST_SUITE_P(
    VbitflipVra, VbitflipVraLiftTest,
    ::testing::Values(VbitflipVraLiftTestCase{0, "VR0"},
                      VbitflipVraLiftTestCase{1, "VR1"},
                      VbitflipVraLiftTestCase{4, "VR4"},
                      VbitflipVraLiftTestCase{7, "VR7"}),
    [](const testing::TestParamInfo<VbitflipVraLiftTest::ParamType>& info) {
      return info.param.name;
    });

TEST(VbitflipVraLift, ExpectedRegisterMapping) {
  for (uint8_t i = 0; i < 8; ++i) {
    const uint16_t opcode = TIC28X::VbitflipVra::SetRegA(i);
    const uint8_t regIdx = TIC28X::VbitflipVra::GetRegA(opcode);
    const uint8_t vrReg = TIC28X::VrIndexToReg(regIdx);

    EXPECT_EQ(regIdx, i) << "Register index mismatch for VR"
                         << static_cast<int>(i);
    EXPECT_EQ(vrReg, TIC28X::Registers::VR0 + i)
        << "VR register enum mismatch for VR" << static_cast<int>(i);
  }
}

TEST(VbitflipVraLift, InstructionLength) {
  TIC28X::VbitflipVra instr;
  EXPECT_EQ(instr.GetLength(), 2u)
      << "VBITFLIP VRa should be a 2-byte instruction";
}

TEST(VbitflipVraLift, OpcodeAndMaskAreValid) {
  // Verify the opcode matches the mask pattern
  EXPECT_EQ(TIC28X::VbitflipVra::opcode & TIC28X::VbitflipVra::opcode_mask,
            TIC28X::VbitflipVra::opcode)
      << "Opcode should match its own mask";
}

// ============================================================================
// TIC28X Intrinsic Tests
// ============================================================================

TEST(TIC28XIntrinsics, BitReverseIntrinsicIsDefined) {
  // Verify the intrinsic enum value is defined
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_BITREVERSE, 0u);
}

TEST(TIC28XIntrinsics, BitReverseIntrinsicName) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-test");
  EXPECT_EQ(arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_BITREVERSE),
            "__bitreverse");
}

TEST(TIC28XIntrinsics, BitReverseIntrinsicInputs) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-test");
  auto inputs = arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_BITREVERSE);
  ASSERT_EQ(inputs.size(), 1u) << "BITREVERSE should have 1 input";
  EXPECT_EQ(inputs[0].name, "value");
}

TEST(TIC28XIntrinsics, BitReverseIntrinsicOutputs) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-test");
  auto outputs = arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_BITREVERSE);
  ASSERT_EQ(outputs.size(), 1u) << "BITREVERSE should have 1 output";
}

TEST(TIC28XIntrinsics, GetAllIntrinsicsIncludesBitReverse) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-test");
  auto intrinsics = arch->GetAllIntrinsics();
  ASSERT_FALSE(intrinsics.empty()) << "Should have at least one intrinsic";
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_BITREVERSE),
            intrinsics.end())
      << "BITREVERSE should be in the intrinsics list";
}
