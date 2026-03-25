// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "text.h"

#include <binaryninjaapi.h>
#include <gtest/gtest.h>

#include <format>

#include "architecture.h"
#include "conditions.h"
#include "instructions.h"
#include "registers.h"

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

// ============================================================================
// Helper Function Unit Tests
// ============================================================================

// --- ConstText Tests ---

TEST(ConstTextTest, UnsignedConstant) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText({.value = 0x1A, .nbits = 8}, result);
  EXPECT_EQ(tokens_to_string(result), "#0x1a");
  EXPECT_EQ(result[0].type, TextToken);     // "#"
  EXPECT_EQ(result[1].type, IntegerToken);  // "0x1a"
}

TEST(ConstTextTest, SignedPositive) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText({.value = 0x05, .nbits = 8, .is_signed = true}, result);
  EXPECT_EQ(tokens_to_string(result), "#0x5");
}

TEST(ConstTextTest, SignedNegative) {
  std::vector<BN::InstructionTextToken> result;
  // 0x80 with 8 bits = -128 (sign bit set)
  TIC28X::ConstText({.value = 0x80, .nbits = 8, .is_signed = true}, result);
  const auto text = tokens_to_string(result);
  EXPECT_TRUE(text.find("#0x") != std::string::npos);
  // Should be sign-extended: high bits all 1s
  EXPECT_EQ(result[1].value, static_cast<uint64_t>(int64_t(-128)));
}

TEST(ConstTextTest, AddressMode) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText({.value = 0x3F, .nbits = 6, .is_address = true}, result);
  EXPECT_EQ(tokens_to_string(result), "@0x3f");
  EXPECT_EQ(result[0].type, TextToken);             // "@"
  EXPECT_EQ(result[1].type, PossibleAddressToken);  // "0x3f"
}

TEST(ConstTextTest, OffsetMode) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText({.value = 3, .nbits = 3, .is_offset = true}, result);
  EXPECT_EQ(tokens_to_string(result), "[0x3]");
  EXPECT_EQ(result[0].type, OperationToken);  // "["
  EXPECT_EQ(result[1].type, IntegerToken);    // "0x3"
  EXPECT_EQ(result[2].type, OperationToken);  // "]"
}

TEST(ConstTextTest, MemioMode) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText({.value = 0x20, .nbits = 8, .is_memio = true}, result);
  EXPECT_EQ(tokens_to_string(result), "*(0x20)");
}

TEST(ConstTextTest, SignedOffset) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ConstText(
      {.value = 0xFF, .nbits = 8, .is_signed = true, .is_offset = true},
      result);
  EXPECT_EQ(result[0].type, OperationToken);  // "["
  EXPECT_EQ(result[2].type, OperationToken);  // "]"
}

// --- RegText Tests ---

TEST(RegTextTest, PlainRegister) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText({.regnum = TIC28X::Registers::ACC}, result);
  EXPECT_EQ(tokens_to_string(result), "acc");
  EXPECT_EQ(result[0].type, RegisterToken);
}

TEST(RegTextTest, DirectDecorator) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText({.regnum = TIC28X::Registers::AR0, .direct = true}, result);
  EXPECT_EQ(tokens_to_string(result), "@ar0");
  EXPECT_EQ(result[0].type, OperationToken);  // "@"
  EXPECT_EQ(result[1].type, RegisterToken);   // "ar0"
}

TEST(RegTextTest, IndirectPostInc) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText(
      {.regnum = TIC28X::Registers::XAR0, .indirect = true, .postinc = true},
      result);
  EXPECT_EQ(tokens_to_string(result), "*xar0++");
}

TEST(RegTextTest, IndirectPreDec) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText(
      {.regnum = TIC28X::Registers::XAR3, .indirect = true, .predec = true},
      result);
  EXPECT_EQ(tokens_to_string(result), "*--xar3");
}

TEST(RegTextTest, IndirectAddWithOffset) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText(
      {.regnum = TIC28X::Registers::XAR2, .indirect = true, .add = true},
      result);
  EXPECT_EQ(tokens_to_string(result), "*+xar2");
}

TEST(RegTextTest, SubDecorator) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText(
      {.regnum = TIC28X::Registers::SP, .indirect = true, .sub = true}, result);
  EXPECT_EQ(tokens_to_string(result), "*-sp");
}

TEST(RegTextTest, CircularPostInc) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText({.regnum = TIC28X::Registers::AR6,
                   .indirect = true,
                   .postinc = true,
                   .circular = true},
                  result);
  EXPECT_EQ(tokens_to_string(result), "*ar6%++");
}

TEST(RegTextTest, IsOffsetWrapping) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::RegText({.regnum = TIC28X::Registers::AR0, .is_offset = true},
                  result);
  EXPECT_EQ(tokens_to_string(result), "[ar0]");
  EXPECT_EQ(result[0].type, OperationToken);  // "["
  EXPECT_EQ(result[2].type, OperationToken);  // "]"
}

// --- CondText Tests ---

TEST(CondTextTest, AllConditions) {
  for (uint8_t cond = 0; cond <= 0xF; ++cond) {
    std::vector<BN::InstructionTextToken> result;
    TIC28X::CondText(cond, result);
    ASSERT_EQ(result.size(), 1u) << "cond=" << static_cast<int>(cond);
    EXPECT_EQ(result[0].type, TextToken);
    EXPECT_EQ(result[0].text, TIC28X::Conditions::NAMES.at(cond));
  }
}

