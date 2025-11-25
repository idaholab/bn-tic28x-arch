// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "architecture.h"

#include <binaryninjaapi.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "flags.h"
#include "instructions.h"
#include "registers.h"
#include "sizes.h"
#include "text.h"
#include "util.h"

namespace BN = BinaryNinja;

namespace TIC28X {
TIC28XArchitecture::TIC28XArchitecture(const std::string& name)
    : Architecture(name) {
  // Set default modes
  OBJMODE = OBJMODE_0;
  AMODE = AMODE_0;
  INSTRUCTIONS = GenerateInstructionVector();
}

TIC28XArchitecture::~TIC28XArchitecture() { INSTRUCTIONS.clear(); }

void TIC28XArchitecture::SetObjmode(const ObjectMode mode) { OBJMODE = mode; }

void TIC28XArchitecture::SetAmode(const AddressMode mode) { AMODE = mode; }

// Safely insert an address into the repeat address set
void TIC28XArchitecture::AddRepeatAddr(const uint64_t addr) {
  std::lock_guard lock(REPEAT_ADDR_MUTEX);
  REPEAT_ADDRS.insert(addr);
}

// Safely check to see if an address is repeated
bool TIC28XArchitecture::IsRepeatAddr(const uint64_t addr) {
  std::lock_guard lock(REPEAT_ADDR_MUTEX);
  return REPEAT_ADDRS.contains(addr);
}

BNRegisterInfo TIC28XArchitecture::RegisterInfo(const uint32_t fullWidthReg,
                                                const size_t offset,
                                                const size_t size,
                                                const bool zeroExtend) {
  BNRegisterInfo result;

  result.fullWidthRegister = fullWidthReg;
  result.offset = offset;
  result.size = size;
  result.extend = zeroExtend ? ZeroExtendToFullWidth : NoExtend;

  return result;
}

[[nodiscard]] size_t TIC28XArchitecture::GetAddressSize() const {
  return Sizes::_4_BYTES;  // 32-bit
}

[[nodiscard]] size_t TIC28XArchitecture::GetDefaultIntegerSize() const {
  return Sizes::_2_BYTES;
}

[[nodiscard]] BNEndianness TIC28XArchitecture::GetEndianness() const {
  return LittleEndian;
}

[[nodiscard]] uint32_t TIC28XArchitecture::GetStackPointerRegister() {
  return Registers::SP;
}

[[nodiscard]] size_t TIC28XArchitecture::GetMaxInstructionLength() const {
  return Sizes::_4_BYTES;
}

[[nodiscard]] size_t TIC28XArchitecture::GetInstructionAlignment() const {
  return Sizes::_2_BYTES;
}

[[nodiscard]] std::vector<uint32_t> TIC28XArchitecture::GetAllRegisters() {
  return std::vector<uint32_t>{
      Registers::ACC,  Registers::AH,   Registers::AL,     Registers::XAR0,
      Registers::XAR1, Registers::XAR2, Registers::XAR3,   Registers::XAR4,
      Registers::XAR5, Registers::XAR6, Registers::XAR7,   Registers::AR0,
      Registers::AR1,  Registers::AR2,  Registers::AR3,    Registers::AR4,
      Registers::AR5,  Registers::AR6,  Registers::AR7,    Registers::AR0H,
      Registers::AR1H, Registers::AR2H, Registers::AR3H,   Registers::AR4H,
      Registers::AR5H, Registers::AR6H, Registers::AR7H,   Registers::BR0,
      Registers::BR1,  Registers::BR2,  Registers::BR3,    Registers::BR4,
      Registers::BR5,  Registers::BR6,  Registers::BR7,    Registers::DP,
      Registers::IFR,  Registers::IER,  Registers::DBGIER, Registers::P,
      Registers::PH,   Registers::PL,   Registers::PC,     Registers::RPC,
      Registers::SP,   Registers::ST0,  Registers::ST1,    Registers::XT,
      Registers::T,    Registers::TL,   Registers::TH,
  };
}

[[nodiscard]] BNRegisterInfo TIC28XArchitecture::GetRegisterInfo(
    const uint32_t reg) {
  if (Registers::INFO.contains(reg)) {
    const auto [fullWidthReg, offset, size, zextend] = Registers::INFO.at(reg);
    return RegisterInfo(fullWidthReg, offset, size, zextend);
  }
  return RegisterInfo(0, 0, 0);
}

[[nodiscard]] std::string TIC28XArchitecture::GetRegisterName(
    const uint32_t reg) {
  if (Registers::NAMES.contains(reg)) {
    return Registers::NAMES.at(reg);
  }
  return "INVALID_REGISTER_ID";
}

[[nodiscard]] std::vector<uint32_t> TIC28XArchitecture::GetAllFlags() {
  return std::vector<uint32_t>{
      Flags::SXM,      Flags::OVM,   Flags::TC,      Flags::C,
      Flags::Z,        Flags::N,     Flags::V,       Flags::PM,
      Flags::OVC,      Flags::INTM,  Flags::DBGM,    Flags::PAGE0,
      Flags::VMAP,     Flags::SPA,   Flags::LOOP,    Flags::EALLOW,
      Flags::IDLESTAT, Flags::AMODE, Flags::OBJMODE, Flags::M0M1MAP,
      Flags::XF,       Flags::ARP};
}

[[nodiscard]] std::string TIC28XArchitecture::GetFlagName(uint32_t flag) {
  if (Flags::NAMES.contains(flag)) {
    return Flags::NAMES.at(flag);
  }
  return "INVALID_FLAG";
}

[[nodiscard]] BNFlagRole TIC28XArchitecture::GetFlagRole(uint32_t flag,
                                                         uint32_t semClass) {
  // TODO
  return SpecialFlagRole;
}

[[nodiscard]] std::vector<uint32_t>
TIC28XArchitecture::GetFlagsWrittenByFlagWriteType(uint32_t flags) {
  // TODO
  return std::vector<uint32_t>{};
}

[[nodiscard]] std::string TIC28XArchitecture::GetFlagWriteTypeName(
    uint32_t flags) {
  // TODO
  return "";
}

[[nodiscard]] std::vector<uint32_t>
TIC28XArchitecture::GetFlagsRequiredForFlagCondition(
    BNLowLevelILFlagCondition cond, uint32_t semClass) {
  // TODO
  return std::vector<uint32_t>{};
}

/**
 * GetInstructionInfo - parses opcodes; determines instruction length and
 * branching behavior
 * @param data   - pointer to the next byte in the binary that is being analyzed
 * @param addr   - current address value held by the program counter
 * @param maxLen - idk tbh
 * @param result - InstructionInfo object to be populated with instruction
 * length and branch behavior
 * @return bool indicating whether a valid instruction was parsed
 */
bool TIC28XArchitecture::GetInstructionInfo(const uint8_t* data,
                                            const uint64_t addr,
                                            const size_t maxLen,
                                            BN::InstructionInfo& result) {
  if (const auto i = DecodeInstruction(data, INSTRUCTIONS, OBJMODE)) {
    return i->get()->Info(data, addr, result, this);
  }
  return false;
}

bool TIC28XArchitecture::GetInstructionText(
    const uint8_t* data, const uint64_t addr, size_t& len,
    std::vector<BN::InstructionTextToken>& result) {
  if (const auto i = DecodeInstruction(data, INSTRUCTIONS, OBJMODE)) {
    // Check to see if we need to generate repeat text (||) for this instruction
    if (i->get()->IsRepeatable() && IsRepeatAddr(addr)) {
      result.emplace_back(TextToken, "|| ");
    }
    return i->get()->Text(data, addr, len, result, AMODE);
  }
  return false;
}

bool TIC28XArchitecture::GetInstructionLowLevelIL(const uint8_t* data,
                                                  const uint64_t addr,
                                                  size_t& len,
                                                  BN::LowLevelILFunction& il) {
  if (const auto i = DecodeInstruction(data, INSTRUCTIONS, OBJMODE)) {
    return i->get()->Lift(data, addr, len, il, this);
  }
  return false;
}

}  // namespace TIC28X
extern "C" {
BN_DECLARE_CORE_ABI_VERSION
BINARYNINJAPLUGIN bool CorePluginInit() {
  const auto tic28x_c27x = new TIC28X::TIC28XArchitecture("tic28x_c27x");
  const auto tic28x_c2xlp = new TIC28X::TIC28XArchitecture("tic28x_c2xlp");
  const auto tic28x = new TIC28X::TIC28XArchitecture("tic28x");

  BN::Architecture::Register(tic28x_c27x);
  BN::Architecture::Register(tic28x_c2xlp);
  BN::Architecture::Register(tic28x);

  // Set compatibility modes for each arch
  tic28x_c27x->SetObjmode(TIC28X::OBJMODE_0);
  tic28x_c27x->SetAmode(TIC28X::AMODE_0);

  tic28x_c2xlp->SetObjmode(TIC28X::OBJMODE_1);
  tic28x_c2xlp->SetAmode(TIC28X::AMODE_1);

  tic28x->SetObjmode(TIC28X::OBJMODE_1);
  tic28x->SetAmode(TIC28X::AMODE_0);

  // TODO: calling conventions
  //    Ref<CallingConvention> cc = new TIC28XCallingConvention(tic28x);
  //    tic28x->RegisterCallingConvention(cc);
  //    tic28x->SetDefaultCallingConvention(cc);
  return true;
}
}