// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_TEXT_H
#define TIC28X_TEXT_H

#include <binaryninjaapi.h>

#include <vector>

#include "instructions.h"

namespace BN = BinaryNinja;

namespace TIC28X {
struct RegTextInfo {
  uint8_t regnum;  // register value (see registers.h)
  // decorators (@, *, --, ++, %)
  bool direct, indirect, predec, preinc, postdec, postinc, circular;
  bool sub;        // prepend a - token to the register
  bool add;        // prepend a + token to the register
  bool is_offset;  // wrap in []
};

struct ConstTextInfo {
  uint64_t value;   // constant value
  size_t nbits;     // number of bits in the constant
  bool is_signed;   // signed or unsigned
  bool is_address;  // address (@) vs. constant value (#)
  bool is_offset;   // wrap in []
  bool is_memio;    // wrap in *()
};

struct LocTextInfo {
  uint8_t loc;        // 8-bit location code
  AddressMode amode;  // current addressing mode
};

inline void OpText(const std::string& instr,
                   std::vector<BN::InstructionTextToken>& result);
inline void SpaceText(std::vector<BN::InstructionTextToken>& result);
inline void OpsepText(std::vector<BN::InstructionTextToken>& result);
inline void RegCombineText(std::vector<BN::InstructionTextToken>& result);
inline void LshiftText(std::vector<BN::InstructionTextToken>& result);
void RegText(RegTextInfo rti, std::vector<BN::InstructionTextToken>& result);
void ConstText(const ConstTextInfo& cti,
               std::vector<BN::InstructionTextToken>& result);
bool loc_text_helper(const LocTextInfo& lti,
                     std::vector<BN::InstructionTextToken>& result);
bool Loc16Text(const LocTextInfo& lti,
               std::vector<BN::InstructionTextToken>& result);
bool Loc32Text(const LocTextInfo& lti,
               std::vector<BN::InstructionTextToken>& result);
void CondText(uint8_t cond, std::vector<BN::InstructionTextToken>& result);
void ModeText(uint8_t mode, std::vector<BN::InstructionTextToken>& result);
void ProductShiftModeText(uint8_t mode, AddressMode amode,
                          std::vector<BN::InstructionTextToken>& result);
void TextOpXarnConst8(char* op_name, uint8_t n, uint8_t const8,
                      std::vector<BN::InstructionTextToken>& result);
bool TextOpLoc32Xarn(char* op_name, uint8_t n, uint8_t loc32, AddressMode amode,
                     std::vector<BN::InstructionTextToken>& result);
bool TextOpXarnLoc32(char* op_name, uint8_t n, uint8_t loc32, AddressMode amode,
                     std::vector<BN::InstructionTextToken>& result);
void TextOpXarnConst22(char* op_name, uint8_t n, uint32_t const22,
                       std::vector<BN::InstructionTextToken>& result);
bool TextOpArnLoc16(char* op_name, uint8_t n, uint8_t loc16, AddressMode amode,
                    std::vector<BN::InstructionTextToken>& result);
}  // namespace TIC28X

#endif  // TIC28X_TEXT_H