TEST(CondTextTest, MasksToLow4Bits) {
  std::vector<BN::InstructionTextToken> result;
  // 0x1F & 0xF = 0xF = UNC
  TIC28X::CondText(0x1F, result);
  EXPECT_EQ(result[0].text, "unc");
}

// --- ModeText Tests ---

TEST(ModeTextTest, EmptyMode) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ModeText(0x00, result);
  EXPECT_TRUE(result.empty());
}

TEST(ModeTextTest, SingleFlagSXM) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ModeText(0x01, result);
  EXPECT_EQ(tokens_to_string(result), "sxm");
}

TEST(ModeTextTest, SingleFlagVMAP) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ModeText(0x80, result);
  EXPECT_EQ(tokens_to_string(result), "vmap");
}

TEST(ModeTextTest, TwoFlags) {
  std::vector<BN::InstructionTextToken> result;
  // SXM (0x01) | C (0x08) = 0x09
  TIC28X::ModeText(0x09, result);
  EXPECT_EQ(tokens_to_string(result), "sxm, c");
}

TEST(ModeTextTest, AllFlags) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ModeText(0xFF, result);
  EXPECT_EQ(tokens_to_string(result),
            "sxm, ovm, tc, c, intm, dbgm, page0, vmap");
}

// --- ProductShiftModeText Tests ---

TEST(ProductShiftModeTextTest, AllCasesAmode0) {
  const std::array<std::string, 8> expected = {"+1", "0",  "-1", "-2",
                                               "-3", "-4", "-5", "-6"};
  for (uint8_t mode = 0; mode < 8; ++mode) {
    std::vector<BN::InstructionTextToken> result;
    TIC28X::ProductShiftModeText(mode, TIC28X::AMODE_0, result);
    ASSERT_EQ(result.size(), 1u) << "mode=" << static_cast<int>(mode);
    EXPECT_EQ(result[0].text, expected[mode]);
  }
}

TEST(ProductShiftModeTextTest, Mode5Amode1) {
  std::vector<BN::InstructionTextToken> result;
  TIC28X::ProductShiftModeText(5, TIC28X::AMODE_1, result);
  EXPECT_EQ(result[0].text, "+4");
}

// ============================================================================
// Addressing Mode Tests (loc_text_helper, Loc16Text, Loc32Text)
// ============================================================================

// --- Direct Addressing ---

TEST(LocTextHelperTest, DirectAmode0) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x00: AMODE_0, direct @0x0
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x00, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@0x0");
}

TEST(LocTextHelperTest, DirectAmode0Max) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x3F: AMODE_0, direct @0x3f (6 bits max)
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x3F, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@0x3f");
}

TEST(LocTextHelperTest, DirectAmode1) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x10: AMODE_1, direct @@0x10 (extra @ prefix)
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x10, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "@@0x10");
}

// --- Stack Addressing ---

TEST(LocTextHelperTest, StackAmode0) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x40: AMODE_0, *-SP[0x0]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x40, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*-sp[0x0]");
}

TEST(LocTextHelperTest, StackAmode0WithOffset) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x55: AMODE_0, *-SP[0x15]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x55, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*-sp[0x15]");
}

TEST(LocTextHelperTest, SpPostInc) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBD: *SP++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBD, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*sp++");
}

TEST(LocTextHelperTest, SpPreDec) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBE: *--SP
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBE, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*--sp");
}

// --- XAR Indirect Addressing ---

TEST(LocTextHelperTest, XarPostInc) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x80: *XAR0++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x80, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*xar0++");
}

TEST(LocTextHelperTest, XarPostIncMax) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x87: *XAR7++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x87, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*xar7++");
}

TEST(LocTextHelperTest, XarPreDec) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x88: *--XAR0
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x88, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*--xar0");
}

TEST(LocTextHelperTest, XarAr0Offset) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x90: *+XAR0[AR0]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x90, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*+xar0[ar0]");
}

TEST(LocTextHelperTest, XarAr1Offset) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x9A: *+XAR2[AR1]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x9A, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*+xar2[ar1]");
}

TEST(LocTextHelperTest, Xar3bitOffsetAmode0) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xC0: AMODE_0, *+XAR0[0x0]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xC0, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*+xar0[0x0]");
}

TEST(LocTextHelperTest, Xar3bitOffsetNonZero) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xFD: AMODE_0, *+XAR5[0x7]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xFD, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*+xar5[0x7]");
}

// --- C2xLP Addressing ---

TEST(LocTextHelperTest, C2xlpStar) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xB8: *
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xB8, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*");
}

TEST(LocTextHelperTest, C2xlpStarInc) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xB9: *++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xB9, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*++");
}

TEST(LocTextHelperTest, C2xlpStarDec) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBA: *--
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBA, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*--");
}

TEST(LocTextHelperTest, C2xlpStar0Inc) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBB: *0++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBB, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*0++");
}

TEST(LocTextHelperTest, C2xlpStar0Dec) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBC: *0--
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBC, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*0--");
}

TEST(LocTextHelperTest, Br0PostInc) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xAE: *BR0++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xAE, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*br0++");
}

TEST(LocTextHelperTest, Br0PostDec) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xAF: *BR0--
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xAF, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*br0--");
}

TEST(LocTextHelperTest, StarArpn) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xB3: *,ARP3
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xB3, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*, arp3");
}

// --- AMODE_1 specific C2xLP modes ---

TEST(LocTextHelperTest, Amode1IncArpn) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xC2: AMODE_1, *++,ARP2
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xC2, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "*++, arp2");
}

TEST(LocTextHelperTest, Amode1DecArpn) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xC9: AMODE_1, *--,ARP1
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xC9, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "*--, arp1");
}

