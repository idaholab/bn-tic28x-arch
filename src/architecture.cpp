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
  return std::vector<uint32_t>{Flags::SXM,
                               Flags::OVM,
                               Flags::TC,
                               Flags::C,
                               Flags::Z,
                               Flags::N,
                               Flags::V,
                               Flags::PM,
                               Flags::OVC,
                               Flags::INTM,
                               Flags::DBGM,
                               Flags::PAGE0,
                               Flags::VMAP,
                               Flags::SPA,
                               Flags::LOOP,
                               Flags::EALLOW,
                               Flags::IDLESTAT,
                               Flags::AMODE,
                               Flags::OBJMODE,
                               Flags::M0M1MAP,
                               Flags::XF,
                               Flags::ARP,
                               Flags::VSTATUS_OVFR,
                               Flags::VSTATUS_OVRI,
                               Flags::VSTATUS_SAT,
                               Flags::VSTATUS_RND};
}

[[nodiscard]] std::string TIC28XArchitecture::GetFlagName(uint32_t flag) {
  if (Flags::NAMES.contains(flag)) {
    return Flags::NAMES.at(flag);
  }
  return "INVALID_FLAG";
}

[[nodiscard]] BNFlagRole TIC28XArchitecture::GetFlagRole(uint32_t flag,
                                                         uint32_t semClass) {
  switch (flag) {
    // ST0 flags
    case Flags::Z:
      return ZeroFlagRole;
    case Flags::N:
      return NegativeSignFlagRole;
    case Flags::C:
      return CarryFlagRole;
    case Flags::V:
      return OverflowFlagRole;
    case Flags::OVM:
      return SpecialFlagRole;  // Overflow mode - affects saturation behavior
    case Flags::SXM:
      return SpecialFlagRole;  // Sign extension mode
    case Flags::TC:
      return SpecialFlagRole;  // Test/control flag
    case Flags::PM:
      return SpecialFlagRole;  // Product shift mode
    case Flags::OVC:
      return SpecialFlagRole;  // Overflow counter

    // ST1 flags - all special purpose
    case Flags::INTM:
    case Flags::DBGM:
    case Flags::PAGE0:
    case Flags::VMAP:
    case Flags::SPA:
    case Flags::LOOP:
    case Flags::EALLOW:
    case Flags::IDLESTAT:
    case Flags::AMODE:
    case Flags::OBJMODE:
    case Flags::M0M1MAP:
    case Flags::XF:
    case Flags::ARP:
      return SpecialFlagRole;

    // VSTATUS flags (VCU)
    case Flags::VSTATUS_OVFR:
      return OverflowFlagRole;  // VCU overflow flag (real part)
    case Flags::VSTATUS_OVRI:
      return OverflowFlagRole;  // VCU overflow flag (imaginary part)
    case Flags::VSTATUS_SAT:
      return SpecialFlagRole;  // VCU saturation mode enable
    case Flags::VSTATUS_RND:
      return SpecialFlagRole;  // VCU rounding mode

    default:
      return SpecialFlagRole;
  }
}

[[nodiscard]] std::vector<uint32_t>
TIC28XArchitecture::GetFlagsWrittenByFlagWriteType(uint32_t writeType) {
  // Flag write types for C28x:
  // 0 = None
  // 1 = NZVC (standard arithmetic flags)
  // 2 = NZC (logical operations)
  // 3 = NZ (compare, test)
  // 4 = C only (shift operations)
  // 5 = V only (overflow check)
  // 6 = VSTATUS_OVFR (VCU overflow - real)
  // 7 = VSTATUS_OVRI (VCU overflow - imaginary)
  // 8 = VSTATUS_OVFR + VSTATUS_OVRI (VCU complex overflow)
  switch (writeType) {
    case 1:  // NZVC - arithmetic operations
      return {Flags::N, Flags::Z, Flags::V, Flags::C};
    case 2:  // NZC - logical operations
      return {Flags::N, Flags::Z, Flags::C};
    case 3:  // NZ - compare/test
      return {Flags::N, Flags::Z};
    case 4:  // C - shift operations
      return {Flags::C};
    case 5:  // V - overflow check
      return {Flags::V};
    case 6:  // VSTATUS_OVFR - VCU overflow (real)
      return {Flags::VSTATUS_OVFR};
    case 7:  // VSTATUS_OVRI - VCU overflow (imaginary)
      return {Flags::VSTATUS_OVRI};
    case 8:  // VCU complex overflow (both real and imaginary)
      return {Flags::VSTATUS_OVFR, Flags::VSTATUS_OVRI};
    default:
      return {};
  }
}

[[nodiscard]] std::string TIC28XArchitecture::GetFlagWriteTypeName(
    uint32_t writeType) {
  switch (writeType) {
    case 1:
      return "nzvc";
    case 2:
      return "nzc";
    case 3:
      return "nz";
    case 4:
      return "c";
    case 5:
      return "v";
    case 6:
      return "vstatus_ovfr";
    case 7:
      return "vstatus_ovri";
    case 8:
      return "vstatus_ovfr_ovri";
    default:
      return "";
  }
}

