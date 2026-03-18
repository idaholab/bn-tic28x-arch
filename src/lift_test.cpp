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

// Shared test case structs for parameterized tests
struct RegAImm5TestCase {
  uint8_t regA;
  uint8_t imm5;
  std::string name;
};
struct RegATestCase {
  uint8_t regA;
  std::string name;
};
struct RegAMem32TestCase {
  uint8_t regA;
  uint8_t mem32;
  std::string name;
};

// Shared name generator for all parameterized test suites
struct NameFromParam {
  template <typename T>
  std::string operator()(const testing::TestParamInfo<T>& info) const {
    return info.param.name;
  }
};

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

class Vashl32Vra5bitLiftTest
    : public ::testing::TestWithParam<RegAImm5TestCase> {};

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
    ::testing::Values(RegAImm5TestCase{0, 0, "VR0_shift0"},
                      RegAImm5TestCase{4, 16, "VR4_shift16"},
                      RegAImm5TestCase{7, 31, "VR7_shift31"},
                      RegAImm5TestCase{2, 1, "VR2_shift1"},
                      RegAImm5TestCase{5, 15, "VR5_shift15"}),
    NameFromParam{});

// ============================================================================
// Vashr32Vra5bit Lift Tests
// ============================================================================

class Vashr32Vra5bitLiftTest
    : public ::testing::TestWithParam<RegAImm5TestCase> {};

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
class Vashr32ShiftZeroILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegATestCase> {};

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

INSTANTIATE_TEST_SUITE_P(Vashr32Vra5bit, Vashr32ShiftZeroILTest,
                         ::testing::Values(RegATestCase{0, "VR0"},
                                           RegATestCase{3, "VR3"},
                                           RegATestCase{5, "VR5"},
                                           RegATestCase{7, "VR7"}),
                         NameFromParam{});

// NOTE: VASHR32 with shift > 0 uses branching (LLIL_IF/LLIL_GOTO/MarkLabel)
// for RND mode handling. The BN core's label system requires a full Function
// context that is not available in unit tests, so IL tree verification is not
// possible for those cases. The helper function tests above cover encoding.

INSTANTIATE_TEST_SUITE_P(
    Vashr32Vra5bit, Vashr32Vra5bitLiftTest,
    ::testing::Values(RegAImm5TestCase{0, 0, "VR0_shift0"},
                      RegAImm5TestCase{4, 16, "VR4_shift16"},
                      RegAImm5TestCase{7, 31, "VR7_shift31"},
                      RegAImm5TestCase{2, 1, "VR2_shift1"},
                      RegAImm5TestCase{5, 15, "VR5_shift15"}),
    NameFromParam{});

// ============================================================================
// VCU - Bit Manipulation Instructions
// ============================================================================

// ============================================================================
// VbitflipVra Lift Tests
// ============================================================================

class VbitflipVraLiftTest : public ::testing::TestWithParam<RegATestCase> {};

TEST_P(VbitflipVraLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VbitflipVra::SetRegA(tc.regA);

  EXPECT_EQ(TIC28X::VbitflipVra().GetLength(), 2u);
  EXPECT_EQ(TIC28X::VbitflipVra::GetRegA(opcode), tc.regA & 0xF);
}

class VbitflipVraILTest : public ILTestFixture,
                          public ::testing::WithParamInterface<RegATestCase> {};

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

static const auto vbitflip_test_cases =
    ::testing::Values(RegATestCase{0, "VR0"}, RegATestCase{1, "VR1"},
                      RegATestCase{4, "VR4"}, RegATestCase{7, "VR7"});

INSTANTIATE_TEST_SUITE_P(VbitflipVra, VbitflipVraLiftTest, vbitflip_test_cases,
                         NameFromParam{});

INSTANTIATE_TEST_SUITE_P(VbitflipVra, VbitflipVraILTest, vbitflip_test_cases,
                         NameFromParam{});

// ============================================================================
// Vlshl32Vra5bit Lift Tests
// ============================================================================

class Vlshl32Vra5bitLiftTest
    : public ::testing::TestWithParam<RegAImm5TestCase> {};

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
      public ::testing::WithParamInterface<RegAImm5TestCase> {};

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
    ::testing::Values(RegAImm5TestCase{0, 0, "VR0_shift0"},
                      RegAImm5TestCase{4, 16, "VR4_shift16"},
                      RegAImm5TestCase{7, 31, "VR7_shift31"},
                      RegAImm5TestCase{2, 1, "VR2_shift1"},
                      RegAImm5TestCase{5, 15, "VR5_shift15"});

