// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "text.h"

#include <binaryninjaapi.h>

#include <format>

#include "conditions.h"
#include "flags.h"
#include "instructions.h"
#include "registers.h"
#include "util.h"

namespace TIC28X {
// Temporary implementation until all instructions are implemented; TODO: remove
bool Instruction::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       AddressMode amode) {
  return false;
}

/* Helper Functions */

inline void OpText(const std::string& instr,
                   std::vector<BN::InstructionTextToken>& result) {
  result.emplace_back(InstructionToken, instr);
}

inline void SpaceText(std::vector<BN::InstructionTextToken>& result) {
  result.emplace_back(TextToken, " ");
}

inline void OpsepText(std::vector<BN::InstructionTextToken>& result) {
  result.emplace_back(OperandSeparatorToken, ", ");
}

inline void RegCombineText(std::vector<BN::InstructionTextToken>& result) {
  result.emplace_back(TextToken, ":");
}

inline void LshiftText(std::vector<BN::InstructionTextToken>& result) {
  SpaceText(result);
  result.emplace_back(OperationToken, "<<");
  SpaceText(result);
}

void RegText(const RegTextInfo rti,
             std::vector<BN::InstructionTextToken>& result) {
  if (rti.is_offset) {
    result.emplace_back(OperationToken, "[");
  }
  if (rti.indirect) {
    result.emplace_back(OperationToken, "*");
  }
  if (rti.direct) {
    result.emplace_back(OperationToken, "@");
  }
  if (rti.sub) {
    result.emplace_back(OperationToken, "-");
  }
  if (rti.add) {
    result.emplace_back(OperationToken, "+");
  }
  if (rti.predec) {
    result.emplace_back(OperationToken, "--");
  }
  if (rti.preinc) {
    result.emplace_back(OperationToken, "++");
  }
  result.emplace_back(RegisterToken, Registers::NAMES.at(rti.regnum));
  if (rti.circular) {
    result.emplace_back(OperationToken, "%");
  }
  if (rti.postdec) {
    result.emplace_back(OperationToken, "--");
  }
  if (rti.postinc) {
    result.emplace_back(OperationToken, "++");
  }
  if (rti.is_offset) {
    result.emplace_back(OperationToken, "]");
  }
}

void ConstText(const ConstTextInfo& cti,
               std::vector<BN::InstructionTextToken>& result) {
  const auto mask = CreateLowerBitMask(cti.nbits);

  if (cti.is_signed) {
    int64_t value = cti.value & mask;
    if (cti.value & 1 << (cti.nbits - 1)) {  // check sign bit
      value |= ~mask;  // set upper bits to 1 for sign extension
    }

    if (cti.is_offset) {
      result.emplace_back(OperationToken, "[");
      result.emplace_back(IntegerToken, std::format("0x{:x}", value), value);
      result.emplace_back(OperationToken, "]");
    } else {
      result.emplace_back(TextToken, "#");
      result.emplace_back(IntegerToken, std::format("0x{:x}", value), value);
    }
  } else {
    const uint64_t value = cti.value & mask;

    if (cti.is_address) {
      result.emplace_back(TextToken, "@");
      result.emplace_back(PossibleAddressToken, std::format("0x{:x}", value),
                          value);
    } else if (cti.is_offset) {
      result.emplace_back(OperationToken, "[");
      result.emplace_back(IntegerToken, std::format("0x{:x}", value), value);
      result.emplace_back(OperationToken, "]");
    } else if (cti.is_memio) {
      result.emplace_back(OperationToken, "*");
      result.emplace_back(TextToken, "(");
      result.emplace_back(PossibleAddressToken, std::format("0x{:x}", value),
                          value);
      result.emplace_back(TextToken, ")");
    } else {
      result.emplace_back(TextToken, "#");
      result.emplace_back(IntegerToken, std::format("0x{:x}", value), value);
    }
  }
}

