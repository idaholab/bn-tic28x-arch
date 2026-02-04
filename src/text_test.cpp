// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include <binaryninjaapi.h>
#include <gtest/gtest.h>

#include <format>

#include "architecture.h"
#include "instructions.h"

constexpr uint32_t TEST_DATA = 0xFFFFFFFF;

// Convert an array of instruction text tokens to a string
static std::string tokens_to_string(
    const std::vector<BN::InstructionTextToken> &tokens) {
  std::string out;

  for (const auto &token : tokens) {
    out.append(token.text);
  }

  return out;
}

// Compare two text token vectors
static void compare_text_tokens(
    const std::vector<BN::InstructionTextToken> &a,
    const std::vector<BN::InstructionTextToken> &b) {
  EXPECT_EQ(a.size(), b.size());
  for (size_t i = 0; i < std::min(a.size(), b.size()); i++) {
    EXPECT_EQ(a[i].type, b[i].type)
        << "token types @ index " << i << " do not match";
  }

  const auto a_assembly = tokens_to_string(a);
  const auto b_assembly = tokens_to_string(b);

  EXPECT_EQ(a_assembly, b_assembly) << "accumulated token strings do not match";
}

// Verify that an architecture returns the correct text tokens
static void test_architecture_text(
    const uint32_t opcode, const TIC28X::ObjectMode objmode,
    const uint64_t address, const std::vector<BN::InstructionTextToken> &want) {
  std::array<uint8_t, 4> full_op = {};

  size_t expected_len;

  // Build full opcode according to 2 or 4 bytes
  if ((opcode & 0xFFFF0000) == 0) {  // 2 byte opcode
    expected_len = 2;
    full_op = {
        static_cast<uint8_t>(opcode & 0xFF),
        static_cast<uint8_t>((opcode & 0xFF00) >> 8),
        0,
        0,
    };
  } else {  // 4-byte opcode
    expected_len = 4;
    full_op = {
        static_cast<uint8_t>((opcode & 0xFF0000) >> 16),
        static_cast<uint8_t>((opcode & 0xFF000000) >> 24),
        static_cast<uint8_t>(opcode & 0xFF),
        static_cast<uint8_t>((opcode & 0xFF00) >> 8),
    };
  }

  size_t len = 0;
  auto got = std::vector<BN::InstructionTextToken>{};
  const auto arch = std::make_unique<TIC28X::TIC28XArchitecture>("tic28x-test");
  arch->SetObjmode(objmode);  // set operating object mode

  EXPECT_TRUE(arch->GetInstructionText(full_op.data(), address, len, got));
  EXPECT_EQ(len, expected_len);
  compare_text_tokens(got, want);
}

/* Instruction Text Tests */

// // Format: OP
// class TestTextOpP : public ::testing::TestWithParam<
//                         std::tuple<unsigned int, TIC28X::ObjectMode>> {};
//
// TEST_P(TestTextOpP, TestInstructionText) {
//   const uint32_t opcode = std::get<0>(GetParam());
//   const TIC28X::ObjectMode objmode = std::get<1>(GetParam());
//   const std::vector<BN::InstructionTextToken> want = {
//       {InstructionToken, TIC28X::Opcodes::NAMES.at(opcode)},
//   };
//
//   test_architecture_text(opcode, objmode, 0x0, want);
// }
//
// // Test all 'OP' format instructions
// INSTANTIATE_TEST_SUITE_P(
//     InstructionFormatOp, TestTextOpP,
//     ::testing::ValuesIn(
//         std::vector<std::tuple<unsigned int, TIC28X::ObjectMode>>{
//             {TIC28X::Aborti::opcode, TIC28X::Aborti::objmode},
//         }),
//     [](const testing::TestParamInfo<TestTextOpP::ParamType> &info) {
//       const auto op = std::get<0>(info.param);
//       return "OP_" + TIC28X::Opcodes::NAMES.at(op);
//     });
//
// TEST(TestInstructionText, AbsAcc) {
//   const auto instr = TIC28X::AbsAcc();
//   const std::vector<BN::InstructionTextToken> want = {
//       {InstructionToken, "abs"}, {TextToken, " "}, {RegisterToken, "acc"}};
//   test_architecture_text(instr.opcode, instr.objmode, 0x0, want);
// };
//
// TEST(TestInstructionText, AbstcAcc) {
//   const auto instr = TIC28X::AbstcAcc();
//   const std::vector<BN::InstructionTextToken> want = {
//       {InstructionToken, "abstc"}, {TextToken, " "}, {RegisterToken, "acc"}};
//   test_architecture_text(instr.opcode, instr.objmode, 0x0, want);
// };