INSTANTIATE_TEST_SUITE_P(Vlshl32Vra5bit, Vlshl32Vra5bitLiftTest,
                         vlshl32_test_cases, NameFromParam{});

INSTANTIATE_TEST_SUITE_P(Vlshl32Vra5bit, Vlshl32Vra5bitILTest,
                         vlshl32_test_cases, NameFromParam{});

// ============================================================================
// Vlshr32Vra5bit Lift Tests
// ============================================================================

class Vlshr32Vra5bitLiftTest
    : public ::testing::TestWithParam<RegAImm5TestCase> {};

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
      public ::testing::WithParamInterface<RegAImm5TestCase> {};

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
    ::testing::Values(RegAImm5TestCase{0, 0, "VR0_shift0"},
                      RegAImm5TestCase{4, 16, "VR4_shift16"},
                      RegAImm5TestCase{7, 31, "VR7_shift31"},
                      RegAImm5TestCase{2, 1, "VR2_shift1"},
                      RegAImm5TestCase{5, 15, "VR5_shift15"});

INSTANTIATE_TEST_SUITE_P(Vlshr32Vra5bit, Vlshr32Vra5bitLiftTest,
                         vlshr32_test_cases, NameFromParam{});

INSTANTIATE_TEST_SUITE_P(Vlshr32Vra5bit, Vlshr32Vra5bitILTest,
                         vlshr32_test_cases, NameFromParam{});

// ============================================================================
// VCU - Negate Instructions
// ============================================================================

// ============================================================================
// VnegVra Lift Tests
// ============================================================================

class VnegVraLiftTest : public ::testing::TestWithParam<RegATestCase> {};

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
    RegATestCase{0, "VR0"}, RegATestCase{1, "VR1"}, RegATestCase{3, "VR3"},
    RegATestCase{4, "VR4"}, RegATestCase{7, "VR7"});

INSTANTIATE_TEST_SUITE_P(VnegVra, VnegVraLiftTest, vneg_test_cases,
                         NameFromParam{});

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

class VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

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
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{3, 0xFF, "VR3_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(VcaddVr5Vr4Vr3Vr2Vmov32VraMem32,
                         VcaddVr5Vr4Vr3Vr2Vmov32VraMem32LiftTest,
                         vcadd_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases,
                         NameFromParam{});

// IL verification: two IL instructions (non-branching).
// Instruction 0: vcadd — TIC28X_INTRIN_VCADD (LLIL_INTRINSIC)
// Instruction 1: parallel VMOV32 — LLIL_SET_REG(VRa, LLIL_LOAD(mem32))

class VcaddVr5Vr4Vr3Vr2Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

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
                         NameFromParam{});

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
// VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32 Lift Tests
// ============================================================================
//
// VCCMAC VR5, VR4, VR3, VR2, VR1, VR0 || VMOV32 VRa, mem32
// 4-byte instruction.  Encoding:
//   LSW (bits [31:16]): 0xE3F7 (fixed)
//   MSW (bits [15:0]):  0001 aaaa mmmm mmmm
//     bits [11:8] = aaaa -> VRa index (destination for parallel load)
//     bits [7:0]  = mmmm mmmm -> mem32 addressing mode
//
// Lifted as: VCCMAC intrinsic + VMOV32 parallel load (2 IL instructions).

class VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

TEST_P(VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32LiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32().GetLength(), 4u);

  // GetRegA round-trips (4-bit field at bits [11:8])
  EXPECT_EQ(TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::GetRegA(opcode),
            tc.regA & 0xF);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vccmac_vr5_vr4_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases =
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{3, 0xFF, "VR3_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(
    VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32,
    VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32LiftTest,
    vccmac_vr5_vr4_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases,
    NameFromParam{});

// IL verification: two IL instructions (non-branching).
// Instruction 0: vccmac — TIC28X_INTRIN_VCCMAC (LLIL_INTRINSIC)
// Instruction 1: vmov32 — LLIL_SET_REG containing LLIL_LOAD

class VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