TEST(LocTextHelperTest, Amode1Br0IncArpn) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xE0: AMODE_1, *BR0++,ARP0
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xE0, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "*br0++, arp0");
}

TEST(LocTextHelperTest, Amode1Br0DecArpn) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xEB: AMODE_1, *BR0--,ARP3
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xEB, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "*br0--, arp3");
}

// --- Circular Addressing ---

TEST(LocTextHelperTest, CircularAr6Amode0) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBF: AMODE_0, *AR6%++
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBF, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*ar6%++");
}

TEST(LocTextHelperTest, CircularAr6Amode1) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xBF: AMODE_1, *+XAR6[AR1%++]
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0xBF, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "*+xar6[ar1%++]");
}

// --- AMODE_1 Direct Addressing (0x40-0x7F) ---

TEST(LocTextHelperTest, DirectAmode1_0x40) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x40: AMODE_1, direct @@0x40 (7-bit address)
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x40, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "@@0x40");
}

TEST(LocTextHelperTest, DirectAmode1_0x7F) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x7F: AMODE_1, direct @@0x7f (7-bit max)
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x7F, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "@@0x7f");
}

TEST(LocTextHelperTest, DirectAmode1_7bitValue) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x10: AMODE_1, @@0x10 — verify 7-bit field yields correct value
  EXPECT_TRUE(
      TIC28X::loc_text_helper({.loc = 0x10, .amode = TIC28X::AMODE_1}, result));
  EXPECT_EQ(tokens_to_string(result), "@@0x10");
}

// --- Loc16Text 16-bit register modes ---

TEST(Loc16TextTest, ArN) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xA3: @AR3
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xA3, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@ar3");
}

TEST(Loc16TextTest, AH) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xA8, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@ah");
}

TEST(Loc16TextTest, AL) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xA9, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@al");
}

TEST(Loc16TextTest, PH) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xAA, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@ph");
}

TEST(Loc16TextTest, PL) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xAB, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@pl");
}

TEST(Loc16TextTest, TH) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xAC, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@th");
}

TEST(Loc16TextTest, SP) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0xAD, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@sp");
}

TEST(Loc16TextTest, FallsThruToHelper) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x80: *XAR0++ (handled by loc_text_helper)
  EXPECT_TRUE(
      TIC28X::Loc16Text({.loc = 0x80, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*xar0++");
}

// --- Loc32Text 32-bit register modes ---

TEST(Loc32TextTest, XarN) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0xA5: @XAR5
  EXPECT_TRUE(
      TIC28X::Loc32Text({.loc = 0xA5, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@xar5");
}

TEST(Loc32TextTest, ACC) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc32Text({.loc = 0xA9, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@acc");
}

TEST(Loc32TextTest, P) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc32Text({.loc = 0xAB, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@p");
}

TEST(Loc32TextTest, XT) {
  std::vector<BN::InstructionTextToken> result;
  EXPECT_TRUE(
      TIC28X::Loc32Text({.loc = 0xAC, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "@xt");
}

TEST(Loc32TextTest, FallsThruToHelper) {
  std::vector<BN::InstructionTextToken> result;
  // loc=0x88: *--XAR0 (handled by loc_text_helper)
  EXPECT_TRUE(
      TIC28X::Loc32Text({.loc = 0x88, .amode = TIC28X::AMODE_0}, result));
  EXPECT_EQ(tokens_to_string(result), "*--xar0");
}

// ============================================================================
// Representative Standard C28x Instruction Text Tests
// ============================================================================

// ADD ACC, loc16: opcode 0x81xx, OBJMODE_1
TEST(AddAccLoc16TextTest, DirectAddressing) {
  // add acc, @0x10
  const uint32_t opcode = TIC28X::AddAccLoc16::SetLoc16(0x10);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "add"}, {TextToken, " "},
      {RegisterToken, "acc"},    {OperandSeparatorToken, ", "},
      {TextToken, "@"},          {PossibleAddressToken, "0x10"},
  };
  test_architecture_text(opcode, TIC28X::AddAccLoc16::objmode, 0x0, want);
}

TEST(AddAccLoc16TextTest, XarIndirect) {
  // add acc, *XAR2++ (loc16=0x82)
  const uint32_t opcode = TIC28X::AddAccLoc16::SetLoc16(0x82);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "add"}, {TextToken, " "},
      {RegisterToken, "acc"},    {OperandSeparatorToken, ", "},
      {OperationToken, "*"},     {RegisterToken, "xar2"},
      {OperationToken, "++"},
  };
  test_architecture_text(opcode, TIC28X::AddAccLoc16::objmode, 0x0, want);
}

// SUB AX, loc16: opcode 0x9Exx, OBJMODE_ANY
TEST(SubAxLoc16TextTest, SubAL) {
  // sub al, @0x0 (x=0 => AL)
  const uint32_t opcode =
      TIC28X::SubAxLoc16::SetRegAx(0) | TIC28X::SubAxLoc16::SetLoc16(0x00);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "sub"}, {TextToken, " "},
      {RegisterToken, "al"},     {OperandSeparatorToken, ", "},
      {TextToken, "@"},          {PossibleAddressToken, "0x0"},
  };
  test_architecture_text(opcode, TIC28X::SubAxLoc16::objmode, 0x0, want);
}

TEST(SubAxLoc16TextTest, SubAH) {
  // sub ah, @al (x=1 => AH, loc16=0xA9 => @AL)
  const uint32_t opcode =
      TIC28X::SubAxLoc16::SetRegAx(1) | TIC28X::SubAxLoc16::SetLoc16(0xA9);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "sub"}, {TextToken, " "},
      {RegisterToken, "ah"},     {OperandSeparatorToken, ", "},
      {OperationToken, "@"},     {RegisterToken, "al"},
  };
  test_architecture_text(opcode, TIC28X::SubAxLoc16::objmode, 0x0, want);
}