bool loc_text_helper(const LocTextInfo& lti,
                     std::vector<BN::InstructionTextToken>& result) {
  /* Direct Addressing Modes (DP) */
  if ((lti.loc & 0xC0) == 0) {
    if (lti.amode == AMODE_0) {  // AMODE0, code == 0 0 III III, @6bit
      ConstText(ConstTextInfo{.value = lti.loc, .nbits = 6, .is_address = true},
                result);
    } else {  // AMODE1, code == 0 I III III, @@7bit
      result.emplace_back(TextToken, "@");  // AMODE_1 requires extra @
      ConstText(ConstTextInfo{.value = lti.loc, .nbits = 6, .is_address = true},
                result);
    }
    return true;
  }

  /* Stack Addressing Modes (SP) */
  // AMODE0, code == 0 1 III III, *-SP[6bit]
  if ((lti.loc & 0xC0) == 0x40 && lti.amode == AMODE_0) {
    RegText(RegTextInfo{.regnum = Registers::SP, .indirect = true, .sub = true},
            result);
    ConstText(ConstTextInfo{.value = lti.loc, .nbits = 6, .is_offset = true},
              result);
    return true;
  }

  // AMODEx, code == 1 0 111 101, *SP++
  if (lti.loc == 0xBD) {
    RegText(
        RegTextInfo{.regnum = Registers::SP, .indirect = true, .postinc = true},
        result);
    return true;
  }

  // AMODEx, code == 1 0 111 110, *--SP
  if (lti.loc == 0xBE) {
    RegText(
        RegTextInfo{.regnum = Registers::SP, .indirect = true, .predec = true},
        result);
    return true;
  }

  /* C28x Indirect Addressing Modes (XAR0 to XAR7) */
  // AMODEx, code == 1 0 000 AAA, *XARn++
  if ((lti.loc & 0xF8) == 0x80) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .indirect = true, .postinc = true},
            result);
    return true;
  }

  // AMODEx, code == 1 0 001 AAA, *--XARn
  if ((lti.loc & 0xF8) == 0x88) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .indirect = true, .predec = true},
            result);
    return true;
  }

  // AMODEx, code == 1 0 010 AAA, *+XARn[AR0]
  if ((lti.loc & 0xF8) == 0x90) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .indirect = true, .add = true},
            result);
    RegText(RegTextInfo{.regnum = Registers::AR0, .is_offset = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 011 AAA, *+XARn[AR1]
  if ((lti.loc & 0xF8) == 0x98) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .indirect = true, .add = true},
            result);
    RegText(RegTextInfo{.regnum = Registers::AR1, .is_offset = true}, result);
    return true;
  }

  // AMODE0, code == 1 1 III AAA, *+XARn[3bit]
  if ((lti.loc & 0xC0) == 0xC0 && lti.amode == AMODE_0) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    const uint8_t const3 = (lti.loc & 0x38) >> 3;
    RegText(RegTextInfo{.regnum = regnum, .indirect = true, .add = true},
            result);
    ConstText(ConstTextInfo{.value = const3, .nbits = 3, .is_offset = true},
              result);
    return true;
  }

  /* C2xLP Indirect Addressing Modes (ARP, XAR0 to XAR7) */
  // AMODEx, code == 1 0 111 000, *
  if (lti.loc == 0xB8) {
    result.emplace_back(OperationToken, "*");
    return true;
  }

  // AMODEx, code == 1 0 111 001, *++
  if (lti.loc == 0xB9) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(OperationToken, "++");
    return true;
  }

  // AMODEx, code == 1 0 111 010, *--
  if (lti.loc == 0xBA) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(OperationToken, "--");
    return true;
  }

  // AMODEx, code == 1 0 111 011, *0++
  if (lti.loc == 0xBB) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(IntegerToken, "0", 0);
    result.emplace_back(OperationToken, "++");
    return true;
  }

  // AMODEx, code == 1 0 111 100, *0--
  if (lti.loc == 0xBC) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(TextToken, "0");
    result.emplace_back(OperationToken, "--");
    return true;
  }

  // AMODEx, code == 1 0 101 110, *BR0++
  if (lti.loc == 0xAE) {
    RegText(
        RegTextInfo{
            .regnum = Registers::BR0, .indirect = true, .postinc = true},
        result);
    return true;
  }

  // AMODEx, code == 1 0 101 111, *BR0--
  if (lti.loc == 0xAF) {
    RegText(
        RegTextInfo{
            .regnum = Registers::BR0, .indirect = true, .postdec = true},
        result);

    return true;
  }

  // AMODEx, code == 1 0 110 RRR, *,ARPn
  if ((lti.loc & 0xF8) == 0xB0) {
    result.emplace_back(OperationToken, "*");
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 000 RRR, *++,ARPn
  if ((lti.loc & 0xF8) == 0xC0 && lti.amode == AMODE_1) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(OperationToken, "++");
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 001 RRR, *--,ARPn
  if ((lti.loc & 0xF8) == 0xC8 && lti.amode == AMODE_1) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(OperationToken, "--");
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 010 RRR, *0++,ARPn
  if ((lti.loc & 0xF8) == 0xD0 && lti.amode == AMODE_1) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(TextToken, "0");
    result.emplace_back(OperationToken, "++");
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 011 RRR, *0--,ARPn
  if ((lti.loc & 0xF8) == 0xD8 && lti.amode == AMODE_1) {
    result.emplace_back(OperationToken, "*");
    result.emplace_back(TextToken, "0");
    result.emplace_back(OperationToken, "--");
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 100 RRR, *BR0++,ARPn
  if ((lti.loc & 0xF8) == 0xE0 && lti.amode == AMODE_1) {
    RegText(
        RegTextInfo{
            .regnum = Registers::BR0, .indirect = true, .postinc = true},
        result);
    OpsepText(result);
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  // AMODE1, code == 1 1 101 RRR, *BR0--,ARPn
  if ((lti.loc & 0xF8) == 0xE8 && lti.amode == AMODE_1) {
    RegText(
        RegTextInfo{
            .regnum = Registers::BR0, .indirect = true, .postdec = true},
        result);
    result.emplace_back(OperandSeparatorToken, ",");
    result.emplace_back(TextToken, std::format("arp{}", lti.loc & 0x7));
    return true;
  }

  /* Circular Indirect Addressing Modes (XAR6, XAR1) */
  // AMODEx, code == 1 0 111 111
  if (lti.loc == 0xBF) {
    if (lti.amode == AMODE_0) {  // *AR6%++
      RegText(RegTextInfo{.regnum = Registers::AR6,
                          .indirect = true,
                          .postinc = true,
                          .circular = true},
              result);
      return true;
    } else {  // *+XAR6[AR1%++]
      RegText(
          RegTextInfo{.regnum = Registers::AR6, .indirect = true, .add = true},
          result);
      RegText(RegTextInfo{.regnum = Registers::AR1,
                          .postinc = true,
                          .circular = true,
                          .is_offset = true},
              result);
      return true;
    }
  }
  return false;
}

bool Loc16Text(const LocTextInfo& lti,
               std::vector<BN::InstructionTextToken>& result) {
  // Try generic addressing mode text first. If we don't find a match try
  // 16-bit
  if (loc_text_helper(lti, result)) {
    return true;
  }

  /* 16-bit Register Addressing Modes (AR0 to AR7, AH, AL, PH, PL, TH, SP) */
  // AMODEx, code == 1 0 100 AAA, @ARn
  if ((lti.loc & 0xF8) == 0xA0) {
    const uint8_t regnum = Registers::AR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 000, @AH
  if (lti.loc == 0xA8) {
    RegText(RegTextInfo{.regnum = Registers::AH, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 001, @AL
  if (lti.loc == 0xA9) {
    RegText(RegTextInfo{.regnum = Registers::AL, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 010, @PH
  if (lti.loc == 0xAA) {
    RegText(RegTextInfo{.regnum = Registers::PH, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 011, @PL
  if (lti.loc == 0xAB) {
    RegText(RegTextInfo{.regnum = Registers::PL, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 100, @TH
  if (lti.loc == 0xAC) {
    RegText(RegTextInfo{.regnum = Registers::TH, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 101, @SP
  if (lti.loc == 0xAD) {
    RegText(RegTextInfo{.regnum = Registers::SP, .direct = true}, result);
    return true;
  }

  return false;
}

bool Loc32Text(const LocTextInfo& lti,
               std::vector<BN::InstructionTextToken>& result) {
  // Try generic addressing mode text first. If we don't find a match try 32-bit
  if (loc_text_helper(lti, result)) {
    return true;
  }

  /* 32-bit Register Addressing Modes (XAR0 to XAR7, ACC, P, XT) */
  // AMODEx, code == 1 0 100 AAA, @XARn
  if ((lti.loc & 0xF8) == 0xA0) {
    const uint8_t regnum = Registers::XAR0 + (lti.loc & 0x7);
    RegText(RegTextInfo{.regnum = regnum, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 001, @ACC
  if (lti.loc == 0xA9) {
    RegText(RegTextInfo{.regnum = Registers::ACC, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 011, @P
  if (lti.loc == 0xAB) {
    RegText(RegTextInfo{.regnum = Registers::P, .direct = true}, result);
    return true;
  }

  // AMODEx, code == 1 0 101 100, @XT
  if (lti.loc == 0xAC) {
    RegText(RegTextInfo{.regnum = Registers::XT, .direct = true}, result);
    return true;
  }

  return false;
}

void CondText(const uint8_t cond,
              std::vector<BN::InstructionTextToken>& result) {
  result.emplace_back(TextToken, Conditions::NAMES.at(cond & 0xF));
}

void ModeText(const uint8_t mode,
              std::vector<BN::InstructionTextToken>& result) {
  bool first_flag = true;

  if (mode & 0x1) {
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::SXM));
    first_flag = false;
  }
  if (mode & 0x2) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::OVM));
    first_flag = false;
  }
  if (mode & 0x4) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::TC));
    first_flag = false;
  }
  if (mode & 0x8) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::C));
    first_flag = false;
  }
  if (mode & 0x10) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::INTM));
    first_flag = false;
  }
  if (mode & 0x20) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::DBGM));
    first_flag = false;
  }
  if (mode & 0x40) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::PAGE0));
    first_flag = false;
  }
  if (mode & 0x80) {
    if (!first_flag) {
      OpsepText(result);
    }
    result.emplace_back(TextToken, Flags::NAMES.at(Flags::VMAP));
  }
}

void ProductShiftModeText(const uint8_t mode, const AddressMode amode,
                          std::vector<BN::InstructionTextToken>& result) {
  switch (mode & 0x7u) {
    case 0:
      result.emplace_back(TextToken, "+1");
      break;
    case 1:
      result.emplace_back(TextToken, "0");
      break;
    case 2:
      result.emplace_back(TextToken, "-1");
      break;
    case 3:
      result.emplace_back(TextToken, "-2");
      break;
    case 4:
      result.emplace_back(TextToken, "-3");
      break;
    case 5: {
      if (amode == AMODE_1) {
        result.emplace_back(TextToken, "+4");
        break;
      }
      result.emplace_back(TextToken, "-4");
      break;
    }
    case 6:
      result.emplace_back(TextToken, "-5");
      break;
    case 7:
      result.emplace_back(TextToken, "-6");
      break;
  }
}

void TextOpXarnConst8(const char* op_name, const uint8_t n,
                      const uint8_t const8,
                      std::vector<BN::InstructionTextToken>& result) {
  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);
}

bool TextOpLoc32Xarn(const char* op_name, const uint8_t n, const uint8_t loc32,
                     const AddressMode amode,
                     std::vector<BN::InstructionTextToken>& result) {
  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  return true;
}

bool TextOpXarnLoc32(const char* op_name, const uint8_t n, const uint8_t loc32,
                     const AddressMode amode,
                     std::vector<BN::InstructionTextToken>& result) {
  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  return true;
}

void TextOpXarnConst22(const char* op_name, const uint8_t n,
                       const uint32_t const22,
                       std::vector<BN::InstructionTextToken>& result) {
  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const22, .nbits = 22}, result);
}

bool TextOpArnLoc16(const char* op_name, const uint8_t n, const uint8_t loc16,
                    const AddressMode amode,
                    std::vector<BN::InstructionTextToken>& result) {
  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + n)},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  return true;
}