TEST_P(VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32ILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32 instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCCMAC intrinsic ===
  const auto vccmacExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(vccmacExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(vccmacExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0)
      << "First instruction should be the vccmac intrinsic";

  // === Instruction 1: Parallel VMOV32 VRa, mem32 — SetRegister(Load(...)) ===
  const auto expectedVrReg = TIC28X::Registers::VR0 + (tc.regA & 0xF);

  const auto setregExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(setregExpr.operation, LLIL_SET_REG)
      << "Second instruction should be LLIL_SET_REG";
  EXPECT_EQ(setregExpr.operands[0], expectedVrReg)
      << "SET_REG destination should be VR" << static_cast<int>(tc.regA);
  EXPECT_EQ(setregExpr.size, 4u);

  const auto loadExpr = il->GetRawExpr(setregExpr.operands[1]);
  EXPECT_EQ(loadExpr.operation, LLIL_LOAD)
      << "Value expression should be LLIL_LOAD";
  EXPECT_EQ(loadExpr.size, 4u);

  // LOAD address: LLIL_CONST with the raw mem32 byte as the address value
  const auto addrExpr = il->GetRawExpr(loadExpr.operands[0]);
  EXPECT_EQ(addrExpr.operation, LLIL_CONST)
      << "Load address should be LLIL_CONST";
  EXPECT_EQ(addrExpr.size, 4u);
  EXPECT_EQ(addrExpr.operands[0], tc.mem32)
      << "Load address should equal mem32 value 0x" << std::hex
      << static_cast<int>(tc.mem32);
}

INSTANTIATE_TEST_SUITE_P(
    VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32,
    VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32ILTest,
    vccmac_vr5_vr4_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases,
    NameFromParam{});

// ============================================================================
// VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc Lift Tests
// ============================================================================
//
// VCCMAC VR7, VR6, VR5, VR4, mem32, *XAR7++
// 4-byte instruction.  Encoding:
//   LSW (bits [31:16]): 0xE251 (fixed)
//   MSW (bits [15:0]):  0010 1111 mmmm mmmm
//     bits [7:0] = mem32 addressing mode
//
// Lifted as a single LLIL_INTRINSIC (vccmac) with mem32 load and XAR7
// post-increment folded into the intrinsic inputs/outputs.

// Only variable field is mem32, so use a simple struct for test cases.
struct Mem32TestCase {
  uint8_t mem32;
  std::string name;
};

class VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincLiftTest
    : public ::testing::TestWithParam<Mem32TestCase> {};

TEST_P(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc().GetLength(), 4u);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vccmac_vr7_vr6_vr5_vr4_mem32_xar7_postinc_test_cases =
    ::testing::Values(
        Mem32TestCase{0x00, "mem_00"}, Mem32TestCase{0xAB, "mem_ab"},
        Mem32TestCase{0xFF, "mem_ff"}, Mem32TestCase{0x42, "mem_42"},
        Mem32TestCase{0x01, "mem_01"});

INSTANTIATE_TEST_SUITE_P(
    VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc,
    VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincLiftTest,
    vccmac_vr7_vr6_vr5_vr4_mem32_xar7_postinc_test_cases,
    [](const testing::TestParamInfo<
        VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincLiftTest::ParamType>& info) {
      return info.param.name;
    });

// Test that the intrinsic ID and name are correctly registered.
TEST(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincIntrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4, 4u);
}

TEST(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincIntrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac7654-test");
  EXPECT_EQ(
      arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4),
      "vccmac");
}

TEST(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincIntrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac7654-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4);
  // VR0-VR7, mem32, XAR7, VSTATUS = 11 inputs
  ASSERT_EQ(inputs.size(), 11u) << "vccmac vr7654 should have 11 inputs";
  EXPECT_EQ(inputs[0].name, "vr0");
  EXPECT_EQ(inputs[7].name, "vr7");
  EXPECT_EQ(inputs[8].name, "mem32");
  EXPECT_EQ(inputs[9].name, "xar7");
  EXPECT_EQ(inputs[10].name, "vstatus");
}

TEST(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincIntrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac7654-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4);
  // VR7, VR6, VR5, VR4, VR3, VR2, VR1, VR0, VSTATUS = 9 outputs
  // (XAR7 post-increment is handled explicitly in the lift, not the intrinsic)
  ASSERT_EQ(outputs.size(), 9u) << "vccmac vr7654 should have 9 outputs";
}

TEST(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincIntrinsic,
     GetAllIntrinsicsIncludesVccmac7654) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmac7654-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4),
            intrinsics.end())
      << "GetAllIntrinsics should include VCCMAC_VR7_VR6_VR5_VR4";
}

// IL verification: single LLIL_INTRINSIC instruction (non-branching).
class VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<Mem32TestCase> {};

TEST_P(VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions:
  //   0: VCCMAC intrinsic
  //   1: XAR7 post-increment (XAR7 = XAR7 + 2)
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCCMAC intrinsic ===
  const auto intrExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrExpr.operation, LLIL_INTRINSIC)
      << "Instruction 0 should be LLIL_INTRINSIC";
  EXPECT_EQ(static_cast<uint32_t>(intrExpr.operands[2]),
            TIC28X::TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4)
      << "Should use the vccmac vr7654 intrinsic";

  // === Instruction 1: XAR7 = XAR7 + 2 ===
  const auto setExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(setExpr.operation, LLIL_SET_REG)
      << "Instruction 1 should be LLIL_SET_REG for XAR7 post-increment";
}