// Vashl32Vra5bit - VCU Arithmetic Shift Left 32-bit
// Format: vashl32 VRn << #imm5
struct Vashl32Vra5bitTestCase {
  uint8_t regA;        // VRa register (0-7)
  uint8_t imm5;        // 5-bit immediate (0-31)
  std::string regStr;  // expected register string
  std::string immStr;  // expected immediate string
};

class TestVashl32Vra5bitText
    : public ::testing::TestWithParam<Vashl32Vra5bitTestCase> {};

TEST_P(TestVashl32Vra5bitText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vashl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashl32Vra5bit::SetImm5(tc.imm5);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vashl32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {TextToken, " "},
      {OperationToken, "<<"},
      {TextToken, " "},
      {TextToken, "#"},
      {IntegerToken, tc.immStr},
  };

  test_architecture_text(opcode, TIC28X::Vashl32Vra5bit::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vashl32Vra5bit, TestVashl32Vra5bitText,
    ::testing::Values(
        // Test VR0 with shift 0
        Vashl32Vra5bitTestCase{0, 0, "vr0", "0x0"},
        // Test VR4 with shift 16 (example from documentation)
        Vashl32Vra5bitTestCase{4, 16, "vr4", "0x10"},
        // Test VR7 with max shift 31
        Vashl32Vra5bitTestCase{7, 31, "vr7", "0x1f"},
        // Test VR2 with shift 8
        Vashl32Vra5bitTestCase{2, 8, "vr2", "0x8"}),
    [](const testing::TestParamInfo<TestVashl32Vra5bitText::ParamType> &info) {
      return std::format("VR{}_shift{}", info.param.regA, info.param.imm5);
    });

// Vashr32Vra5bit - VCU Arithmetic Shift Right 32-bit
// Format: vashr32 VRn >> #imm5
struct Vashr32Vra5bitTestCase {
  uint8_t regA;        // VRa register (0-7)
  uint8_t imm5;        // 5-bit immediate (0-31)
  std::string regStr;  // expected register string
  std::string immStr;  // expected immediate string
};

class TestVashr32Vra5bitText
    : public ::testing::TestWithParam<Vashr32Vra5bitTestCase> {};

TEST_P(TestVashr32Vra5bitText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vashr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vashr32Vra5bit::SetImm5(tc.imm5);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vashr32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {TextToken, " "},
      {OperationToken, ">>"},
      {TextToken, " "},
      {TextToken, "#"},
      {IntegerToken, tc.immStr},
  };

  test_architecture_text(opcode, TIC28X::Vashr32Vra5bit::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vashr32Vra5bit, TestVashr32Vra5bitText,
    ::testing::Values(
        // Test VR0 with shift 0
        Vashr32Vra5bitTestCase{0, 0, "vr0", "0x0"},
        // Test VR1 with shift 16 (example from documentation)
        Vashr32Vra5bitTestCase{1, 16, "vr1", "0x10"},
        // Test VR7 with max shift 31
        Vashr32Vra5bitTestCase{7, 31, "vr7", "0x1f"},
        // Test VR3 with shift 8
        Vashr32Vra5bitTestCase{3, 8, "vr3", "0x8"}),
    [](const testing::TestParamInfo<TestVashr32Vra5bitText::ParamType> &info) {
      return std::format("VR{}_shift{}", info.param.regA, info.param.imm5);
    });

// VbitflipVra - VCU Bit Flip
// Format: vbitflip VRn
struct VbitflipVraTestCase {
  uint8_t regA;        // VRa register (0-7)
  std::string regStr;  // expected register string
};

class TestVbitflipVraText
    : public ::testing::TestWithParam<VbitflipVraTestCase> {};

TEST_P(TestVbitflipVraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::VbitflipVra::SetRegA(tc.regA);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vbitflip"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(opcode, TIC28X::VbitflipVra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VbitflipVra, TestVbitflipVraText,
    ::testing::Values(
        // Test VR0
        VbitflipVraTestCase{0, "vr0"},
        // Test VR1
        VbitflipVraTestCase{1, "vr1"},
        // Test VR4
        VbitflipVraTestCase{4, "vr4"},
        // Test VR7 (max register)
        VbitflipVraTestCase{7, "vr7"}),
    [](const testing::TestParamInfo<TestVbitflipVraText::ParamType> &info) {
      return std::format("VR{}", info.param.regA);
    });