[[nodiscard]] std::vector<uint32_t>
TIC28XArchitecture::GetFlagsRequiredForFlagCondition(
    BNLowLevelILFlagCondition cond, uint32_t semClass) {
  // Return flags needed to evaluate each condition
  switch (cond) {
    case LLFC_E:   // Equal (Z == 1)
    case LLFC_NE:  // Not equal (Z == 0)
      return {Flags::Z};

    case LLFC_SLT:  // Signed less than (N != V)
    case LLFC_SGE:  // Signed greater or equal (N == V)
      return {Flags::N, Flags::V};

    case LLFC_SLE:  // Signed less or equal (Z == 1 || N != V)
    case LLFC_SGT:  // Signed greater than (Z == 0 && N == V)
      return {Flags::Z, Flags::N, Flags::V};

    case LLFC_ULT:  // Unsigned less than (C == 0)
    case LLFC_UGE:  // Unsigned greater or equal (C == 1)
      return {Flags::C};

    case LLFC_ULE:  // Unsigned less or equal (C == 0 || Z == 1)
    case LLFC_UGT:  // Unsigned greater than (C == 1 && Z == 0)
      return {Flags::C, Flags::Z};

    case LLFC_NEG:  // Negative (N == 1)
    case LLFC_POS:  // Positive (N == 0)
      return {Flags::N};

    case LLFC_O:   // Overflow (V == 1)
    case LLFC_NO:  // No overflow (V == 0)
      return {Flags::V};

    default:
      return {};
  }
}

// ============================================================================
// Intrinsics
// ============================================================================

[[nodiscard]] std::string TIC28XArchitecture::GetIntrinsicName(
    uint32_t intrinsic) {
  switch (intrinsic) {
    case TIC28X_INTRIN_BITREVERSE:
      return "__bitreverse";
    case TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2:
      return "vcadd";
    case TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4:
      return "vcadd";
    case TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      return "vccmac";
    case TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4:
      return "vccmac";
    case TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0:
      return "vccmpy";
    case TIC28X_INTRIN_VCCON_VRA:
      return "vccon";
    case TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2:
      return "vcdadd16";
    case TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2:
      return "vcdsub16";
    case TIC28X_INTRIN_VCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      return "vcmac";
    case TIC28X_INTRIN_VCMAC_VR7_VR6_VR5_VR4:
      return "vcmac";
    case TIC28X_INTRIN_VCMAG_VRB_VRA:
      return "vcmag";
    case TIC28X_INTRIN_VCMPY_VR3_VR2_VR1_VR0:
      return "vcmpy";
    default:
      return "";
  }
}

[[nodiscard]] std::vector<uint32_t> TIC28XArchitecture::GetAllIntrinsics() {
  return {TIC28X_INTRIN_BITREVERSE,
          TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2,
          TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4,
          TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0,
          TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4,
          TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0,
          TIC28X_INTRIN_VCCON_VRA,
          TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2,
          TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2,
          TIC28X_INTRIN_VCMAC_VR5_VR4_VR3_VR2_VR1_VR0,
          TIC28X_INTRIN_VCMAC_VR7_VR6_VR5_VR4,
          TIC28X_INTRIN_VCMAG_VRB_VRA,
          TIC28X_INTRIN_VCMPY_VR3_VR2_VR1_VR0};
}