INSTANTIATE_TEST_SUITE_P(
    VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc,
    VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincILTest,
    vccmac_vr7_vr6_vr5_vr4_mem32_xar7_postinc_test_cases,
    [](const testing::TestParamInfo<
        VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincILTest::ParamType>& info) {
      return info.param.name;
    });

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

class Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

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
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{3, 0xFF, "VR3_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(Vmov32VraMem32, Vmov32VraMem32LiftTest,
                         vmov32_vra_mem32_test_cases, NameFromParam{});

// IL verification: single LLIL_SET_REG(LLIL_LOAD(...)) instruction.

class Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

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
                         vmov32_vra_mem32_test_cases, NameFromParam{});

// ============================================================================
// Vmov32Mem32Vra Lift Tests
// ============================================================================
//
// VMOV32 mem32, VRa — Store VRa into memory location mem32.
// 4-byte instruction with RegA (4-bit VRa) and Mem32 (8-bit address).

static const auto vmov32_mem32_vra_test_cases =
    ::testing::Values(RegAMem32TestCase{0, 0, "VR0_mem0"},
                      RegAMem32TestCase{7, 0xFF, "VR7_mem255"},
                      RegAMem32TestCase{3, 0x42, "VR3_mem66"});

class Vmov32Mem32VraLiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

TEST_P(Vmov32Mem32VraLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32Mem32Vra::SetRegA(tc.regA) |
                          TIC28X::Vmov32Mem32Vra::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::Vmov32Mem32Vra().GetLength(), 4u);

  // Round-trip: encode then decode
  EXPECT_EQ(TIC28X::Vmov32Mem32Vra::GetRegA(opcode), tc.regA & 0xF);
  EXPECT_EQ(TIC28X::Vmov32Mem32Vra::GetMem32(opcode), tc.mem32 & 0xFF);
}

INSTANTIATE_TEST_SUITE_P(Vmov32Mem32Vra, Vmov32Mem32VraLiftTest,
                         vmov32_mem32_vra_test_cases, NameFromParam{});

// IL verification: single LLIL_STORE(LLIL_CONST(mem32), LLIL_REG(VRa)).

class Vmov32Mem32VraILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

TEST_P(Vmov32Mem32VraILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32Mem32Vra::SetRegA(tc.regA) |
                          TIC28X::Vmov32Mem32Vra::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vmov32Mem32Vra instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 1 IL instruction
  ASSERT_EQ(il->GetInstructionCount(), 1u);

  // === Instruction 0: VMOV32 [mem32] = VRa — Store(Const, Reg) ===
  const auto storeExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(storeExpr.operation, LLIL_STORE)
      << "Instruction should be LLIL_STORE";

  // Destination address: LLIL_CONST with mem32 value
  const auto destExpr = il->GetRawExpr(storeExpr.operands[0]);
  EXPECT_EQ(destExpr.operation, LLIL_CONST);
  EXPECT_EQ(destExpr.operands[0], tc.mem32 & 0xFF);

  // Source value: LLIL_REG with VRa
  const auto srcExpr = il->GetRawExpr(storeExpr.operands[1]);
  EXPECT_EQ(srcExpr.operation, LLIL_REG);
  const uint8_t expectedVrReg =
      static_cast<uint8_t>(TIC28X::Registers::VR0 + (tc.regA & 0xF));
  EXPECT_EQ(srcExpr.operands[0], expectedVrReg);
}

INSTANTIATE_TEST_SUITE_P(Vmov32Mem32Vra, Vmov32Mem32VraILTest,
                         vmov32_mem32_vra_test_cases, NameFromParam{});

// ============================================================================
// VccmpyVr3Vr2Vr1Vr0 Lift Tests
// ============================================================================
//
// VCCMPY VR3, VR2, VR1, VR0 (opcode 0xE50E, exact 2-byte match, no variable
// fields). Complex Conjugate 16x16=32-bit Multiply.
// Lifted as a single LLIL_INTRINSIC named "vccmpy" with:
//   inputs:  VR0, VR1, VSTATUS
//   outputs: VR3, VR2, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(VccmpyVr3Vr2Vr1Vr0Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0, 5u);
}

TEST(VccmpyVr3Vr2Vr1Vr0Intrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmpy-test");
  EXPECT_EQ(
      arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0),
      "vccmpy");
}

