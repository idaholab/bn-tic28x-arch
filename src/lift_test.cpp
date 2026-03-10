// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "lift.h"

#include <gtest/gtest.h>

#include <format>

#include "architecture.h"
#include "binaryninjaapi.h"
#include "flags.h"
#include "instructions.h"
#include "registers.h"

// Provides a registered BN Architecture for tests that need LowLevelILFunction.
// BN core init + architecture registration happens once per test suite.
class ILTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    if (!s_initialized) {
      BNSetBundledPluginDirectory(BNGetBundledPluginDirectory());
      BNInitPlugins(false);
      s_arch = new TIC28X::TIC28XArchitecture("tic28x-il-test");
      BN::Architecture::Register(s_arch);
      s_initialized = true;
    }
  }

  static BN::Ref<BN::LowLevelILFunction> CreateIL() {
    return new BN::LowLevelILFunction(s_arch);
  }

  static TIC28X::TIC28XArchitecture* GetArch() { return s_arch; }

 private:
  static inline bool s_initialized = false;
  static inline TIC28X::TIC28XArchitecture* s_arch = nullptr;
};

// Convert a 4-byte opcode value back to the byte array format expected by Lift.
// Reverses: data[1]<<24 | data[0]<<16 | data[3]<<8 | data[2]
static void OpcodeToData4(uint32_t opcode, uint8_t data[4]) {
  data[0] = (opcode >> 16) & 0xFF;
  data[1] = (opcode >> 24) & 0xFF;
  data[2] = (opcode >> 0) & 0xFF;
  data[3] = (opcode >> 8) & 0xFF;
}

// Convert a 2-byte opcode value back to the byte array format expected by Lift.
// Reverses: data[1]<<8 | data[0]
static void OpcodeToData2(uint16_t opcode, uint8_t data[2]) {
  data[0] = opcode & 0xFF;
  data[1] = (opcode >> 8) & 0xFF;
}

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

// ============================================================================
// VCU - Arithmetic Math Instructions
// ============================================================================

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

// NOTE: VASHL32 uses complex branching (LLIL_IF/LLIL_GOTO/MarkLabel) for SAT
// mode handling. The BN core's label system requires a full Function context
// that is not available in unit tests, so IL tree verification is not possible
// for this instruction. The helper function tests above cover operand encoding.

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

// When shift == 0, VASHR32 takes the simple path (no RND branching).
// Verify the full IL tree: LLIL_SET_REG.d(VRn, LLIL_ASR.d(LLIL_REG.d(VRn),
// LLIL_CONST.b(0))) We test all 8 VR registers to get broad coverage of the
// simple path.
struct Vashr32ShiftZeroTestCase {
  uint8_t regA;
  std::string name;
};

class Vashr32ShiftZeroILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<Vashr32ShiftZeroTestCase> {};

TEST_P(Vashr32ShiftZeroILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vashr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashr32Vra5bit::SetImm5(0);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  const uint8_t expectedVrReg = TIC28X::Registers::VR0 + (tc.regA & 0x7);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vashr32Vra5bit instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Shift == 0 takes the simple path: exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_SET_REG.d(VRn, <shift_expr>)
  const auto setReg = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(setReg.operation, LLIL_SET_REG);
  EXPECT_EQ(setReg.size, 4u);
  EXPECT_EQ(setReg.operands[0], expectedVrReg);

  // Value expression: LLIL_ASR.d(LLIL_REG.d(VRn), LLIL_CONST.b(0))
  const auto asr = il->GetRawExpr(setReg.operands[1]);
  EXPECT_EQ(asr.operation, LLIL_ASR);
  EXPECT_EQ(asr.size, 4u);

  const auto reg = il->GetRawExpr(asr.operands[0]);
  EXPECT_EQ(reg.operation, LLIL_REG);
  EXPECT_EQ(reg.size, 4u);
  EXPECT_EQ(reg.operands[0], expectedVrReg);

  const auto cnst = il->GetRawExpr(asr.operands[1]);
  EXPECT_EQ(cnst.operation, LLIL_CONST);
  EXPECT_EQ(cnst.size, 1u);
  EXPECT_EQ(cnst.operands[0], 0u);
}