// SB off8, COND: opcode 0x6xxx
TEST(SbOff8CondTextTest, BranchUNC) {
  const uint32_t opcode =
      TIC28X::SbOff8Cond::SetOff8(0x10) | TIC28X::SbOff8Cond::SetCond(0xF);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "sb"}, {TextToken, " "},
      {OperationToken, "["},    {IntegerToken, "0x10"},
      {OperationToken, "]"},    {OperandSeparatorToken, ", "},
      {TextToken, "unc"},
  };
  test_architecture_text(opcode, TIC28X::SbOff8Cond::objmode, 0x0, want);
}

TEST(SbOff8CondTextTest, BranchEQ) {
  const uint32_t opcode =
      TIC28X::SbOff8Cond::SetOff8(0x04) | TIC28X::SbOff8Cond::SetCond(0x1);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "sb"}, {TextToken, " "},
      {OperationToken, "["},    {IntegerToken, "0x4"},
      {OperationToken, "]"},    {OperandSeparatorToken, ", "},
      {TextToken, "eq"},
  };
  test_architecture_text(opcode, TIC28X::SbOff8Cond::objmode, 0x0, want);
}

// ============================================================================
// VCU - Arithmetic Math Instructions
// ============================================================================

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

// ============================================================================
// VCU - Bit Manipulation Instructions
// ============================================================================

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

// Vlshl32Vra5bit - VCU Logical Shift Left 32-bit
// Format: vlshl32 VRn << #imm5
struct Vlshl32Vra5bitTestCase {
  uint8_t regA;        // VRa register (0-7)
  uint8_t imm5;        // 5-bit immediate (0-31)
  std::string regStr;  // expected register string
  std::string immStr;  // expected immediate string
};

class TestVlshl32Vra5bitText
    : public ::testing::TestWithParam<Vlshl32Vra5bitTestCase> {};

TEST_P(TestVlshl32Vra5bitText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshl32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshl32Vra5bit::SetImm5(tc.imm5);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vlshl32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {TextToken, " "},
      {OperationToken, "<<"},
      {TextToken, " "},
      {TextToken, "#"},
      {IntegerToken, tc.immStr},
  };

  test_architecture_text(opcode, TIC28X::Vlshl32Vra5bit::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vlshl32Vra5bit, TestVlshl32Vra5bitText,
    ::testing::Values(
        // Test VR0 with shift 0 (minimum values)
        Vlshl32Vra5bitTestCase{0, 0, "vr0", "0x0"},
        // Test VR0 with shift 16 (example from documentation)
        Vlshl32Vra5bitTestCase{0, 16, "vr0", "0x10"},
        // Test VR7 with max shift 31 (maximum values)
        Vlshl32Vra5bitTestCase{7, 31, "vr7", "0x1f"},
        // Test VR3 with shift 8 (mid-range values)
        Vlshl32Vra5bitTestCase{3, 8, "vr3", "0x8"}),
    [](const testing::TestParamInfo<TestVlshl32Vra5bitText::ParamType> &info) {
      return std::format("VR{}_shift{}", info.param.regA, info.param.imm5);
    });

// Vlshr32Vra5bit - VCU Logical Shift Right 32-bit
// Format: vlshr32 VRn >> #imm5
struct Vlshr32Vra5bitTestCase {
  uint8_t regA;        // VRa register (0-7)
  uint8_t imm5;        // 5-bit immediate (0-31)
  std::string regStr;  // expected register string
  std::string immStr;  // expected immediate string
};

class TestVlshr32Vra5bitText
    : public ::testing::TestWithParam<Vlshr32Vra5bitTestCase> {};

TEST_P(TestVlshr32Vra5bitText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vlshr32Vra5bit::SetRegA(tc.regA) |
                          TIC28X::Vlshr32Vra5bit::SetImm5(tc.imm5);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vlshr32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {TextToken, " "},
      {OperationToken, ">>"},
      {TextToken, " "},
      {TextToken, "#"},
      {IntegerToken, tc.immStr},
  };

  test_architecture_text(opcode, TIC28X::Vlshr32Vra5bit::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vlshr32Vra5bit, TestVlshr32Vra5bitText,
    ::testing::Values(
        // Test VR0 with shift 0 (minimum values)
        Vlshr32Vra5bitTestCase{0, 0, "vr0", "0x0"},
        // Test VR0 with shift 16 (example from documentation)
        Vlshr32Vra5bitTestCase{0, 16, "vr0", "0x10"},
        // Test VR7 with max shift 31 (maximum values)
        Vlshr32Vra5bitTestCase{7, 31, "vr7", "0x1f"},
        // Test VR3 with shift 8 (mid-range values)
        Vlshr32Vra5bitTestCase{3, 8, "vr3", "0x8"}),
    [](const testing::TestParamInfo<TestVlshr32Vra5bitText::ParamType> &info) {
      return std::format("VR{}_shift{}", info.param.regA, info.param.imm5);
    });

// ============================================================================
// VCU - Negate Instructions
// ============================================================================

// VnegVra - VCU Negate
// Format: vneg VRn
struct VnegVraTestCase {
  uint8_t regA;        // VRa register (0-7)
  std::string regStr;  // expected register string
};