[[nodiscard]] std::vector<BN::NameAndType>
TIC28XArchitecture::GetIntrinsicInputs(uint32_t intrinsic) {
  switch (intrinsic) {
    case TIC28X_INTRIN_BITREVERSE:
      // Takes a 32-bit unsigned integer as input
      return {BN::NameAndType("value", BN::Type::IntegerType(4, false))};
    case TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2:
      // Inputs: VR5=Re(X), VR4=Im(X), VR3=Re(Y), VR2=Im(Y), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10]
      return {
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4:
      // Inputs: VR7=Re(X), VR6=Im(X), VR5=Re(Y), VR4=Im(Y), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10]
      return {
          BN::NameAndType("vr7", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr6", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      // Inputs: VR0 (first complex operand), VR1 (second complex operand),
      //         VR2=Im(prev), VR3=Re(prev),
      //         VR4=Im(accum), VR5=Re(accum), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10], CPACK[14]
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0:
      // Inputs: VR0 (first complex operand), VR1 (second complex operand),
      // VSTATUS VSTATUS carries CPACK[14] to select channel ordering
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCCON_VRA:
      // Inputs: VRa (complex value), VSTATUS (CPACK, SAT bits)
      return {
          BN::NameAndType("vra", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4:
      // Inputs: VR0-VR3 (temps), VR4=Im(accum), VR5=Re(accum),
      //         VR6=Im(accum2), VR7=Re(accum2), mem32 (data),
      //         XAR7 (pointer for second operand), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10], CPACK[14]
      // Note: XAR7 post-increment is handled explicitly in the lift,
      //       not inside the intrinsic.
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr6", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr7", BN::Type::IntegerType(4, true)),
          BN::NameAndType("mem32", BN::Type::IntegerType(4, false)),
          BN::NameAndType("xar7", BN::Type::IntegerType(4, false)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2:
      // Inputs: VR4 (packed Re/Im 16-bit), VR3=Re(Y) (32-bit),
      //         VR2=Im(Y) (32-bit), VSTATUS
      // VSTATUS carries CPACK[14], SHIFTL[9:5], SHIFTR[4:0], RND[11], SAT[10]
      return {
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2:
      // Inputs: VR4 (packed Re/Im 16-bit), VR3=Re(Y) (32-bit),
      //         VR2=Im(Y) (32-bit), VSTATUS
      // VSTATUS carries CPACK[14], SHIFTL[9:5], SHIFTR[4:0], RND[11], SAT[10]
      return {
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      // Inputs: VR0 (first complex operand), VR1 (second complex operand),
      //         VR2=Im(prev), VR3=Re(prev),
      //         VR4=Im(accum), VR5=Re(accum), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10], CPACK[14]
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCMAC_VR7_VR6_VR5_VR4:
      // Inputs: VR0-VR3 (temps), VR4=Im(accum), VR5=Re(accum),
      //         VR6=Im(accum2), VR7=Re(accum2), mem32 (data),
      //         XAR7 (pointer for second operand), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10], CPACK[14]
      // Note: XAR7 post-increment is handled explicitly in the lift,
      //       not inside the intrinsic.
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr2", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr3", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr4", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr5", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr6", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr7", BN::Type::IntegerType(4, true)),
          BN::NameAndType("mem32", BN::Type::IntegerType(4, false)),
          BN::NameAndType("xar7", BN::Type::IntegerType(4, false)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCMAG_VRB_VRA:
      // Inputs: VRa (complex value: VRaH=Re, VRaL=Im), VSTATUS
      // VSTATUS carries SHIFTR[4:0], RND[11], SAT[10]
      return {
          BN::NameAndType("vra", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    case TIC28X_INTRIN_VCMPY_VR3_VR2_VR1_VR0:
      // Inputs: VR0 (first complex operand), VR1 (second complex operand),
      // VSTATUS carries CPACK[14] to select channel ordering and SAT[10]
      return {
          BN::NameAndType("vr0", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vr1", BN::Type::IntegerType(4, true)),
          BN::NameAndType("vstatus", BN::Type::IntegerType(4, false)),
      };
    default:
      return {};
  }
}

[[nodiscard]] std::vector<BN::Confidence<BN::Ref<BN::Type>>>
TIC28XArchitecture::GetIntrinsicOutputs(uint32_t intrinsic) {
  switch (intrinsic) {
    case TIC28X_INTRIN_BITREVERSE:
      // Returns a 32-bit unsigned integer
      return {BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2:
      // Outputs: VR5=Re(Z), VR4=Im(Z), VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4:
      // Outputs: VR7=Re(Z), VR6=Im(Z), VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      // Outputs: VR5=Re(accum), VR4=Im(accum), VR3=Re(mult), VR2=Im(mult),
      //          VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCCMPY_VR3_VR2_VR1_VR0:
      // Outputs: VR3=Re(Z), VR2=Im(Z), VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCCON_VRA:
      // Outputs: VRa (conjugated), VSTATUS (OVFI flag updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCCMAC_VR7_VR6_VR5_VR4:
      // Outputs: VR7=Re(accum2), VR6=Im(accum2), VR5=Re(accum),
      //          VR4=Im(accum), VR3=Re(mult), VR2=Im(mult),
      //          VR1=temp, VR0=temp, VSTATUS (OVFR/OVFI flags updated)
      // Note: XAR7 post-increment is handled explicitly in the lift.
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCDADD16_VR5_VR4_VR3_VR2:
      // Outputs: VR5 (packed Re(Z)/Im(Z) 16-bit), VSTATUS (OVFR/OVFI updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCDSUB16_VR6_VR4_VR3_VR2:
      // Outputs: VR6 (packed Re(Z)/Im(Z) 16-bit), VSTATUS (OVFR/OVFI updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCMAC_VR5_VR4_VR3_VR2_VR1_VR0:
      // Outputs: VR5=Re(accum), VR4=Im(accum), VR3=Re(mult), VR2=Im(mult),
      //          VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCMAC_VR7_VR6_VR5_VR4:
      // Outputs: VR7=Re(accum2), VR6=Im(accum2), VR5=Re(accum),
      //          VR4=Im(accum), VR3=Re(mult), VR2=Im(mult),
      //          VR1=temp, VR0=temp, VSTATUS (OVFR/OVFI flags updated)
      // Note: XAR7 post-increment is handled explicitly in the lift.
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCMAG_VRB_VRA:
      // Outputs: VRb (magnitude result), VSTATUS (OVFR flag updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, false)};
    case TIC28X_INTRIN_VCMPY_VR3_VR2_VR1_VR0:
      // Outputs: VR3=Re(Z), VR2=Im(Z), VSTATUS (OVFR/OVFI flags updated)
      return {BN::Type::IntegerType(4, true), BN::Type::IntegerType(4, true),
              BN::Type::IntegerType(4, false)};
    default:
      return {};
  }
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