/* Instruction Text Implementations */

bool Aborti::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool AbsAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool AbstcAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool AddAccConst16Shift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool AddAccLoc16ShiftT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool AddAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddAccLoc16Shift1_15::Text(const uint8_t* data, uint64_t addr, size_t& len,
                                std::vector<BN::InstructionTextToken>& result,
                                const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool AddAccLoc16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 5}, result);

  return true;
}

bool AddAccLoc16Shift0_15::Text(const uint8_t* data, uint64_t addr, size_t& len,
                                std::vector<BN::InstructionTextToken>& result,
                                const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool AddAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool AddLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_signed = true},
            result);

  return true;
}

bool AddbAccConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool AddbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool AddbSpConst7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const7 = GetConst7(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::SP}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const7, .nbits = 7}, result);

  return true;
}

bool AddbXarnConst7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const7 = GetConst7(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const7, .nbits = 7}, result);

  return true;
}

bool AddclAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddcuAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddlAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddlAccPShiftPm::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  LshiftText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::PM));

  return true;
}

bool AddlLoc32Acc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool AdduAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddulPLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AddulAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AdrkImm8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto imm8 = GetImm8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = imm8, .nbits = 8}, result);

  return true;
}

bool AndAccConst16Shift0_15::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool AndAccConst16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                                std::vector<BN::InstructionTextToken>& result,
                                const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 5}, result);

  return true;
}