class TestVnegVraText : public ::testing::TestWithParam<VnegVraTestCase> {};

TEST_P(TestVnegVraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::VnegVra::SetRegA(tc.regA);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vneg"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(opcode, TIC28X::VnegVra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VnegVra, TestVnegVraText,
    ::testing::Values(
        // Test VR0 (minimum register)
        VnegVraTestCase{0, "vr0"},
        // Test VR1
        VnegVraTestCase{1, "vr1"},
        // Test VR4 (mid-range)
        VnegVraTestCase{4, "vr4"},
        // Test VR7 (maximum register)
        VnegVraTestCase{7, "vr7"}),
    [](const testing::TestParamInfo<TestVnegVraText::ParamType> &info) {
      return std::format("VR{}", info.param.regA);
    });

// ============================================================================
// VCU - General Move Instructions
// ============================================================================

// Vmov32VraMem32 - VCU Load 32-bit register from memory
// Format: vmov32 VRa, @mem32
struct Vmov32VraMem32TestCase {
  uint8_t regA;
  uint8_t mem32;
  std::string regStr;
};

class TestVmov32VraMem32Text
    : public ::testing::TestWithParam<Vmov32VraMem32TestCase> {};

TEST_P(TestVmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32VraMem32::SetRegA(tc.regA) |
                          TIC28X::Vmov32VraMem32::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };
  test_architecture_text(opcode, TIC28X::Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vmov32VraMem32, TestVmov32VraMem32Text,
    ::testing::Values(Vmov32VraMem32TestCase{0, 0x00, "vr0"},
                      Vmov32VraMem32TestCase{1, 0x42, "vr1"},
                      Vmov32VraMem32TestCase{7, 0xff, "vr7"},
                      Vmov32VraMem32TestCase{8, 0x10, "vr8"}),
    [](const testing::TestParamInfo<TestVmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// Vmov32Mem32Vra - VCU Store 32-bit register to memory
// Format: vmov32 @mem32, VRa
struct Vmov32Mem32VraTestCase {
  uint8_t regA;
  uint8_t mem32;
  std::string regStr;
};

class TestVmov32Mem32VraText
    : public ::testing::TestWithParam<Vmov32Mem32VraTestCase> {};

TEST_P(TestVmov32Mem32VraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::Vmov32Mem32Vra::SetRegA(tc.regA) |
                          TIC28X::Vmov32Mem32Vra::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
      {OperandSeparatorToken, ", "},
      {RegisterToken, tc.regStr},
  };
  test_architecture_text(opcode, TIC28X::Vmov32Mem32Vra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vmov32Mem32Vra, TestVmov32Mem32VraText,
    ::testing::Values(Vmov32Mem32VraTestCase{0, 0x00, "vr0"},
                      Vmov32Mem32VraTestCase{1, 0x42, "vr1"},
                      Vmov32Mem32VraTestCase{7, 0xff, "vr7"},
                      Vmov32Mem32VraTestCase{8, 0x10, "vr8"}),
    [](const testing::TestParamInfo<TestVmov32Mem32VraText::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// ============================================================================
// VCU - Complex Math Instructions
// ============================================================================

// VcaddVr5Vr4Vr3Vr2 - VCU Complex Add
// Format: vcadd VR5, VR4, VR3, VR2
// All operands are fixed implicit registers - opcode 0xE502 is an exact match.
TEST(VcaddVr5Vr4Vr3Vr2TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcadd"}, {TextToken, " "},
      {RegisterToken, "vr5"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
  };

  test_architecture_text(TIC28X::VcaddVr5Vr4Vr3Vr2::opcode,
                         TIC28X::VcaddVr5Vr4Vr3Vr2::objmode, 0x0, want);
}

// VcaddVr7Vr6Vr5Vr4 - VCU Complex Add (VR7, VR6, VR5, VR4)
// Format: vcadd VR7, VR6, VR5, VR4
// All operands are fixed implicit registers - opcode 0xE52A is an exact match.
TEST(VcaddVr7Vr6Vr5Vr4TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcadd"}, {TextToken, " "},
      {RegisterToken, "vr7"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr6"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr5"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
  };

  test_architecture_text(TIC28X::VcaddVr7Vr6Vr5Vr4::opcode,
                         TIC28X::VcaddVr7Vr6Vr5Vr4::objmode, 0x0, want);
}

// VcaddVr5Vr4Vr3Vr2Vmov32VraMem32 - VCU Complex Add with parallel VMOV32
// Format: vcadd VR5, VR4, VR3, VR2 || vmov32 VRa, mem32
struct VcaddVr5Vr4Vr3Vr2Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcaddVr5Vr4Vr3Vr2Vmov32VraMem32Text
    : public ::testing::TestWithParam<VcaddVr5Vr4Vr3Vr2Vmov32VraMem32TestCase> {
};

TEST_P(TestVcaddVr5Vr4Vr3Vr2Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  // mem32=0 selects direct addressing mode @0x0 (bits [7:0] = 0x00)
  const uint32_t opcode =
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(0);
  // With AMODE_0 and mem32=0, Loc32Text emits: TextToken("@") +
  // PossibleAddressToken("0x0")
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcadd"},
      {TextToken, " "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, "0x0"},
  };

  test_architecture_text(
      opcode, TIC28X::VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcaddVr5Vr4Vr3Vr2Vmov32VraMem32, TestVcaddVr5Vr4Vr3Vr2Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register)
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32TestCase{0, "vr0"},
        // Test VR1
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32TestCase{1, "vr1"},
        // Test VR7 (maximum register)
        VcaddVr5Vr4Vr3Vr2Vmov32VraMem32TestCase{7, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcaddVr5Vr4Vr3Vr2Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}", info.param.regA);
    });

// VccmacVr5Vr4Vr3Vr2Vr1Vr0 - VCU Complex Conjugate Multiply and Accumulate
// Format: vccmac VR5, VR4, VR3, VR2, VR1, VR0
// All operands are fixed implicit registers - opcode 0xE50F is an exact match.
TEST(VccmacVr5Vr4Vr3Vr2Vr1Vr0TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmac"}, {TextToken, " "},
      {RegisterToken, "vr5"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
  };

  test_architecture_text(TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0::opcode,
                         TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0::objmode, 0x0, want);
}

// VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32 - VCU Complex Conjugate Multiply and
// Accumulate with parallel VMOV32
// Format: vccmac VR5, VR4, VR3, VR2, VR1, VR0 || vmov32 VRa, mem32
struct VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text
    : public ::testing::TestWithParam<
          VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase> {};

TEST_P(TestVccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  // With AMODE_0 and direct addressing, Loc32Text emits: TextToken("@") +
  // PossibleAddressToken("0x<mem32>")
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmac"},
      {TextToken, " "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(
      opcode, TIC28X::VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::objmode, 0x0,
      want);
}

INSTANTIATE_TEST_SUITE_P(
    VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32,
    TestVccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        VccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVccmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc - VCU Complex Conjugate Multiply and
// Accumulate (repeated form)
// Format: vccmac VR7, VR6, VR5, VR4, mem32, *XAR7++
struct VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase {
  uint8_t mem32;  // mem32 addressing mode byte
};

class TestVccmacVr7Vr6Vr5Vr4Mem32Xar7PostincText
    : public ::testing::TestWithParam<
          VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase> {};

TEST_P(TestVccmacVr7Vr6Vr5Vr4Mem32Xar7PostincText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmac"},
      {TextToken, " "},
      {RegisterToken, "vr7"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr6"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
      {OperandSeparatorToken, ", "},
      {OperationToken, "*"},
      {RegisterToken, "xar7"},
      {OperationToken, "++"},
  };

  test_architecture_text(
      opcode, TIC28X::VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VccmacVr7Vr6Vr5Vr4Mem32Xar7Postinc,
    TestVccmacVr7Vr6Vr5Vr4Mem32Xar7PostincText,
    ::testing::Values(
        // Test minimum mem32
        VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0x00},
        // Test mid-range mem32
        VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0x42},
        // Test maximum mem32
        VccmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0xff}),
    [](const testing::TestParamInfo<
        TestVccmacVr7Vr6Vr5Vr4Mem32Xar7PostincText::ParamType> &info) {
      return std::format("mem{:02x}", info.param.mem32);
    });

// VccmpyVr3Vr2Vr1Vr0 - VCU Complex Conjugate 16x16=32-bit Multiply
// Format: vccmpy VR3, VR2, VR1, VR0
// All operands are fixed implicit registers - opcode is an exact match.
TEST(VccmpyVr3Vr2Vr1Vr0TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmpy"}, {TextToken, " "},
      {RegisterToken, "vr3"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},       {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
  };

  test_architecture_text(TIC28X::VccmpyVr3Vr2Vr1Vr0::opcode,
                         TIC28X::VccmpyVr3Vr2Vr1Vr0::objmode, 0x0, want);
}

// VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32 - VCU Complex Conjugate Multiply with
// parallel 32-bit store
// Format: vccmpy VR3, VR2, VR1, VR0 || vmov32 mem32, VRa
struct VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text
    : public ::testing::TestWithParam<
          VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase> {};

TEST_P(TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  // Store direction: vmov32 mem32, VRa (mem32 before register)
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmpy"},
      {TextToken, " "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
      {OperandSeparatorToken, ", "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(
      opcode, TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32, TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load - Complex Conjugate Multiply with
// Parallel Load
// Format: vccmpy VR3, VR2, VR1, VR0 || vmov32 VRa, mem32

class TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadText
    : public ::testing::TestWithParam<
          VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase> {};

TEST_P(TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetRegA(tc.regA) |
      TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::SetMem32(tc.mem32);
  // Load direction: vmov32 VRa, mem32 (register before mem32)
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccmpy"},
      {TextToken, " "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(
      opcode, TIC28X::VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32Load,
    TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadText,
    ::testing::Values(VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{0, 0x00, "vr0"},
                      VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{1, 0x42, "vr1"},
                      VccmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVccmpyVr3Vr2Vr1Vr0Vmov32VraMem32LoadText::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// VcconVra - Complex Conjugate
// Format: vccon VRa

struct VcconVraTestCase {
  uint8_t regA;
  std::string regStr;
};

class TestVcconVraText : public ::testing::TestWithParam<VcconVraTestCase> {};

TEST_P(TestVcconVraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::VcconVra::SetRegA(tc.regA);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vccon"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(opcode, TIC28X::VcconVra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcconVra, TestVcconVraText,
    ::testing::Values(VcconVraTestCase{0, "vr0"}, VcconVraTestCase{1, "vr1"},
                      VcconVraTestCase{4, "vr4"}, VcconVraTestCase{7, "vr7"}),
    [](const testing::TestParamInfo<TestVcconVraText::ParamType> &info) {
      return std::format("VR{}", info.param.regA);
    });

// Vcdadd16Vr5Vr4Vr3Vr2 - VCU Complex 16+32=16 Addition
// Format: vcdadd16 VR5, VR4, VR3, VR2
// All operands are fixed implicit registers - opcode 0xE504 is an exact match.
TEST(Vcdadd16Vr5Vr4Vr3Vr2TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcdadd16"}, {TextToken, " "},
      {RegisterToken, "vr5"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
  };

  test_architecture_text(TIC28X::Vcdadd16Vr5Vr4Vr3Vr2::opcode,
                         TIC28X::Vcdadd16Vr5Vr4Vr3Vr2::objmode, 0x0, want);
}

// Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32 - VCU Complex 16+32=16 Addition with
// parallel VMOV32
// Format: vcdadd16 VR5, VR4, VR3, VR2 || vmov32 VRa, mem32
struct Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32Text
    : public ::testing::TestWithParam<
          Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32TestCase> {};

TEST_P(TestVcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);
  // With AMODE_0 and direct addressing, Loc32Text emits: TextToken("@") +
  // PossibleAddressToken("0x<mem32>")
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcdadd16"},
      {TextToken, " "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(
      opcode, TIC28X::Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32,
    TestVcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        Vcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcdadd16Vr5Vr4Vr3Vr2Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// Vcdsub16Vr6Vr4Vr3Vr2 - VCU Complex 16-32=16 Subtraction
// Format: vcdsub16 VR6, VR4, VR3, VR2
// All operands are fixed implicit registers - opcode 0xE505 is an exact match.
TEST(Vcdsub16Vr6Vr4Vr3Vr2TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcdsub16"}, {TextToken, " "},
      {RegisterToken, "vr6"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},         {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
  };

  test_architecture_text(TIC28X::Vcdsub16Vr6Vr4Vr3Vr2::opcode,
                         TIC28X::Vcdsub16Vr6Vr4Vr3Vr2::objmode, 0x0, want);
}

// Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32 - VCU Complex 16-32=16 Subtraction with
// parallel VMOV32 load
// Format: vcdsub16 VR6, VR4, VR3, VR2 || vmov32 VRa, mem32

struct Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32Text
    : public ::testing::TestWithParam<
          Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32TestCase> {};

TEST_P(TestVcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcdsub16"},
      {TextToken, " "},
      {RegisterToken, "vr6"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(
      opcode, TIC28X::Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32,
    TestVcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        Vcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcdsub16Vr6Vr4Vr3Vr2Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// VcflipVra - VCU Conjugate Flip
// Format: vcflip VRa

struct VcflipVraTestCase {
  uint8_t regA;
  std::string regStr;
};

class TestVcflipVraText : public ::testing::TestWithParam<VcflipVraTestCase> {};

TEST_P(TestVcflipVraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::VcflipVra::SetRegA(tc.regA);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcflip"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(opcode, TIC28X::VcflipVra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcflipVra, TestVcflipVraText,
    ::testing::Values(VcflipVraTestCase{0, "vr0"}, VcflipVraTestCase{1, "vr1"},
                      VcflipVraTestCase{4, "vr4"}, VcflipVraTestCase{7, "vr7"}),
    [](const testing::TestParamInfo<TestVcflipVraText::ParamType> &info) {
      return std::format("VR{}", info.param.regA);
    });

// VcmacVr5Vr4Vr3Vr2Vr1Vr0 - VCU Complex Multiply and Accumulate
// Format: vcmac VR5, VR4, VR3, VR2, VR1, VR0
// All operands are fixed implicit registers - opcode 0xE501 is an exact match.
TEST(VcmacVr5Vr4Vr3Vr2Vr1Vr0TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmac"}, {TextToken, " "},
      {RegisterToken, "vr5"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
  };

  test_architecture_text(TIC28X::VcmacVr5Vr4Vr3Vr2Vr1Vr0::opcode,
                         TIC28X::VcmacVr5Vr4Vr3Vr2Vr1Vr0::objmode, 0x0, want);
}

// VcmacVr7Vr6Vr5Vr4Mem32Xar7Postinc - VCU Complex Multiply and Accumulate
// (repeated form)
// Format: vcmac VR7, VR6, VR5, VR4, mem32, *XAR7++
struct VcmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase {
  uint8_t mem32;
};

class TestVcmacVr7Vr6Vr5Vr4Mem32Xar7PostincText
    : public ::testing::TestWithParam<
          VcmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase> {};

TEST_P(TestVcmacVr7Vr6Vr5Vr4Mem32Xar7PostincText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmac"},
      {TextToken, " "},
      {RegisterToken, "vr7"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr6"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
      {OperandSeparatorToken, ", "},
      {OperationToken, "*"},
      {RegisterToken, "xar7"},
      {OperationToken, "++"},
  };

  test_architecture_text(
      opcode, TIC28X::VcmacVr7Vr6Vr5Vr4Mem32Xar7Postinc::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcmacVr7Vr6Vr5Vr4Mem32Xar7Postinc,
    TestVcmacVr7Vr6Vr5Vr4Mem32Xar7PostincText,
    ::testing::Values(
        // Test minimum mem32
        VcmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0x00},
        // Test mid-range mem32
        VcmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0x42},
        // Test maximum mem32
        VcmacVr7Vr6Vr5Vr4Mem32Xar7PostincTestCase{0xff}),
    [](const testing::TestParamInfo<
        TestVcmacVr7Vr6Vr5Vr4Mem32Xar7PostincText::ParamType> &info) {
      return std::format("mem{:02x}", info.param.mem32);
    });

// VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32 - VCU Complex Multiply and
// Accumulate with parallel VMOV32
// Format: vcmac VR5, VR4, VR3, VR2, VR1, VR0 || vmov32 VRa, mem32
struct VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text
    : public ::testing::TestWithParam<
          VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase> {};

TEST_P(TestVcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmac"},
      {TextToken, " "},
      {RegisterToken, "vr5"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr4"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(opcode,
                         TIC28X::VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32::objmode,
                         0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32,
    TestVcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        VcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcmacVr5Vr4Vr3Vr2Vr1Vr0Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// VcmagVrbVra - Complex Magnitude
// Format: vcmag VRb, VRa

struct VcmagVrbVraTestCase {
  uint8_t regA;
  uint8_t regB;
  std::string regAStr;
  std::string regBStr;
};

class TestVcmagVrbVraText
    : public ::testing::TestWithParam<VcmagVrbVraTestCase> {};

TEST_P(TestVcmagVrbVraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode = TIC28X::VcmagVrbVra::SetRegA(tc.regA) |
                          TIC28X::VcmagVrbVra::SetRegB(tc.regB);
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmag"}, {TextToken, " "},
      {RegisterToken, tc.regBStr}, {OperandSeparatorToken, ", "},
      {RegisterToken, tc.regAStr},
  };

  test_architecture_text(opcode, TIC28X::VcmagVrbVra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcmagVrbVra, TestVcmagVrbVraText,
    ::testing::Values(VcmagVrbVraTestCase{0, 0, "vr0", "vr0"},
                      VcmagVrbVraTestCase{7, 0, "vr7", "vr0"},
                      VcmagVrbVraTestCase{0, 7, "vr0", "vr7"},
                      VcmagVrbVraTestCase{3, 5, "vr3", "vr5"},
                      VcmagVrbVraTestCase{7, 7, "vr7", "vr7"}),
    [](const testing::TestParamInfo<TestVcmagVrbVraText::ParamType> &info) {
      return std::format("VRb{}_VRa{}", info.param.regB, info.param.regA);
    });

// VcmpyVr3Vr2Vr1Vr0 - VCU Complex 16x16=32-bit Multiply
// Format: vcmpy VR3, VR2, VR1, VR0
// All operands are fixed implicit registers - opcode is an exact match.
TEST(VcmpyVr3Vr2Vr1Vr0TextTest, FixedRegisters) {
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmpy"}, {TextToken, " "},
      {RegisterToken, "vr3"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
  };

  test_architecture_text(TIC28X::VcmpyVr3Vr2Vr1Vr0::opcode,
                         TIC28X::VcmpyVr3Vr2Vr1Vr0::objmode, 0x0, want);
}

// VcmpyVr3Vr2Vr1Vr0Vmov32Mem32Vra - VCU Complex Multiply with parallel
// 32-bit store
// Format: vcmpy VR3, VR2, VR1, VR0 || vmov32 mem32, VRa
struct VcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraTestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraText
    : public ::testing::TestWithParam<VcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraTestCase> {
};

TEST_P(TestVcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraText, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32Mem32Vra::SetRegA(tc.regA) |
      TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32Mem32Vra::SetMem32(tc.mem32);
  // Store direction: vmov32 mem32, VRa (mem32 before register)
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmpy"},
      {TextToken, " "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
      {OperandSeparatorToken, ", "},
      {RegisterToken, tc.regStr},
  };

  test_architecture_text(
      opcode, TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32Mem32Vra::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcmpyVr3Vr2Vr1Vr0Vmov32Mem32Vra, TestVcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraText,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraTestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraTestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraTestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcmpyVr3Vr2Vr1Vr0Vmov32Mem32VraText::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });

// ============================================================================
// VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32 - VCU Complex Multiply with parallel
// 32-bit load
// Format: vcmpy VR3, VR2, VR1, VR0 || vmov32 VRa, mem32
struct VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase {
  uint8_t regA;        // VRa register index (0-7)
  uint8_t mem32;       // mem32 addressing mode byte
  std::string regStr;  // expected register string (e.g. "vr0")
};

class TestVcmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text
    : public ::testing::TestWithParam<VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase> {
};

TEST_P(TestVcmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text, TestInstructionText) {
  const auto &tc = GetParam();
  const uint32_t opcode =
      TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetRegA(tc.regA) |
      TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32::SetMem32(tc.mem32);
  // Load direction: vmov32 VRa, mem32 (register before mem32)
  const std::vector<BN::InstructionTextToken> want = {
      {InstructionToken, "vcmpy"},
      {TextToken, " "},
      {RegisterToken, "vr3"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr2"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr1"},
      {OperandSeparatorToken, ", "},
      {RegisterToken, "vr0"},
      {TextToken, " || "},
      {InstructionToken, "vmov32"},
      {TextToken, " "},
      {RegisterToken, tc.regStr},
      {OperandSeparatorToken, ", "},
      {TextToken, "@"},
      {PossibleAddressToken, std::format("0x{:x}", tc.mem32)},
  };

  test_architecture_text(
      opcode, TIC28X::VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32::objmode, 0x0, want);
}

INSTANTIATE_TEST_SUITE_P(
    VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32, TestVcmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text,
    ::testing::Values(
        // Test VR0 (minimum register, minimum mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{0, 0x00, "vr0"},
        // Test VR1 (mid-range mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{1, 0x42, "vr1"},
        // Test VR7 (maximum register, maximum mem32)
        VcmpyVr3Vr2Vr1Vr0Vmov32VraMem32TestCase{7, 0xff, "vr7"}),
    [](const testing::TestParamInfo<
        TestVcmpyVr3Vr2Vr1Vr0Vmov32VraMem32Text::ParamType> &info) {
      return std::format("VR{}_mem{:02x}", info.param.regA, info.param.mem32);
    });
