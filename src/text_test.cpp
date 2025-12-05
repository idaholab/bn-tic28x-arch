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
        static_cast<uint8_t>((opcode & 0xFF00) >> 8),
        static_cast<uint8_t>(opcode & 0xFF),
        0,
        0,
    };
  } else {  // 4-byte opcode
    expected_len = 4;
    full_op = {
        static_cast<uint8_t>((opcode & 0xFF000000) >> 24),
        static_cast<uint8_t>((opcode & 0xFF0000) >> 16),
        static_cast<uint8_t>((opcode & 0xFF00) >> 8),
        static_cast<uint8_t>(opcode & 0xFF),
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