bool AndAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AndAxLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool AndIerConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IER}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool AndIfrConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IFR}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool AndLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool AndAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool AndLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool AndbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool Asp::Text(const uint8_t* data, uint64_t addr, size_t& len,
               std::vector<BN::InstructionTextToken>& result,
               const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool AsrAxShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool AsrAxT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool Asr64AccPShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool Asr64AccPT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool AsrlAccT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool BOff16Cond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off16 = GetOff16(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off16, .nbits = 16}, result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool BanzOff16Arn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off16 = GetOff16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off16, .nbits = 16}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + n),
                      .postdec = true},
          result);

  return true;
}

bool BarOff16ArnArmEq::Text(const uint8_t* data, uint64_t addr, size_t& len,
                            std::vector<BN::InstructionTextToken>& result,
                            const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off16 = GetOff16(dataOp);
  const auto n = GetRegN(dataOp);
  const auto m = GetRegM(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off16, .nbits = 16}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + n)},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + m)},
          result);
  OpsepText(result);
  CondText(Conditions::EQ, result);

  return true;
}

bool BarOff16ArnArmNeq::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off16 = GetOff16(dataOp);
  const auto n = GetRegN(dataOp);
  const auto m = GetRegM(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off16, .nbits = 16}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + n)},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + m)},
          result);
  OpsepText(result);
  CondText(Conditions::NEQ, result);

  return true;
}

bool BfOff16Cond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off16 = GetOff16(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off16, .nbits = 16}, result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool ClrcAmode::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ClrcM0M1Map::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ClrcObjmode::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ClrcOvc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ClrcXf::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ClrcMode::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto mode = GetMode(dataOp);

  if (mode == 0) {
    return false;  // not valid for mode == 0
  }

  OpText(op_name, result);
  SpaceText(result);
  ModeText(mode, result);

  return true;
}

bool CmpAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool CmpLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_signed = true},
            result);

  return true;
}

bool Cmp64AccP::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool CmpbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool CmplAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool CmplAccPShiftPm::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  LshiftText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::PM));

  return true;
}

bool Cmpr0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Cmpr1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Cmpr2::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Cmpr3::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool CsbAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool DecLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool DmacAccPLoc32Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool DmacAccPLoc32Xar7Postinc::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(
      RegTextInfo{.regnum = Registers::XAR7, .indirect = true, .postinc = true},
      result);

  return true;
}

bool DmovLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool Eallow::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Edis::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Estop0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Estop1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool FfcXar7Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const22, .nbits = 22, .is_address = true},
            result);

  return true;
}