INSTANTIATE_TEST_SUITE_P(
    Vashr32Vra5bit, Vashr32ShiftZeroILTest,
    ::testing::Values(Vashr32ShiftZeroTestCase{0, "VR0"},
                      Vashr32ShiftZeroTestCase{3, "VR3"},
                      Vashr32ShiftZeroTestCase{5, "VR5"},
                      Vashr32ShiftZeroTestCase{7, "VR7"}),
    [](const testing::TestParamInfo<Vashr32ShiftZeroILTest::ParamType>& info) {
      return info.param.name;
    });

// NOTE: VASHR32 with shift > 0 uses branching (LLIL_IF/LLIL_GOTO/MarkLabel)
// for RND mode handling. The BN core's label system requires a full Function
// context that is not available in unit tests, so IL tree verification is not
// possible for those cases. The helper function tests above cover encoding.

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

// ============================================================================
// VCU - Bit Manipulation Instructions
// ============================================================================

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

class VbitflipVraILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<VbitflipVraLiftTestCase> {};

// Verify VBITFLIP generates: LLIL_INTRINSIC([VRn], BITREVERSE,
// [LLIL_REG.d(VRn)])
TEST_P(VbitflipVraILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VbitflipVra::SetRegA(tc.regA);
  uint8_t data[2];
  OpcodeToData2(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VbitflipVra instr;
  size_t len = 2;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 2u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_INTRINSIC
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);

  // operands[2] is the intrinsic ID
  EXPECT_EQ(intrinsicExpr.operands[2], TIC28X::TIC28X_INTRIN_BITREVERSE)
      << "Should use the BITREVERSE intrinsic";
}

static const auto vbitflip_test_cases = ::testing::Values(
    VbitflipVraLiftTestCase{0, "VR0"}, VbitflipVraLiftTestCase{1, "VR1"},
    VbitflipVraLiftTestCase{4, "VR4"}, VbitflipVraLiftTestCase{7, "VR7"});

static auto vbitflip_name_gen = [](const auto& info) {
  return info.param.name;
};

INSTANTIATE_TEST_SUITE_P(VbitflipVra, VbitflipVraLiftTest, vbitflip_test_cases,
                         vbitflip_name_gen);

INSTANTIATE_TEST_SUITE_P(VbitflipVra, VbitflipVraILTest, vbitflip_test_cases,
                         vbitflip_name_gen);

// ============================================================================
// Vlshl32Vra5bit Lift Tests
// ============================================================================

struct Vlshl32LiftTestCase {
  uint8_t regA;
  uint8_t imm5;
  std::string name;
};

class Vlshl32Vra5bitLiftTest
    : public ::testing::TestWithParam<Vlshl32LiftTestCase> {};

TEST_P(Vlshl32Vra5bitLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshl32Vra5bit::SetImm5(tc.imm5);

  EXPECT_EQ(TIC28X::Vlshl32Vra5bit().GetLength(), 4u);
  EXPECT_EQ(TIC28X::Vlshl32Vra5bit::GetRegA(opcode), tc.regA & 0x7);
  EXPECT_EQ(TIC28X::Vlshl32Vra5bit::GetImm5(opcode), tc.imm5 & 0x1F);
}

class Vlshl32Vra5bitILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<Vlshl32LiftTestCase> {};

// Verify Lift() produces the correct IL tree:
//   LLIL_SET_REG.d(VRn,
//     LLIL_LSL.d(
//       LLIL_REG.d(VRn),
//       LLIL_CONST.b(imm)))
TEST_P(Vlshl32Vra5bitILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshl32Vra5bit::SetImm5(tc.imm5);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  const uint8_t expectedVrReg = TIC28X::Registers::VR0 + (tc.regA & 0x7);
  const uint8_t expectedShift = tc.imm5 & 0x1F;

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vlshl32Vra5bit instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_SET_REG.d(VRn, <shift_expr>)
  const auto setReg = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(setReg.operation, LLIL_SET_REG);
  EXPECT_EQ(setReg.size, 4u);
  EXPECT_EQ(setReg.operands[0], expectedVrReg)
      << "SET_REG destination should be VR" << static_cast<int>(tc.regA);

  // Value expression: LLIL_LSL.d(<reg_expr>, <const_expr>)
  const auto lsl = il->GetRawExpr(setReg.operands[1]);
  EXPECT_EQ(lsl.operation, LLIL_LSL);
  EXPECT_EQ(lsl.size, 4u);

  // Left operand of LSL: LLIL_REG.d(VRn)
  const auto reg = il->GetRawExpr(lsl.operands[0]);
  EXPECT_EQ(reg.operation, LLIL_REG);
  EXPECT_EQ(reg.size, 4u);
  EXPECT_EQ(reg.operands[0], expectedVrReg)
      << "REG source should be VR" << static_cast<int>(tc.regA);

  // Right operand of LSL: LLIL_CONST.b(imm)
  const auto cnst = il->GetRawExpr(lsl.operands[1]);
  EXPECT_EQ(cnst.operation, LLIL_CONST);
  EXPECT_EQ(cnst.size, 1u);
  EXPECT_EQ(cnst.operands[0], expectedShift)
      << "CONST should be shift amount " << static_cast<int>(expectedShift);
}