TEST(VccmpyVr3Vr2Vr1Vr0Intrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmpy-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0);
  // VR0, VR1, VSTATUS
  ASSERT_EQ(inputs.size(), 3u) << "vccmpy should have 3 inputs";
  EXPECT_EQ(inputs[0].name, "vr0");
  EXPECT_EQ(inputs[1].name, "vr1");
  EXPECT_EQ(inputs[2].name, "vstatus");
}

TEST(VccmpyVr3Vr2Vr1Vr0Intrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmpy-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0);
  // VR3 (Re result), VR2 (Im result), VSTATUS (flag updates)
  ASSERT_EQ(outputs.size(), 3u) << "vccmpy should have 3 outputs";
}

TEST(VccmpyVr3Vr2Vr1Vr0Intrinsic, GetAllIntrinsicsIncludesVccmpy) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vccmpy-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0),
            intrinsics.end())
      << "vccmpy should be in the intrinsics list";
}

// IL verification test: VCCMPY produces a single LLIL_INTRINSIC with the
// correct intrinsic ID.  Lift is straight-line (no branching), so IL tests
// are safe in unit-test contexts.
class VccmpyVr3Vr2Vr1Vr0ILTest : public ILTestFixture {};

TEST_F(VccmpyVr3Vr2Vr1Vr0ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE50E as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(static_cast<uint16_t>(TIC28X::Opcodes::VCCMPY_VR3_VR2_VR1_VR0),
                data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmpyVr3Vr2Vr1Vr0 instr;
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
            TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0)
      << "Should use the vccmpy intrinsic";
}

// ============================================================================
// VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32 Lift Tests
// ============================================================================
//
// VCCMPY VR3, VR2, VR1, VR0 || VMOV32 mem32, VRa
// Complex Conjugate 16x16=32-bit Multiply with parallel 32-bit store.
// Variable fields: VRa (bits [11:8]), mem32 (bits [7:0]).
// Lifted as:
//   Instruction 0: LLIL_INTRINSIC (vccmpy) — same as standalone VCCMPY
//   Instruction 1: LLIL_STORE([mem32], VRa)

class VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

TEST_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32().GetLength(), 4u);

  // GetRegA round-trips (4-bit field at bits [11:8])
  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::GetRegA(opcode),
            tc.regA & 0xF);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vccmpy_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases =
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{4, 0xFF, "VR4_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32,
                         VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LiftTest,
                         vccmpy_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases,
                         NameFromParam{});

// IL verification: two IL instructions (non-branching).
// Instruction 0: vccmpy — TIC28X_INTRIN_VCCMPY (LLIL_INTRINSIC)
// Instruction 1: parallel VMOV32 — LLIL_STORE([mem32], VRa)

class VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

TEST_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32ILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32 instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCCMPY intrinsic ===
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(intrinsicExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0)
      << "First instruction should be the vccmpy intrinsic";

  // === Instruction 1: Parallel VMOV32 [mem32] = VRa — Store(Const, Reg) ===
  const auto storeExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(storeExpr.operation, LLIL_STORE)
      << "Second instruction should be LLIL_STORE";

  // Destination address: LLIL_CONST with mem32 value
  const auto destExpr = il->GetRawExpr(storeExpr.operands[0]);
  EXPECT_EQ(destExpr.operation, LLIL_CONST);
  EXPECT_EQ(destExpr.operands[0], tc.mem32 & 0xFF);

  // Source value: LLIL_REG with VRa
  const auto srcExpr = il->GetRawExpr(storeExpr.operands[1]);
  EXPECT_EQ(srcExpr.operation, LLIL_REG);
  const uint8_t expectedVrReg =
      static_cast<uint8_t>(TIC28X::Registers::VR0 + (tc.regA & 0xF));
  EXPECT_EQ(srcExpr.operands[0], expectedVrReg);
}

INSTANTIATE_TEST_SUITE_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32,
                         VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32ILTest,
                         vccmpy_vr3_vr2_vr1_vr0_vmov32_vra_mem32_test_cases,
                         NameFromParam{});

// ============================================================================
// VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load — Complex Conjugate Multiply with
// Parallel Load
// ============================================================================

class VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadLiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