bool FlipAx::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool IackConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool Idle::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool ImaclPLoc32Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool ImaclPLoc32Xar7Postinc::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(
      RegTextInfo{.regnum = Registers::XAR7, .indirect = true, .postinc = true},
      result);

  return true;
}

bool ImpyalPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool ImpylAccXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool ImpylPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool ImpyslPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool ImpyxulPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool InLoc16Pa::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_memio = true},
            result);

  return true;
}

bool IncLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool IntrIntx::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetIntrX(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);

  // x == 0 --> RESET vector
  if (x == 0) {
    result.emplace_back(TextToken, "reset");
  } else {
    result.emplace_back(TextToken, std::format("int{}", x));
  }

  return true;
}

bool IntrNmi::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool IntrEmuint::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Iret::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool LbXar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool LbConst22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const22, .nbits = 22, .is_address = true},
            result);

  return true;
}

bool LcXar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool LcConst22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const22, .nbits = 22, .is_address = true},
            result);

  return true;
}

bool LcrConst22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const22, .nbits = 22, .is_address = true},
            result);

  return true;
}

bool LcrXarn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n),
                      .indirect = true},
          result);

  return true;
}

bool LoopnzLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool LoopzLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool Lpaddr::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Lret::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Lrete::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool Lretr::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool LslAccShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool LslAccT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool LslAxShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool LslAxT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool Lsl64AccPShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool Lsl64AccPT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool LsllAccT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool LsrAxShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool LsrAxT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool Lsr64AccPShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool Lsr64AccPT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool LsrlAccT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool MacPLoc16Pma::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  result.emplace_back(IntegerToken, "0", 0);
  RegCombineText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);

  return true;
}

bool MacPLoc16Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool MacPLoc16Xar7Postinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                                std::vector<BN::InstructionTextToken>& result,
                                const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(
      RegTextInfo{.regnum = Registers::XAR7, .indirect = true, .postinc = true},
      result);

  return true;
}

bool MaxAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MaxculPLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MaxlAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MinAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MinculPLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MinlAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovMem16Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_memio = true},
            result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovAccConst16Shift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool MovAccLoc16ShiftT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool MovAccLoc16Objmode1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovAccLoc16Objmode0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovAccLoc16Shift1_15Objmode1::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool MovAccLoc16Shift1_15Objmode0::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool MovAccLoc16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 4}, result);

  return true;
}

bool MovAr6Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR6}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovAr7Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR7}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovDpConst10::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const10 = GetConst10(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const10, .nbits = 10}, result);

  return true;
}

bool MovIerLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IER}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool MovLoc16Mem16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_memio = true},
            result);

  return true;
}

bool MovLoc16_0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = 0, .nbits = 16}, result);

  return true;
}

bool MovLoc16AccShift1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 1, .nbits = 4}, result);

  return true;
}

bool MovLoc16AccShift2_8Objmode1::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 3}, result);

  return true;
}

bool MovLoc16AccShift2_8Objmode0::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 3}, result);

  return true;
}

bool MovLoc16Arn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::AR0 + n)},
          result);

  return true;
}

bool MovLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool MovLoc16AxCond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  CondText(cond, result);

  return true;
}

bool MovLoc16Ier::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::IER}, result);

  return true;
}

bool MovLoc16Ovc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::OVC));

  return true;
}

bool MovLoc16P::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool MovLoc16T::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool MovOvcLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::OVC));
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovPhLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::PH}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovPlLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::PL}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovPmAx::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::PM));
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool MovTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovTl0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::TL}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = 0, .nbits = 1}, result);

  return true;
}

bool MovXarnPc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::PC}, result);

  return true;
}

bool MovaTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovadTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovbAccConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MovbAr6Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR6}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MovbAr7Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR7}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MovbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MovbAxlsbLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  result.emplace_back(TextToken, ".lsb");
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovbAxmsbLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  result.emplace_back(TextToken, ".msb");
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovbLoc16Const8Cond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const8 = GetConst8(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool MovbLoc16Axlsb::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  result.emplace_back(TextToken, ".lsb");

  return true;
}

bool MovbLoc16Axmsb::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  result.emplace_back(TextToken, ".msb");

  return true;
}

bool MovbXar0Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 0, const8, result);

  return true;
}

bool MovbXar1Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 1, const8, result);

  return true;
}

bool MovbXar2Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 2, const8, result);

  return true;
}

bool MovbXar3Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 3, const8, result);

  return true;
}

bool MovbXar4Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 4, const8, result);

  return true;
}

bool MovbXar5Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 5, const8, result);

  return true;
}

bool MovbXar6Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 6, const8, result);

  return true;
}

bool MovbXar7Const8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  TextOpXarnConst8(op_name, 7, const8, result);

  return true;
}

bool MovdlXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovhLoc16AccShift1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 1, .nbits = 4}, result);

  return true;
}

bool MovhLoc16AccShift2_8Objmode1::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 3}, result);

  return true;
}

bool MovhLoc16AccShift2_8Objmode0::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 3}, result);

  return true;
}