static const auto vlshl32_test_cases =
    ::testing::Values(Vlshl32LiftTestCase{0, 0, "VR0_shift0"},
                      Vlshl32LiftTestCase{4, 16, "VR4_shift16"},
                      Vlshl32LiftTestCase{7, 31, "VR7_shift31"},
                      Vlshl32LiftTestCase{2, 1, "VR2_shift1"},
                      Vlshl32LiftTestCase{5, 15, "VR5_shift15"});

static auto vlshl32_name_gen = [](const auto& info) { return info.param.name; };

INSTANTIATE_TEST_SUITE_P(Vlshl32Vra5bit, Vlshl32Vra5bitLiftTest,
                         vlshl32_test_cases, vlshl32_name_gen);

INSTANTIATE_TEST_SUITE_P(Vlshl32Vra5bit, Vlshl32Vra5bitILTest,
                         vlshl32_test_cases, vlshl32_name_gen);

// ============================================================================
// Vlshr32Vra5bit Lift Tests
// ============================================================================

struct Vlshr32LiftTestCase {
  uint8_t regA;
  uint8_t imm5;
  std::string name;
};

class Vlshr32Vra5bitLiftTest
    : public ::testing::TestWithParam<Vlshr32LiftTestCase> {};

TEST_P(Vlshr32Vra5bitLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshr32Vra5bit::SetImm5(tc.imm5);

  EXPECT_EQ(TIC28X::Vlshr32Vra5bit().GetLength(), 4u);
  EXPECT_EQ(TIC28X::Vlshr32Vra5bit::GetRegA(opcode), tc.regA & 0x7);
  EXPECT_EQ(TIC28X::Vlshr32Vra5bit::GetImm5(opcode), tc.imm5 & 0x1F);
}

class Vlshr32Vra5bitILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<Vlshr32LiftTestCase> {};

// Verify Lift() produces the correct IL tree:
//   LLIL_SET_REG.d(VRn,
//     LLIL_LSR.d(
//       LLIL_REG.d(VRn),
//       LLIL_CONST.b(imm)))
TEST_P(Vlshr32Vra5bitILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshr32Vra5bit::SetImm5(tc.imm5);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  const uint8_t expectedVrReg = TIC28X::Registers::VR0 + (tc.regA & 0x7);
  const uint8_t expectedShift = tc.imm5 & 0x1F;

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vlshr32Vra5bit instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_SET_REG.d(VRn, <shift_expr>)
  const auto setReg = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(setReg.operation, LLIL_SET_REG);
  EXPECT_EQ(setReg.size, 4u);
  EXPECT_EQ(setReg.operands[0], expectedVrReg)
      << "SET_REG destination should be VR" << static_cast<int>(tc.regA);

  // Value expression: LLIL_LSR.d(<reg_expr>, <const_expr>)
  const auto lsr = il->GetRawExpr(setReg.operands[1]);
  EXPECT_EQ(lsr.operation, LLIL_LSR);
  EXPECT_EQ(lsr.size, 4u);

  // Left operand of LSR: LLIL_REG.d(VRn)
  const auto reg = il->GetRawExpr(lsr.operands[0]);
  EXPECT_EQ(reg.operation, LLIL_REG);
  EXPECT_EQ(reg.size, 4u);
  EXPECT_EQ(reg.operands[0], expectedVrReg)
      << "REG source should be VR" << static_cast<int>(tc.regA);

  // Right operand of LSR: LLIL_CONST.b(imm)
  const auto cnst = il->GetRawExpr(lsr.operands[1]);
  EXPECT_EQ(cnst.operation, LLIL_CONST);
  EXPECT_EQ(cnst.size, 1u);
  EXPECT_EQ(cnst.operands[0], expectedShift)
      << "CONST should be shift amount " << static_cast<int>(expectedShift);
}