TEST_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetMem32(tc.mem32);

  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load().GetLength(), 4u);
  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::GetRegA(opcode),
            tc.regA & 0xF);
  EXPECT_EQ(TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vccmpy_load_test_cases =
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{4, 0xFF, "VR4_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load,
                         VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadLiftTest,
                         vccmpy_load_test_cases, NameFromParam{});

// IL verification: two IL instructions (non-branching).
// Instruction 0: vccmpy — TIC28X_INTRIN_VCCMPY (LLIL_INTRINSIC)
// Instruction 1: parallel VMOV32 — LLIL_SET_REG(VRa, Load([mem32]))

class VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

TEST_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCCMPY intrinsic ===
  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(intrinsicExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0)
      << "First instruction should be the vccmpy intrinsic";

  // === Instruction 1: Parallel VMOV32 VRa = [mem32] — SetReg(VRa, Load) ===
  const auto setRegExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(setRegExpr.operation, LLIL_SET_REG)
      << "Second instruction should be LLIL_SET_REG";

  // Destination register: VRa
  const uint8_t expectedVrReg =
      static_cast<uint8_t>(TIC28X::Registers::VR0 + (tc.regA & 0xF));
  EXPECT_EQ(setRegExpr.operands[0], expectedVrReg);

  // Source: LLIL_LOAD from LLIL_CONST(mem32)
  const auto loadExpr = il->GetRawExpr(setRegExpr.operands[1]);
  EXPECT_EQ(loadExpr.operation, LLIL_LOAD);
  const auto addrExpr = il->GetRawExpr(loadExpr.operands[0]);
  EXPECT_EQ(addrExpr.operation, LLIL_CONST);
  EXPECT_EQ(addrExpr.operands[0], tc.mem32 & 0xFF);
}

INSTANTIATE_TEST_SUITE_P(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load,
                         VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadILTest,
                         vccmpy_load_test_cases, NameFromParam{});

// ============================================================================
// VcconVra — Complex Conjugate
// ============================================================================

class VcconVraLiftTest : public ::testing::TestWithParam<RegATestCase> {};

TEST_P(VcconVraLiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VcconVra::SetRegA(tc.regA);

  EXPECT_EQ(TIC28X::VcconVra().GetLength(), 2u);
  EXPECT_EQ(TIC28X::VcconVra::GetRegA(opcode), tc.regA & 0xF);
}

class VcconVraILTest : public ILTestFixture,
                       public ::testing::WithParamInterface<RegATestCase> {};

TEST_P(VcconVraILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint16_t opcode = TIC28X::VcconVra::SetRegA(tc.regA);
  uint8_t data[2];
  OpcodeToData2(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::VcconVra instr;
  size_t len = 2;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 2u);

  ASSERT_EQ(il->GetInstructionCount(), 1u);

  const auto intrinsicExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(intrinsicExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(intrinsicExpr.operands[2], TIC28X::TIC28X_INTRIN_VCCON_VRA)
      << "Should use the VCCON intrinsic";
}

static const auto vccon_test_cases =
    ::testing::Values(RegATestCase{0, "VR0"}, RegATestCase{1, "VR1"},
                      RegATestCase{4, "VR4"}, RegATestCase{7, "VR7"});

INSTANTIATE_TEST_SUITE_P(VcconVra, VcconVraLiftTest, vccon_test_cases,
                         NameFromParam{});

INSTANTIATE_TEST_SUITE_P(VcconVra, VcconVraILTest, vccon_test_cases,
                         NameFromParam{});

// ============================================================================
// Vcdadd16Vr5Vr4Vr3Vr2 Lift Tests
// ============================================================================
//
// VCDADD16 VR5, VR4, VR3, VR2 (opcode 0xE504, exact 2-byte match, no variable
// fields).  Complex 16+32=16-bit addition.
// Lifted as a single LLIL_INTRINSIC named "vcdadd16" with:
//   inputs:  VR4, VR3, VR2, VSTATUS
//   outputs: VR5, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(Vcdadd16Vr5Vr4Vr3Vr2Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2, 7u);
}

TEST(Vcdadd16Vr5Vr4Vr3Vr2Intrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdadd16-test");
  EXPECT_EQ(
      arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2),
      "vcdadd16");
}

TEST(Vcdadd16Vr5Vr4Vr3Vr2Intrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdadd16-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2);
  // VR4, VR3, VR2, VSTATUS
  ASSERT_EQ(inputs.size(), 4u) << "vcdadd16 should have 4 inputs";
  EXPECT_EQ(inputs[0].name, "vr4");
  EXPECT_EQ(inputs[1].name, "vr3");
  EXPECT_EQ(inputs[2].name, "vr2");
  EXPECT_EQ(inputs[3].name, "vstatus");
}

TEST(Vcdadd16Vr5Vr4Vr3Vr2Intrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdadd16-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2);
  // VR5 (packed result), VSTATUS (flag updates)
  ASSERT_EQ(outputs.size(), 2u) << "vcdadd16 should have 2 outputs";
}