bool MovhLoc16P::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool MovlAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovlAccPShiftPm::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  LshiftText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::PM));

  return true;
}

bool MovlLoc32Acc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool MovlLoc32AccCond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                            std::vector<BN::InstructionTextToken>& result,
                            const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool MovlLoc32P::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool MovlLoc32Xar0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 0, loc32, amode, result);
}

bool MovlLoc32Xar1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 1, loc32, amode, result);
}

bool MovlLoc32Xar2::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 2, loc32, amode, result);
}

bool MovlLoc32Xar3::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 3, loc32, amode, result);
}

bool MovlLoc32Xar4::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 4, loc32, amode, result);
}

bool MovlLoc32Xar5::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 5, loc32, amode, result);
}

bool MovlLoc32Xar6::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 6, loc32, amode, result);
}

bool MovlLoc32Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpLoc32Xarn(op_name, 7, loc32, amode, result);
}

bool MovlLoc32Xt::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);

  return true;
}

bool MovlPAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool MovlPLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovlXar0Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 0, loc32, amode, result);
}

bool MovlXar1Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 1, loc32, amode, result);
}

bool MovlXar2Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 2, loc32, amode, result);
}

bool MovlXar3Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 3, loc32, amode, result);
}

bool MovlXar4Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 4, loc32, amode, result);
}

bool MovlXar5Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 5, loc32, amode, result);
}

bool MovlXar6Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 6, loc32, amode, result);
}

bool MovlXar7Loc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  return TextOpXarnLoc32(op_name, 7, loc32, amode, result);
}

bool MovlXar0Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 0, const22, result);

  return true;
}

bool MovlXar1Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 1, const22, result);

  return true;
}

bool MovlXar2Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 2, const22, result);

  return true;
}

bool MovlXar3Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 3, const22, result);

  return true;
}

bool MovlXar4Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 4, const22, result);

  return true;
}

bool MovlXar5Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 5, const22, result);

  return true;
}

bool MovlXar6Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 6, const22, result);

  return true;
}

bool MovlXar7Const22::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const22 = GetConst22(dataOp);
  len = GetLength();

  TextOpXarnConst22(op_name, 7, const22, result);

  return true;
}

bool MovlXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovpTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovsTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovuAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovuLoc16Ovc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::OVC));

  return true;
}

bool MovuOvcLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  result.emplace_back(TextToken, Flags::NAMES.at(Flags::OVC));
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovwDpConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool MovxTlLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::TL}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MovzAr0Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 0, loc16, amode, result);
}

bool MovzAr1Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 1, loc16, amode, result);
}

bool MovzAr2Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 2, loc16, amode, result);
}

bool MovzAr3Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 3, loc16, amode, result);
}

bool MovzAr4Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 4, loc16, amode, result);
}

bool MovzAr5Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 5, loc16, amode, result);
}

bool MovzAr6Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 6, loc16, amode, result);
}

bool MovzAr7Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  return TextOpArnLoc16(op_name, 7, loc16, amode, result);
}

bool MovzDpConst10::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const10 = GetConst10(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const10, .nbits = 10}, result);

  return true;
}

bool MpyAccLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool MpyAccTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyPLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                            std::vector<BN::InstructionTextToken>& result,
                            const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool MpyPTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyaPLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool MpyaPTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpybAccTConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MpybPTConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool MpysPTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyuPTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyuAccTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyxuAccTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool MpyxuPTLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool Nasp::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

bool NegAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool NegAx::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool Neg64AccP::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool NegtcAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool NopIndArpn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetInd(dataOp);  // ind is a subset of loc16/32 mode
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool NormAccInd::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");

  return true;
}

bool NormAccIndPostinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "++");

  return true;
}

bool NormAccIndPostdec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "--");

  return true;
}

bool NormAccInd0Postinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "++");

  return true;
}

bool NormAccInd0Postdec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "--");
  return true;
}

bool NormAccXarnPostinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n),
                      .postinc = true},
          result);

  return true;
}

bool NormAccXarnPostdec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n),
                      .postdec = true},
          result);

  return true;
}

bool NotAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool NotAx::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool OrAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool OrAccConst16Shift0_15::Text(const uint8_t* data, uint64_t addr,
                                 size_t& len,
                                 std::vector<BN::InstructionTextToken>& result,
                                 const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool OrAccConst16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 4}, result);

  return true;
}

bool OrAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  return true;
}

bool OrIerConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IER}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool OrIfrConst16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IFR}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool OrLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);

  return true;
}

bool OrLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool OrbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool OutPaLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_memio = true},
            result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool PopAr1Ar0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR1}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR0}, result);

  return true;
}

bool PopAr3Ar2::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR3}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR2}, result);

  return true;
}

bool PopAr5Ar4::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR5}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR4}, result);

  return true;
}

bool PopAr1hAr0h::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR1H}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR0H}, result);

  return true;
}

bool PopDbgier::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DBGIER}, result);

  return true;
}

bool PopDp::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);

  return true;
}

bool PopDpSt1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::ST1}, result);

  return true;
}

bool PopIfr::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IFR}, result);

  return true;
}