static const auto vlshr32_test_cases =
    ::testing::Values(Vlshr32LiftTestCase{0, 0, "VR0_shift0"},
                      Vlshr32LiftTestCase{4, 16, "VR4_shift16"},
                      Vlshr32LiftTestCase{7, 31, "VR7_shift31"},
                      Vlshr32LiftTestCase{2, 1, "VR2_shift1"},
                      Vlshr32LiftTestCase{5, 15, "VR5_shift15"});

static auto vlshr32_name_gen = [](const auto& info) { return info.param.name; };

INSTANTIATE_TEST_SUITE_P(Vlshr32Vra5bit, Vlshr32Vra5bitLiftTest,
                         vlshr32_test_cases, vlshr32_name_gen);

INSTANTIATE_TEST_SUITE_P(Vlshr32Vra5bit, Vlshr32Vra5bitILTest,
                         vlshr32_test_cases, vlshr32_name_gen);

// ============================================================================
// VCU - Negate Instructions
// ============================================================================

// ============================================================================
// VnegVra Lift Tests
// ============================================================================

struct VnegVraLiftTestCase {
  uint8_t regA;  // VRa register index (0-7)
  std::string name;
};

class VnegVraLiftTest : public ::testing::TestWithParam<VnegVraLiftTestCase> {};

TEST_P(VnegVraLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VnegVra::SetRegA(tc.regA);

  // 2-byte instruction
  EXPECT_EQ(TIC28X::VnegVra().GetLength(), 2u);

  // GetRegA round-trips correctly (bits 3-0, lower 4 bits of the register
  // index)
  EXPECT_EQ(TIC28X::VnegVra::GetRegA(opcode), tc.regA & 0xF);
}

// NOTE: VnegVra uses branching (il.If/il.MarkLabel/il.Goto) for overflow
// detection and SAT mode handling. The BN core's label system requires a full
// Function context that is not available in unit tests, so IL tree verification
// is not possible. The helper function tests above cover operand encoding.

static const auto vneg_test_cases = ::testing::Values(
    VnegVraLiftTestCase{0, "VR0"}, VnegVraLiftTestCase{1, "VR1"},
    VnegVraLiftTestCase{3, "VR3"}, VnegVraLiftTestCase{4, "VR4"},
    VnegVraLiftTestCase{7, "VR7"});

static auto vneg_name_gen = [](const auto& info) { return info.param.name; };

INSTANTIATE_TEST_SUITE_P(VnegVra, VnegVraLiftTest, vneg_test_cases,
                         vneg_name_gen);

// ============================================================================
// VCU - Complex Math Instructions
// ============================================================================

// ============================================================================
// VcaddVr5Vr4Vr3Vr2 Lift Tests
// ============================================================================
//
// VCADD VR5, VR4, VR3, VR2 (opcode 0xE502, exact 2-byte match, no variable
// fields). The instruction is lifted as a single LLIL_INTRINSIC named "vcadd"
// with:
//   inputs:  VR5, VR4, VR3, VR2, VSTATUS
//   outputs: VR5, VR4, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(VcaddVr5Vr4Vr3Vr2Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2, 1u);
}

TEST(VcaddVr5Vr4Vr3Vr2Intrinsic, IntrinsicName) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd-test");
  EXPECT_EQ(arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2),
            "vcadd");
}

TEST(VcaddVr5Vr4Vr3Vr2Intrinsic, IntrinsicInputCount) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2);
  // VR5, VR4, VR3, VR2, VSTATUS
  ASSERT_EQ(inputs.size(), 5u) << "vcadd should have 5 inputs";
  EXPECT_EQ(inputs[0].name, "vr5");
  EXPECT_EQ(inputs[1].name, "vr4");
  EXPECT_EQ(inputs[2].name, "vr3");
  EXPECT_EQ(inputs[3].name, "vr2");
  EXPECT_EQ(inputs[4].name, "vstatus");
}