TEST(Vcdadd16Vr5Vr4Vr3Vr2Intrinsic, GetAllIntrinsicsIncludesVcdadd16) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdadd16-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2),
            intrinsics.end())
      << "vcdadd16 should be in the intrinsics list";
}

// IL verification test: VCDADD16 produces a single LLIL_INTRINSIC with the
// correct intrinsic ID.  Lift is straight-line (no branching), so IL tests
// are safe in unit-test contexts.
class Vcdadd16Vr5Vr4Vr3Vr2ILTest : public ILTestFixture {};

TEST_F(Vcdadd16Vr5Vr4Vr3Vr2ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE504 as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(
      static_cast<uint16_t>(TIC28X::Opcodes::VCDADD16_VR5_VR4_VR3_VR2), data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vcdadd16Vr5Vr4Vr3Vr2 instr;
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
            TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2)
      << "Should use the vcdadd16 intrinsic";
}

// ============================================================================
// Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32 Lift Tests
// ============================================================================
//
// VCDADD16 VR5, VR4, VR3, VR2 || VMOV32 VRa, mem32
// 4-byte instruction.  Encoding:
//   LSW (bits [31:16]): 0xE3FA (fixed)
//   MSW (bits [15:0]):  0000 aaaa mmmm mmmm
//     bits [11:8] = VRa index (0–7, not 5 or 8)
//     bits [7:0]  = mem32 addressing code
//
// Lifted as two straight-line IL instructions (no branching):
//   Instruction 0: VCDADD16 intrinsic
//   Instruction 1: Parallel VMOV32 — LLIL_SET_REG(VRa, LLIL_LOAD(mem32))

class Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32LiftTest
    : public ::testing::TestWithParam<RegAMem32TestCase> {};

TEST_P(Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32LiftTest, HelperFunctionsWork) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);

  // 4-byte instruction
  EXPECT_EQ(TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32().GetLength(), 4u);

  // GetRegA round-trips (4-bit field at bits [11:8])
  EXPECT_EQ(TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::GetRegA(opcode),
            tc.regA & 0xF);

  // GetMem32 round-trips (8-bit field at bits [7:0])
  EXPECT_EQ(TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::GetMem32(opcode),
            tc.mem32 & 0xFF);
}

static const auto vcdadd16_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases =
    ::testing::Values(RegAMem32TestCase{0, 0x00, "VR0_mem_0"},
                      RegAMem32TestCase{1, 0xAB, "VR1_mem_ab"},
                      RegAMem32TestCase{3, 0xFF, "VR3_mem_ff"},
                      RegAMem32TestCase{6, 0x42, "VR6_mem_42"},
                      RegAMem32TestCase{7, 0x01, "VR7_mem_01"});

INSTANTIATE_TEST_SUITE_P(Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32,
                         Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32LiftTest,
                         vcdadd16_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases,
                         NameFromParam{});

// IL verification: two IL instructions (non-branching).
// Instruction 0: vcdadd16 — TIC28X_INTRIN_VCDADD16 (LLIL_INTRINSIC)
// Instruction 1: parallel VMOV32 — LLIL_SET_REG(VRa, LLIL_LOAD(mem32))

class Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32ILTest
    : public ILTestFixture,
      public ::testing::WithParamInterface<RegAMem32TestCase> {};

TEST_P(Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32ILTest, GeneratesCorrectIL) {
  const auto& tc = GetParam();
  const uint32_t opcode =
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);
  uint8_t data[4];
  OpcodeToData4(opcode, data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32 instr;
  size_t len = 4;
  ASSERT_TRUE(instr.Lift(data, 0x1000, len, *il, GetArch()));
  EXPECT_EQ(len, 4u);

  // Should produce exactly 2 IL instructions
  ASSERT_EQ(il->GetInstructionCount(), 2u);

  // === Instruction 0: VCDADD16 intrinsic ===
  const auto vcdaddExpr = il->GetRawExpr(il->GetIndexForInstruction(0));
  EXPECT_EQ(vcdaddExpr.operation, LLIL_INTRINSIC);
  EXPECT_EQ(vcdaddExpr.operands[2],
            TIC28X::TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2)
      << "First instruction should be the vcdadd16 intrinsic";

  // === Instruction 1: Parallel VMOV32 VRa, mem32 — SetRegister(Load(...)) ===
  const auto expectedVrReg = TIC28X::Registers::VR0 + (tc.regA & 0xF);

  const auto setregExpr = il->GetRawExpr(il->GetIndexForInstruction(1));
  EXPECT_EQ(setregExpr.operation, LLIL_SET_REG)
      << "Second instruction should be LLIL_SET_REG";
  EXPECT_EQ(setregExpr.operands[0], expectedVrReg)
      << "SET_REG destination should be VR" << static_cast<int>(tc.regA);
  EXPECT_EQ(setregExpr.size, 4u);

  const auto loadExpr = il->GetRawExpr(setregExpr.operands[1]);
  EXPECT_EQ(loadExpr.operation, LLIL_LOAD)
      << "Value expression should be LLIL_LOAD";
  EXPECT_EQ(loadExpr.size, 4u);

  // LOAD address: LLIL_CONST with the raw mem32 byte as the address value
  const auto addrExpr = il->GetRawExpr(loadExpr.operands[0]);
  EXPECT_EQ(addrExpr.operation, LLIL_CONST)
      << "Load address should be LLIL_CONST";
  EXPECT_EQ(addrExpr.size, 4u);
  EXPECT_EQ(addrExpr.operands[0], tc.mem32)
      << "Load address should equal mem32 value 0x" << std::hex
      << static_cast<int>(tc.mem32);
}