bool PopLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool PopP::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool PopRpc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::RPC}, result);

  return true;
}

bool PopSt0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ST0}, result);

  return true;
}

bool PopSt1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ST1}, result);

  return true;
}

bool PopTSt0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::ST0}, result);

  return true;
}

bool PopXt::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);

  return true;
}

bool PreadLoc16Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool PushAr1Ar0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR1}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR0}, result);

  return true;
}

bool PushAr3Ar2::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR3}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR2}, result);

  return true;
}

bool PushAr5Ar4::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR5}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR4}, result);

  return true;
}

bool PushAr1hAr0h::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AR1H}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::AR0H}, result);

  return true;
}

bool PushDbgier::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DBGIER}, result);

  return true;
}

bool PushDp::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);

  return true;
}

bool PushDpSt1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::DP}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::ST1}, result);

  return true;
}

bool PushIfr::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::IFR}, result);

  return true;
}

bool PushLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool PushP::Text(const uint8_t* data, uint64_t addr, size_t& len,
                 std::vector<BN::InstructionTextToken>& result,
                 const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool PushRpc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::RPC}, result);

  return true;
}

bool PushSt0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ST0}, result);

  return true;
}

bool PushSt1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ST1}, result);

  return true;
}

bool PushTSt0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::ST0}, result);

  return true;
}

bool PushXt::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);

  return true;
}

bool PwriteXar7Loc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmaclPLoc32Xar7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XAR7, .indirect = true}, result);

  return true;
}

bool QmaclPLoc32Xar7Postinc::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(
      RegTextInfo{.regnum = Registers::XAR7, .indirect = true, .postinc = true},
      result);

  return true;
}

bool QmpyalPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmpylPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmpylAccXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmpyslPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmpyulPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool QmpyxulPXtLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::XT}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool RolAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool RorAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool RptConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool RptLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SatAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool Sat64AccP::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  RegCombineText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);

  return true;
}

bool SbOff8Cond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off8 = GetOff8(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off8, .nbits = 8, .is_offset = true},
            result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool SbbuAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SbfOff8Eq::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off8 = GetOff8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off8, .nbits = 8, .is_offset = true},
            result);
  OpsepText(result);
  CondText(Conditions::EQ, result);

  return true;
}

bool SbfOff8Neq::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off8 = GetOff8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off8, .nbits = 8, .is_offset = true},
            result);
  OpsepText(result);
  CondText(Conditions::NEQ, result);

  return true;
}

bool SbfOff8Tc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off8 = GetOff8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off8, .nbits = 8, .is_offset = true},
            result);
  OpsepText(result);
  CondText(Conditions::TC, result);

  return true;
}

bool SbfOff8Ntc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto off8 = GetOff8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = off8, .nbits = 8, .is_offset = true},
            result);
  OpsepText(result);
  CondText(Conditions::NTC, result);

  return true;
}

bool SbrkConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool SetcMode::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto mode = GetMode(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ModeText(mode, result);

  return true;
}

bool SetcM0M1Map::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ModeText(Flags::M0M1MAP, result);

  return true;
}

bool SetcObjmode::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ModeText(Flags::OBJMODE, result);

  return true;
}

bool SetcXf::Text(const uint8_t* data, uint64_t addr, size_t& len,
                  std::vector<BN::InstructionTextToken>& result,
                  const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ModeText(Flags::XF, result);

  return true;
}

bool SfrAccShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool SfrAccT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool SpmShift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto pm = GetPM(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ProductShiftModeText(pm, amode, result);

  return true;
}

bool SqraLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SqrsLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubAccLoc16Objmode1::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubAccLoc16Objmode0::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubAccLoc16Shift1_15Objmode1::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool SubAccLoc16Shift1_15Objmode0::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool SubAccLoc16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 5}, result);

  return true;
}

bool SubAccLoc16ShiftT::Text(const uint8_t* data, uint64_t addr, size_t& len,
                             std::vector<BN::InstructionTextToken>& result,
                             const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  LshiftText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool SubAccConst16Shift::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool SubAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool SubbAccConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool SubbSpConst7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const7 = GetConst7(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::SP}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const7, .nbits = 7}, result);

  return true;
}

bool SubbXarnConst7::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  const auto const7 = GetConst7(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = static_cast<uint8_t>(Registers::XAR0 + n)},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const7, .nbits = 7}, result);

  return true;
}

bool SubblAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubcuAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubculAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SublAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SublLoc32Acc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool SubrLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool SubrlLoc32Acc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool SubuAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubulAccLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool SubulPLoc32::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc32 = GetLoc32(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc32Text(LocTextInfo{.loc = loc32, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool TbitLoc16Bit::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto bit = GetBitIndex(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = bit, .nbits = 4}, result);

  return true;
}

bool TbitLoc16T::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::T}, result);

  return true;
}

bool TclrLoc16Bit::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto bit = GetBitIndex(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = bit, .nbits = 4}, result);

  return true;
}

bool TestAcc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);

  return true;
}

bool TrapVec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto vec = GetVec(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = vec, .nbits = 5}, result);

  return true;
}