TEST(VcaddVr5Vr4Vr3Vr2Intrinsic, IntrinsicOutputCount) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2);
  // VR5 (Re result), VR4 (Im result), VSTATUS (flag updates)
  ASSERT_EQ(outputs.size(), 3u) << "vcadd should have 3 outputs";
}

TEST(VcaddVr5Vr4Vr3Vr2Intrinsic, GetAllIntrinsicsIncludesVcadd) {
  auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2),
            intrinsics.end())
      << "vcadd should be in the intrinsics list";
}

// IL verification test: VCADD produces a single LLIL_INTRINSIC with the
// correct intrinsic ID.  Lift is straight-line (no branching), so IL tests
// are safe in unit-test contexts.
class VcaddVr5Vr4Vr3Vr2ILTest : public ILTestFixture {};

TEST_F(VcaddVr5Vr4Vr3Vr2ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE502 as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(static_cast<uint16_t>(TIC28X::Opcodes::VCADD_VR5_VR4_VR3_VR2),
                data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VcaddVr5Vr4Vr3Vr2 instr;
  size_t len = 2;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 2u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_INTRINSIC
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);

  // operands[2] is the intrinsic ID
  EXPECT_EQ(intrinsicExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2)
      << "Should use the vcadd intrinsic";
}

// ============================================================================
// VcaddVr7Vr6Vr5Vr4 Lift Tests
// ============================================================================
//
// VCADD VR7, VR6, VR5, VR4 (opcode 0xE52A, exact 2-byte match, no variable
// fields). The instruction is lifted as a single LLIL_INTRINSIC named "vcadd"
// with:
//   inputs:  VR7, VR6, VR5, VR4, VSTATUS
//   outputs: VR7, VR6, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(VcaddVr7Vr6Vr5Vr4Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4, 2u);
}

TEST(VcaddVr7Vr6Vr5Vr4Intrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd76-test");
  EXPECT_EQ(arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4),
            "vcadd");
}

TEST(VcaddVr7Vr6Vr5Vr4Intrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd76-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4);
  // VR7, VR6, VR5, VR4, VSTATUS
  ASSERT_EQ(inputs.size(), 5u) << "vcadd VR7,VR6,VR5,VR4 should have 5 inputs";
  EXPECT_EQ(inputs[0].name, "vr7");
  EXPECT_EQ(inputs[1].name, "vr6");
  EXPECT_EQ(inputs[2].name, "vr5");
  EXPECT_EQ(inputs[3].name, "vr4");
  EXPECT_EQ(inputs[4].name, "vstatus");
}

TEST(VcaddVr7Vr6Vr5Vr4Intrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd76-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4);
  // VR7 (Re result), VR6 (Im result), VSTATUS (flag updates)
  ASSERT_EQ(outputs.size(), 3u)
      << "vcadd VR7,VR6,VR5,VR4 should have 3 outputs";
}

TEST(VcaddVr7Vr6Vr5Vr4Intrinsic, GetAllIntrinsicsIncludesVcadd76) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcadd76-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4),
            intrinsics.end())
      << "vcadd VR7,VR6,VR5,VR4 should be in the intrinsics list";
}

// IL verification test: VCADD VR7,VR6,VR5,VR4 produces a single
// LLIL_INTRINSIC with the correct intrinsic ID.  Lift is straight-line
// (no branching), so IL tests are safe in unit-test contexts.
class VcaddVr7Vr6Vr5Vr4ILTest : public ILTestFixture {};

TEST_F(VcaddVr7Vr6Vr5Vr4ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE52A as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(static_cast<uint16_t>(TIC28X::Opcodes::VCADD_VR7_VR6_VR5_VR4),
                data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VcaddVr7Vr6Vr5Vr4 instr;
  size_t len = 2;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 2u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_INTRINSIC
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);

  // operands[2] is the intrinsic ID
  EXPECT_EQ(intrinsicExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4)
      << "Should use the vcadd VR7,VR6,VR5,VR4 intrinsic";
}