INSTANTIATE_TEST_SUITE_P(Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32,
                         Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32ILTest,
                         vcdadd16_vr5_vr4_vr3_vr2_vmov32_vra_mem32_test_cases,
                         NameFromParam{});

// ============================================================================
// Vcdsub16Vr6Vr4Vr3Vr2 Lift Tests
// ============================================================================
//
// VCDSUB16 VR6, VR4, VR3, VR2 (opcode 0xE505, exact 2-byte match, no variable
// fields).  Complex 16-32=16-bit subtraction.
// Lifted as a single LLIL_INTRINSIC named "vcdsub16" with:
//   inputs:  VR4, VR3, VR2, VSTATUS
//   outputs: VR6, VSTATUS

// Test that the intrinsic ID and name are correctly registered.
TEST(Vcdsub16Vr6Vr4Vr3Vr2Intrinsic, IntrinsicIsDefined) {
  EXPECT_EQ(TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2, 8u);
}

TEST(Vcdsub16Vr6Vr4Vr3Vr2Intrinsic, IntrinsicName) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdsub16-test");
  EXPECT_EQ(
      arch->GetIntrinsicName(TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2),
      "vcdsub16");
}

TEST(Vcdsub16Vr6Vr4Vr3Vr2Intrinsic, IntrinsicInputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdsub16-test");
  auto inputs =
      arch->GetIntrinsicInputs(TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2);
  // VR4, VR3, VR2, VSTATUS
  ASSERT_EQ(inputs.size(), 4u) << "vcdsub16 should have 4 inputs";
  EXPECT_EQ(inputs[0].name, "vr4");
  EXPECT_EQ(inputs[1].name, "vr3");
  EXPECT_EQ(inputs[2].name, "vr2");
  EXPECT_EQ(inputs[3].name, "vstatus");
}

TEST(Vcdsub16Vr6Vr4Vr3Vr2Intrinsic, IntrinsicOutputCount) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdsub16-test");
  auto outputs =
      arch->GetIntrinsicOutputs(TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2);
  // VR6 (packed result), VSTATUS (flag updates)
  ASSERT_EQ(outputs.size(), 2u) << "vcdsub16 should have 2 outputs";
}

TEST(Vcdsub16Vr6Vr4Vr3Vr2Intrinsic, GetAllIntrinsicsIncludesVcdsub16) {
  auto arch =
      std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-vcdsub16-test");
  auto intrinsics = arch->GetAllIntrinsics();
  EXPECT_NE(std::find(intrinsics.begin(), intrinsics.end(),
                      TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2),
            intrinsics.end())
      << "vcdsub16 should be in the intrinsics list";
}

// IL verification test: VCDSUB16 produces a single LLIL_INTRINSIC with the
// correct intrinsic ID.  Lift is straight-line (no branching), so IL tests
// are safe in unit-test contexts.
class Vcdsub16Vr6Vr4Vr3Vr2ILTest : public ILTestFixture {};

TEST_F(Vcdsub16Vr6Vr4Vr3Vr2ILTest, GeneratesCorrectIL) {
  // Encode opcode 0xE505 as a 2-byte little-endian byte array.
  uint8_t data[2];
  OpcodeToData2(
      static_cast<uint16_t>(TIC28X::Opcodes::VCDSUB16_VR6_VR4_VR3_VR2), data);

  auto il = CreateIL();
  if (!il || !il->GetObject()) GTEST_SKIP() << "BN LLIL unavailable (CI mode)";

  TIC28X::Vcdsub16Vr6Vr4Vr3Vr2 instr;
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
            TIC28X::TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2)
      << "Should use the vcdsub16 intrinsic";
}