bool TsetLoc16Bit::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto bit = GetBitIndex(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = bit, .nbits = 4}, result);

  return true;
}

bool UoutPaLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_memio = true},
            result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool XbAl::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AL, .indirect = true}, result);

  return true;
}

bool XbPmaArpn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto n = GetRegN(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XbPmaCond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool XbanzPmaInd::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");

  return true;
}

bool XbanzPmaIndPostinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "++");

  return true;
}

bool XbanzPmaIndPostdec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                              std::vector<BN::InstructionTextToken>& result,
                              const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "--");

  return true;
}

bool XbanzPmaInd0Postinc::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "++");

  return true;
}

bool XbanzPmaInd0Postdec::Text(const uint8_t* data, uint64_t addr, size_t& len,
                               std::vector<BN::InstructionTextToken>& result,
                               const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "--");

  return true;
}

bool XbanzPmaIndArpn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XbanzPmaIndPostincArpn::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "++");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XbanzPmaIndPostdecArpn::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(OperationToken, "--");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XbanzPmaInd0PostincArpn::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "++");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XbanzPmaInd0PostdecArpn::Text(
    const uint8_t* data, uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result, const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  result.emplace_back(TextToken, "0");
  result.emplace_back(OperationToken, "--");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XcallAl::Text(const uint8_t* data, uint64_t addr, size_t& len,
                   std::vector<BN::InstructionTextToken>& result,
                   const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AL, .indirect = true}, result);

  return true;
}

bool XcallPmaArpn::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto n = GetRegN(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  result.emplace_back(OperationToken, "*");
  OpsepText(result);
  result.emplace_back(TextToken, std::format("arp{}", n));

  return true;
}

bool XcallPmaCond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_address = true},
            result);
  OpsepText(result);
  CondText(cond, result);

  return true;
}

bool XmacPLoc16Pma::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(
      ConstTextInfo{
          .value = const16, .nbits = 16, .is_address = true, .is_memio = true},
      result);

  return true;
}

bool XmacdPLoc16Pma::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::P}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(
      ConstTextInfo{
          .value = const16, .nbits = 16, .is_address = true, .is_memio = true},
      result);

  return true;
}

bool XorAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                       std::vector<BN::InstructionTextToken>& result,
                       const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool XorAccConst16Shift0_15::Text(const uint8_t* data, uint64_t addr,
                                  size_t& len,
                                  std::vector<BN::InstructionTextToken>& result,
                                  const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  const auto shift = GetShift(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = shift, .nbits = 4}, result);

  return true;
}

bool XorAccConst16Shift16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                                std::vector<BN::InstructionTextToken>& result,
                                const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16}, result);
  LshiftText(result);
  ConstText(ConstTextInfo{.value = 16, .nbits = 5}, result);

  return true;
}

bool XorAxLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool XorLoc16Ax::Text(const uint8_t* data, uint64_t addr, size_t& len,
                      std::vector<BN::InstructionTextToken>& result,
                      const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto x = GetRegAx(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);

  return true;
}

bool XorLoc16Const16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                           std::vector<BN::InstructionTextToken>& result,
                           const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const16, .nbits = 16, .is_signed = true},
            result);

  return true;
}

bool XorbAxConst8::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto x = GetRegAx(dataOp);
  const auto const8 = GetConst8(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = x == 1 ? Registers::AH : Registers::AL},
          result);
  OpsepText(result);
  ConstText(ConstTextInfo{.value = const8, .nbits = 8}, result);

  return true;
}

bool XpreadLoc16Pma::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  const auto const16 = GetConst16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  ConstText(
      ConstTextInfo{
          .value = const16, .nbits = 16, .is_address = true, .is_memio = true},
      result);

  return true;
}

bool XpreadLoc16Al::Text(const uint8_t* data, uint64_t addr, size_t& len,
                         std::vector<BN::InstructionTextToken>& result,
                         const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }
  OpsepText(result);
  RegText(RegTextInfo{.regnum = Registers::AL, .indirect = true}, result);

  return true;
}

bool XpwriteAlLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                          std::vector<BN::InstructionTextToken>& result,
                          const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::AL, .indirect = true}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool XretcCond::Text(const uint8_t* data, uint64_t addr, size_t& len,
                     std::vector<BN::InstructionTextToken>& result,
                     const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto cond = GetCond(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  CondText(cond, result);

  return true;
}

bool ZalrAccLoc16::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        const AddressMode amode) {
  const auto dataOp = DataToOpcode(data, GetLength());
  const auto loc16 = GetLoc16(dataOp);
  len = GetLength();

  OpText(op_name, result);
  SpaceText(result);
  RegText(RegTextInfo{.regnum = Registers::ACC}, result);
  OpsepText(result);
  if (!Loc16Text(LocTextInfo{.loc = loc16, .amode = amode}, result)) {
    return false;
  }

  return true;
}

bool Zapa::Text(const uint8_t* data, uint64_t addr, size_t& len,
                std::vector<BN::InstructionTextToken>& result,
                const AddressMode amode) {
  len = GetLength();

  OpText(op_name, result);

  return true;
}

}  // namespace TIC28X