// ============================================================================
// VcaddVr5Vr4Vr3Vr2Vmov32VraMem32 Lift Tests
// ============================================================================
//
// VCADD VR5, VR4, VR3, VR2 || VMOV32 VRa, mem32
// 4-byte instruction.  Encoding:
//   LSW (bits [31:16]): 0xE3F8 (fixed)
//   MSW (bits [15:0]):  0000 aaaa mmmm mmmm
//     bits [11:8] = VRa index (0–7, not 8 or 4 or 5)
//     bits [7:0]  = mem32 addressing code
//
// Lifted as two straight-line LLIL_INTRINSIC instructions (no branching),
// so IL tree verification is safe in unit-test contexts.

struct VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase {
  uint8_t regA;
  uint8_t mem32;
  std::string name;
};

class VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<
          VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase> {};

TEST_P(VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32().GetLength(), 4u);

  // GetRegA round-trips (4-bit field at bits [11:8])
  EXPECT_EQ(TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::GetRegA(opcode),
            tc.regA & 0xF);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases =
    ::testing::Values(
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase{0, 0x00, "VR0_mem_0"},
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase{1, 0xAB, "VR1_mem_ab"},
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase{3, 0xFF, "VR3_mem_ff"},
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase{6, 0x42, "VR6_mem_42"},
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase{7, 0x01, "VR7_mem_01"});

static auto vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_name_gen =
    [](const auto& info) { return info.param.name; };

INSTANTIATE_TEST_SUITE_P(VcaddVr5Vr4Vr3Vr2Vmov32VraMem32,
                         VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTest,
                         vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases,
                         vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_name_gen);

// IL verification: two IL instructions (non-branching).
// Instruction 0: vcadd — TIC28X_INTRIN_VCADD (LLIL_INTRINSIC)
// Instruction 1: parallel VMOV32 — LLIL_SET_REG(VRa, LLIL_LOAD(mem32))

class VcaddVr5Vr4Vr3Vr2Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<
          VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTestCase> {};

TEST_P(VcaddVr5Vr4Vr3Vr2Vmov32VraMem32ILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32 instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCADD intrinsic ===
  const auto vcaddExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(vcaddExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(vcaddExpr.operands[2], TIC28X::TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2)
      << "First instruction should be the vcadd intrinsic";

  // === Instruction 1: Parallel VMOV32 VRa, mem32 — SetRegister(Load(...)) ===
  const auto setregExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(setregExpr.operation, LLIL_SET_REG)
      << "Second instruction should be LLIL_SET_REG";
  const auto loadExpr = il->GetRawExpr(setregExpr.operands[1]);
  EXPECT_EQ(loadExpr.operation, LLIL_LOAD)
      << "Value expression should be LLIL_LOAD";
}

INSTANTIATE_TEST_SUITE_P(VcaddVr5Vr4Vr3Vr2Vmov32VraMem32,
                         VcaddVr5Vr4Vr3Vr2Vmov32VraMem32ILTest,
                         vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases,
                         vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_name_gen);

// ============================================================================
// VccmacVr5Vr4Vr3Vr2Vr1Vr0 Lift Tests
// ============================================================================
//
// VCCMAC VR5, VR4, VR3, VR2, VR1, VR0 (opcode 0xE50F, exact 2-byte match,
// no variable fields). The instruction is lifted as a single LLIL_INTRINSIC
// named "vccmac" with:
//   inputs:  VR0, VR1, VR2, VR3, VR4, VR5, VSTATUS
//   outputs: VR5, VR4, VR3, VR2, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0, 3u);
}

TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0Intrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac-test");
  EXPECT_EQ(arch->GetIntrinsicName(
                TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0),
            "vccmac");
}

TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0Intrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac-test");
  auto inputs = arch->GetIntrinsicInputs(
      TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0);
  // VR0, VR1, VR2, VR3, VR4, VR5, VSTATUS
  ASSERT_EQ(inputs.size(), 7u) << "vccmac should have 7 inputs";
  EXPECT_EQ(inputs[0].name, "vr0");
  EXPECT_EQ(inputs[1].name, "vr1");
  EXPECT_EQ(inputs[2].name, "vr2");
  EXPECT_EQ(inputs[3].name, "vr3");
  EXPECT_EQ(inputs[4].name, "vr4");
  EXPECT_EQ(inputs[5].name, "vr5");
  EXPECT_EQ(inputs[6].name, "vstatus");
}

TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0Intrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac-test");
  auto outputs = arch->GetIntrinsicOutputs(
      TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0);
  // VR5 (Re accum), VR4 (Im accum), VR3 (Re mult), VR2 (Im mult), VSTATUS
  ASSERT_EQ(outputs.size(), 5u) << "vccmac should have 5 outputs";
}

TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0Intrinsic, GetAllIntrinsicsIncludesVccmac) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0),
            intrinsics.end())
      << "vccmac should be in the intrinsics list";
}

// IL verification test: VCCMAC produces a single LLIL_INTRINSIC with the
// correct intrinsic ID.  Lift is straight-line (no branching), so IL tests
// are safe in unit-test contexts.
class VccmacVr5Vr4Vr3Vr2Vr1Vr0ILTest : public ILTestFixture {};

TEST_F(VccmacVr5Vr4Vr3Vr2Vr1Vr0ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE50F as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(
      static_cast<uint16_t>(TIC28X::Opcodes::VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0),
      data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0 instr;
  size_t len = 2;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 2u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // Top-level: LLIL_INTRINSIC
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);

  // operands[2] is the intrinsic ID
  EXPECT_EQ(intrinsicExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0)
      << "Should use the vccmac intrinsic";
}

// ============================================================================
// VCU - General Move Instructions
// ============================================================================

// ============================================================================
// Vmov32VraMem32 Lift Tests
// ============================================================================
//
// VMOV32 VRa, mem32  (standalone)
// 4-byte instruction.  Encoding:
//   LSW (bits [31:16]): 0xE3F0 (fixed)
//   MSW (bits [15:0]):  0000 aaaa mmmm mmmm
//     bits [11:8] = VRa index (4-bit)
//     bits [7:0]  = mem32 addressing code
//
// Lifted as a single straight-line LLIL_INTRINSIC (no branching), so IL tree
// verification is safe in unit-test contexts.

struct Vmov32VraMem32LiftTestCase {
  uint8_t regA;
  uint8_t mem32;
  std::string name;
};

class Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<Vmov32VraMem32LiftTestCase> {};

TEST_P(Vmov32VraMem32LiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32VraMem32::SetRegA(tc.regA) |
                          TIC28X::Vmov32VraMem32::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::Vmov32VraMem32().GetLength(), 4u);

  // GetRegA round-trips (4-bit field at bits [11:8])
  EXPECT_EQ(TIC28X::Vmov32VraMem32::GetRegA(opcode), tc.regA & 0xF);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::Vmov32VraMem32::GetMem32(opcode), tc.mem32 & 0xFF);
}

static const auto vmov32_vra_mem32_test_cases =
    ::testing::Values(Vmov32VraMem32LiftTestCase{0, 0x00, "VR0_mem_0"},
                      Vmov32VraMem32LiftTestCase{1, 0xAB, "VR1_mem_ab"},
                      Vmov32VraMem32LiftTestCase{3, 0xFF, "VR3_mem_ff"},
                      Vmov32VraMem32LiftTestCase{6, 0x42, "VR6_mem_42"},
                      Vmov32VraMem32LiftTestCase{7, 0x01, "VR7_mem_01"});

static auto vmov32_vra_mem32_name_gen = [](const auto& info) {
  return info.param.name;
};

INSTANTIATE_TEST_SUITE_P(Vmov32VraMem32, Vmov32VraMem32LiftTest,
                         vmov32_vra_mem32_test_cases,
                         vmov32_vra_mem32_name_gen);

// IL verification: single LLIL_SET_REG(LLIL_LOAD(...)) instruction.

class Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<Vmov32VraMem32LiftTestCase> {};

TEST_P(Vmov32VraMem32ILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32VraMem32::SetRegA(tc.regA) |
                          TIC28X::Vmov32VraMem32::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vmov32VraMem32 instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // === Instruction 0: VMOV32 VRa = Load(mem32) ===
  const auto setregExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(setregExpr.operation, LLIL_SET_REG)
      << "Instruction should be LLIL_SET_REG";
  const auto loadExpr = il->GetRawExpr(setregExpr.operands[1]);
  EXPECT_EQ(loadExpr.operation, LLIL_LOAD)
      << "Value expression should be LLIL_LOAD";
}

INSTANTIATE_TEST_SUITE_P(Vmov32VraMem32, Vmov32VraMem32ILTest,
                         vmov32_vra_mem32_test_cases,
                         vmov32_vra_mem32_name_gen);
