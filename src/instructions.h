// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_INSTRUCTIONS_H
#define TIC28X_INSTRUCTIONS_H

#include <binaryninjaapi.h>

#include "architecture.h"
#include "opcodes.h"
#include "sizes.h"

namespace BN = BinaryNinja;

namespace TIC28X {

/* Instruction Operand Extractors */
uint8_t FpuGetRegA_I(uint32_t data);
uint32_t FpuSetRegA_I(uint32_t opcode, uint8_t a);
uint8_t FpuGetRegB_I(uint32_t data);
uint32_t FpuSetRegB_I(uint32_t opcode, uint8_t b);
uint8_t FpuGetRegC_I(uint32_t data);
uint32_t FpuSetRegC_I(uint32_t opcode, uint8_t c);
uint16_t FpuGet16FHi_I(uint32_t data);
uint32_t FpuSet16FHi_I(uint32_t opcode, uint16_t i);

uint8_t FpuGetRegA_II(uint32_t data);
uint32_t FpuSetRegA_II(uint32_t opcode, uint8_t a);
uint8_t FpuGetRegD_II(uint32_t data);
uint32_t FpuSetRegD_II(uint32_t opcode, uint8_t d);
uint8_t FpuGetRegE_II(uint32_t data);
uint32_t FpuSetRegE_II(uint32_t opcode, uint8_t e);
uint8_t FpuGetRegF_II(uint32_t data);
uint32_t FpuSetRegF_II(uint32_t opcode, uint8_t f);

uint8_t FpuGetRegA_III(uint32_t data);
uint32_t FpuSetRegA_III(uint32_t opcode, uint8_t a);
uint16_t FpuGet16FHi_III(uint32_t data);
uint32_t FpuSet16FHi_III(uint32_t opcode, uint16_t i);

uint8_t FpuGetRegD_IV(uint32_t data);
uint32_t FpuSetRegD_IV(uint32_t opcode, uint8_t d);
uint8_t FpuGetRegE_IV(uint32_t data);
uint32_t FpuSetRegE_IV(uint32_t opcode, uint8_t e);
uint8_t FpuGetRegF_IV(uint32_t data);
uint32_t FpuSetRegF_IV(uint32_t opcode, uint8_t f);

uint8_t FpuGetMem(uint32_t data);
uint32_t FpuSetMem(uint32_t opcode, uint8_t mem);

/**
 * 16-bit Instruction class
 */
class Instruction2Byte : public Instruction {
 public:
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;
  size_t GetLength() override { return Sizes::_2_BYTES; }
};

/**
 * 32-bit Instruction class
 */
class Instruction4Byte : public Instruction {
 public:
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;
  size_t GetLength() override { return Sizes::_4_BYTES; }
};

/* Helper Functions */
std::optional<std::shared_ptr<Instruction>> DecodeInstruction(
    const uint8_t* data,
    const std::vector<std::optional<std::shared_ptr<Instruction>>>&
        instruction_list,
    ObjectMode objmode);

std::vector<std::optional<std::shared_ptr<Instruction>>>
GenerateInstructionVector();

/* Final CPU Instruction Classes
 * Note: the instruction data is declared public so it can be accessed for
 * testing without having to instantiate an object. We can't use static methods
 * for getting the opcode, mask, etc. because we need these to be defined
 * in the abstract Instruction class so that they work for any arbitrary
 * Instruction object.
 */
class Aborti final : public Instruction2Byte {
 public:
  Aborti() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ABORTI;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Aborti";
  static constexpr auto op_name = "aborti";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
};

class AbsAcc final : public Instruction2Byte {
 public:
  AbsAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ABS_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "AbsAcc";
  static constexpr auto op_name = "abs";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
};

class AbstcAcc final : public Instruction2Byte {
 public:
  AbstcAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ABSTC_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "AbstcAcc";
  static constexpr auto op_name = "abstc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
};

class AddAccConst16Shift final : public Instruction4Byte {
 public:
  AddAccConst16Shift() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_CONST16_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "AddAccConst16Shift";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAccLoc16ShiftT final : public Instruction4Byte {
 public:
  AddAccLoc16ShiftT() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_LOC16_SHIFT_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "AddAccLoc16ShiftT";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAccLoc16 final : public Instruction2Byte {
 public:
  AddAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AddAccLoc16";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAccLoc16Shift1_15 final : public Instruction4Byte {
 public:
  AddAccLoc16Shift1_15() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_LOC16_SHIFT1_15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "AddAccLoc16Shift1_15";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAccLoc16Shift16 final : public Instruction2Byte {
 public:
  AddAccLoc16Shift16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_LOC16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AddAccLoc16Shift16";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAccLoc16Shift0_15 final : public Instruction2Byte {
 public:
  AddAccLoc16Shift0_15() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_ACC_LOC16_SHIFT0_15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F000;
  static constexpr auto full_name = "AddAccLoc16Shift0_15";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddAxLoc16 final : public Instruction2Byte {
 public:
  AddAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AddAxLoc16";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddLoc16Ax final : public Instruction2Byte {
 public:
  AddLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AddLoc16Ax";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddLoc16Const16 final : public Instruction4Byte {
 public:
  AddLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADD_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "AddLoc16Const16";
  static constexpr auto op_name = "add";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddbAccConst8 final : public Instruction2Byte {
 public:
  AddbAccConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDB_ACC_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AddbAccConst8";
  static constexpr auto op_name = "addb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddbAxConst8 final : public Instruction2Byte {
 public:
  AddbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AddbAxConst8";
  static constexpr auto op_name = "addb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddbSpConst7 final : public Instruction2Byte {
 public:
  AddbSpConst7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDB_SP_CONST7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF80;
  static constexpr auto full_name = "AddbSpConst7";
  static constexpr auto op_name = "addb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst7(uint32_t data);
  static uint32_t SetConst7(uint8_t const7);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddbXarnConst7 final : public Instruction2Byte {
 public:
  AddbXarnConst7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDB_XARN_CONST7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F880;
  static constexpr auto full_name = "AddbXarnConst7";
  static constexpr auto op_name = "addb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint8_t GetConst7(uint32_t data);
  static uint32_t SetConst7(uint8_t const7);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddclAccLoc32 final : public Instruction4Byte {
 public:
  AddclAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDCL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "AddclAccLoc32";
  static constexpr auto op_name = "addc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddcuAccLoc16 final : public Instruction2Byte {
 public:
  AddcuAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDCU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AddcuAccLoc16";
  static constexpr auto op_name = "addcu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddlAccLoc32 final : public Instruction2Byte {
 public:
  AddlAccLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AddlAccLoc32";
  static constexpr auto op_name = "addl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddlAccPShiftPm final : public Instruction2Byte {
 public:
  AddlAccPShiftPm() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDL_ACC_P_SHIFT_PM;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "AddlAccPShiftPm";
  static constexpr auto op_name = "addl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddlLoc32Acc final : public Instruction4Byte {
 public:
  AddlLoc32Acc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDL_LOC32_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "AddlLoc32Acc";
  static constexpr auto op_name = "addl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AdduAccLoc16 final : public Instruction2Byte {
 public:
  AdduAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AdduAccLoc16";
  static constexpr auto op_name = "addu";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddulPLoc32 final : public Instruction4Byte {
 public:
  AddulPLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDUL_P_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "AddulPLoc32";
  static constexpr auto op_name = "addul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AddulAccLoc32 final : public Instruction4Byte {
 public:
  AddulAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDUL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "AddulAccLoc32";
  static constexpr auto op_name = "addul";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AdrkImm8 final : public Instruction2Byte {
 public:
  AdrkImm8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADRK_IMM8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AdrkImm8";
  static constexpr auto op_name = "adrk";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetImm8(uint32_t data);
  static uint32_t SetImm8(uint8_t imm8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndAccConst16Shift0_15 final : public Instruction4Byte {
 public:
  AndAccConst16Shift0_15() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_ACC_CONST16_SHIFT0_15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "AndAccConst16Shift0_15";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndAccConst16Shift16 final : public Instruction4Byte {
 public:
  AndAccConst16Shift16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_ACC_CONST16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "AndAccConst16Shift16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndAccLoc16 final : public Instruction2Byte {
 public:
  AndAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "AndAccLoc16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndAxLoc16Const16 final : public Instruction4Byte {
 public:
  AndAxLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_AX_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE000000;
  static constexpr auto full_name = "AndAxLoc16Const16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndIerConst16 final : public Instruction4Byte {
 public:
  AndIerConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_IER_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "AndIerConst16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndIfrConst16 final : public Instruction4Byte {
 public:
  AndIfrConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_IFR_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "AndIfrConst16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndLoc16Ax final : public Instruction2Byte {
 public:
  AndLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AndLoc16Ax";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndAxLoc16 final : public Instruction2Byte {
 public:
  AndAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AndAxLoc16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndLoc16Const16 final : public Instruction4Byte {
 public:
  AndLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::AND_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "AndLoc16Const16";
  static constexpr auto op_name = "and";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AndbAxConst8 final : public Instruction2Byte {
 public:
  AndbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ANDB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "AndbAxConst8";
  static constexpr auto op_name = "andb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Asp final : public Instruction2Byte {
 public:
  Asp() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Asp";
  static constexpr auto op_name = "asp";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AsrAxShift final : public Instruction2Byte {
 public:
  AsrAxShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASR_AX_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFE0;
  static constexpr auto full_name = "AsrAxShift";
  static constexpr auto op_name = "asr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AsrAxT final : public Instruction2Byte {
 public:
  AsrAxT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASR_AX_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "AsrAxT";
  static constexpr auto op_name = "asr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Asr64AccPShift final : public Instruction2Byte {
 public:
  Asr64AccPShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASR64_ACC_P_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "Asr64AccPShift";
  static constexpr auto op_name = "asr64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Asr64AccPT final : public Instruction2Byte {
 public:
  Asr64AccPT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASR64_ACC_P_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Asr64AccPT";
  static constexpr auto op_name = "asr64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class AsrlAccT final : public Instruction2Byte {
 public:
  AsrlAccT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ASRL_ACC_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "AsrlAccT";
  static constexpr auto op_name = "asrl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class BOff16Cond final : public Instruction4Byte {
 public:
  BOff16Cond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::B_OFF16_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "BOff16Cond";
  static constexpr auto op_name = "b";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetOff16(uint32_t data);
  static uint32_t SetOff16(uint16_t off16);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class BanzOff16Arn final : public Instruction4Byte {
 public:
  BanzOff16Arn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::BANZ_OFF16_ARN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "BanzOff16Arn";
  static constexpr auto op_name = "banz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetOff16(uint32_t data);
  static uint32_t SetOff16(uint16_t off16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class BarOff16ArnArmEq final : public Instruction4Byte {
 public:
  BarOff16ArnArmEq() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::BAR_OFF16_ARN_ARM_EQ;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "BarOff16ArnArmEq";
  static constexpr auto op_name = "bar";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetOff16(uint32_t data);
  static uint32_t SetOff16(uint16_t off16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint8_t GetRegM(uint32_t data);
  static uint32_t SetRegM(uint8_t m);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class BarOff16ArnArmNeq final : public Instruction4Byte {
 public:
  BarOff16ArnArmNeq() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::BAR_OFF16_ARN_ARM_NEQ;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "BarOff16ArnArmNeq";
  static constexpr auto op_name = "bar";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetOff16(uint32_t data);
  static uint32_t SetOff16(uint16_t off16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint8_t GetRegM(uint32_t data);
  static uint32_t SetRegM(uint8_t m);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class BfOff16Cond final : public Instruction4Byte {
 public:
  BfOff16Cond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::BF_OFF16_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "BfOff16Cond";
  static constexpr auto op_name = "b";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetOff16(uint32_t data);
  static uint32_t SetOff16(uint16_t off16);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcAmode final : public Instruction2Byte {
 public:
  ClrcAmode() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_AMODE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "ClrcAmode";
  static constexpr auto op_name = "clrc amode";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcM0M1Map final : public Instruction2Byte {
 public:
  ClrcM0M1Map() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_M0M1MAP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "ClrcM0M1Map";
  static constexpr auto op_name = "clrc m0m1map";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcObjmode final : public Instruction2Byte {
 public:
  ClrcObjmode() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_OBJMODE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "ClrcObjmode";
  static constexpr auto op_name = "clrc objmode";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcOvc final : public Instruction2Byte {
 public:
  ClrcOvc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_OVC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "ClrcOvc";
  static constexpr auto op_name = "clrc ovc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcXf final : public Instruction2Byte {
 public:
  ClrcXf() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_XF;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "ClrcXf";
  static constexpr auto op_name = "clrc xf";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ClrcMode final : public Instruction2Byte {
 public:
  ClrcMode() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CLRC_MODE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "ClrcMode";
  static constexpr auto op_name = "clrc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetMode(uint32_t data);
  static uint32_t SetMode(uint8_t mode);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CmpAxLoc16 final : public Instruction2Byte {
 public:
  CmpAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMP_AX_LOC15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "CmpAxLoc16";
  static constexpr auto op_name = "cmp";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CmpLoc16Const16 final : public Instruction4Byte {
 public:
  CmpLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMP_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "CmpLoc16Const16";
  static constexpr auto op_name = "cmp";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmp64AccP final : public Instruction2Byte {
 public:
  Cmp64AccP() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMP64_ACC_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Cmp64AccP";
  static constexpr auto op_name = "cmp64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CmpbAxConst8 final : public Instruction2Byte {
 public:
  CmpbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "CmpbAxConst8";
  static constexpr auto op_name = "cmpb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CmplAccLoc32 final : public Instruction2Byte {
 public:
  CmplAccLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "CmplAccLoc32";
  static constexpr auto op_name = "cmpl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CmplAccPShiftPm final : public Instruction2Byte {
 public:
  CmplAccPShiftPm() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPL_ACC_P_SHIFT_PM;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "CmplAccPShiftPm";
  static constexpr auto op_name = "cmpl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpr0 final : public Instruction2Byte {
 public:
  Cmpr0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPR_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Cmpr0";
  static constexpr auto op_name = "cmpr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpr1 final : public Instruction2Byte {
 public:
  Cmpr1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPR_1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Cmpr1";
  static constexpr auto op_name = "cmpr1";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpr2 final : public Instruction2Byte {
 public:
  Cmpr2() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPR_2;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Cmpr2";
  static constexpr auto op_name = "cmpr2";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpr3 final : public Instruction2Byte {
 public:
  Cmpr3() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPR_3;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Cmpr3";
  static constexpr auto op_name = "cmpr3";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class CsbAcc final : public Instruction2Byte {
 public:
  CsbAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CSB_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "CsbAcc";
  static constexpr auto op_name = "csb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class DecLoc16 final : public Instruction2Byte {
 public:
  DecLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::DEC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "DecLoc16";
  static constexpr auto op_name = "dec";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class DmacAccPLoc32Xar7 final : public Instruction4Byte {
 public:
  DmacAccPLoc32Xar7() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::DMAC_ACCC_P_LOC32_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "DmacAccPLoc32Xar7";
  static constexpr auto op_name = "dmac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class DmacAccPLoc32Xar7Postinc final : public Instruction4Byte {
 public:
  DmacAccPLoc32Xar7Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::DMAC_ACCC_P_LOC32_XAR7_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "DmacAccPLoc32Xar7Postinc";
  static constexpr auto op_name = "dmac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class DmovLoc16 final : public Instruction2Byte {
 public:
  DmovLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::DMOV_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "DmovLoc16";
  static constexpr auto op_name = "dmov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Eallow final : public Instruction2Byte {
 public:
  Eallow() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::EALLOW;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Eallow";
  static constexpr auto op_name = "eallow";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Edis final : public Instruction2Byte {
 public:
  Edis() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::EDIS;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Edis";
  static constexpr auto op_name = "edis";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Estop0 final : public Instruction2Byte {
 public:
  Estop0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ESTOP0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Estop0";
  static constexpr auto op_name = "estop0";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Estop1 final : public Instruction2Byte {
 public:
  Estop1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ESTOP1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Estop1";
  static constexpr auto op_name = "estop1";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class FfcXar7Const22 final : public Instruction4Byte {
 public:
  FfcXar7Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::FFC_XAR7_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "FfcXar7Const22";
  static constexpr auto op_name = "ffc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class FlipAx final : public Instruction2Byte {
 public:
  FlipAx() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::FLIP_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "FlipAx";
  static constexpr auto op_name = "flip";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class IackConst16 final : public Instruction4Byte {
 public:
  IackConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IACK_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "IackConst16";
  static constexpr auto op_name = "iack";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Idle final : public Instruction2Byte {
 public:
  Idle() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IDLE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Idle";
  static constexpr auto op_name = "idle";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImaclPLoc32Xar7 final : public Instruction4Byte {
 public:
  ImaclPLoc32Xar7() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMACL_P_LOC32_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImaclPLoc32Xar7";
  static constexpr auto op_name = "imacl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImaclPLoc32Xar7Postinc final : public Instruction4Byte {
 public:
  ImaclPLoc32Xar7Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMACL_P_LOC32_XAR7_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImaclPLoc32Xar7Postinc";
  static constexpr auto op_name = "imacl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImpyalPXtLoc32 final : public Instruction4Byte {
 public:
  ImpyalPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMPYAL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImpyalPXtLoc32";
  static constexpr auto op_name = "impyal";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImpylAccXtLoc32 final : public Instruction4Byte {
 public:
  ImpylAccXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMPYL_ACC_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImpylAccXtLoc32";
  static constexpr auto op_name = "impyl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImpylPXtLoc32 final : public Instruction4Byte {
 public:
  ImpylPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMPYL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImpylPXtLoc32";
  static constexpr auto op_name = "impyl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImpyslPXtLoc32 final : public Instruction4Byte {
 public:
  ImpyslPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMPYSL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImpyslPXtLoc32";
  static constexpr auto op_name = "impysl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ImpyxulPXtLoc32 final : public Instruction4Byte {
 public:
  ImpyxulPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IMPYXUL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ImpyxulPXtLoc32";
  static constexpr auto op_name = "impyxul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class InLoc16Pa final : public Instruction4Byte {
 public:
  InLoc16Pa() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IN_LOC16_PA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "InLoc16Pa";
  static constexpr auto op_name = "in";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class IncLoc16 final : public Instruction2Byte {
 public:
  IncLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::INC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "IncLoc16";
  static constexpr auto op_name = "inc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class IntrIntx final : public Instruction2Byte {
 public:
  IntrIntx() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::INTR_INTX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "IntrIntx";
  static constexpr auto op_name = "intr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetIntrX(uint32_t data);
  static uint32_t SetIntrX(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class IntrNmi final : public Instruction2Byte {
 public:
  IntrNmi() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::INTR_NMI;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "IntrNmi";
  static constexpr auto op_name = "intr nmi";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class IntrEmuint final : public Instruction2Byte {
 public:
  IntrEmuint() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::INTR_EMUINT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "IntrEmuint";
  static constexpr auto op_name = "intr emuint";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Iret final : public Instruction2Byte {
 public:
  Iret() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::IRET;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Iret";
  static constexpr auto op_name = "iret";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LbXar7 final : public Instruction2Byte {
 public:
  LbXar7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LB_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "LbXar7";
  static constexpr auto op_name = "lb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LbConst22 final : public Instruction4Byte {
 public:
  LbConst22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LB_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "LbConst22";
  static constexpr auto op_name = "lb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LcXar7 final : public Instruction2Byte {
 public:
  LcXar7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LC_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "LcXar7";
  static constexpr auto op_name = "lc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LcConst22 final : public Instruction4Byte {
 public:
  LcConst22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LC_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "LcConst22";
  static constexpr auto op_name = "lc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LcrConst22 final : public Instruction4Byte {
 public:
  LcrConst22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LCR_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "LcrConst22";
  static constexpr auto op_name = "lcr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LcrXarn final : public Instruction2Byte {
 public:
  LcrXarn() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LCR_XARN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "LcrXarn";
  static constexpr auto op_name = "lcr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LoopnzLoc16Const16 final : public Instruction4Byte {
 public:
  LoopnzLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LOOPNZ_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "LoopnzLoc16Const16";
  static constexpr auto op_name = "loopnz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LoopzLoc16Const16 final : public Instruction4Byte {
 public:
  LoopzLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LOOPZ_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "LoopzLoc16Const16";
  static constexpr auto op_name = "loopz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lpaddr final : public Instruction2Byte {
 public:
  Lpaddr() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LPADDR;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lpaddr";
  static constexpr auto op_name = "lpaddr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lret final : public Instruction2Byte {
 public:
  Lret() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LRET;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lret";
  static constexpr auto op_name = "lret";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lrete final : public Instruction2Byte {
 public:
  Lrete() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LRETE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lrete";
  static constexpr auto op_name = "lrete";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lretr final : public Instruction2Byte {
 public:
  Lretr() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LRETR;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lretr";
  static constexpr auto op_name = "lretr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LslAccShift final : public Instruction2Byte {
 public:
  LslAccShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL_ACC_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "LslAccShift";
  static constexpr auto op_name = "lsl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LslAccT final : public Instruction2Byte {
 public:
  LslAccT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL_ACC_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "LslAccT";
  static constexpr auto op_name = "lsl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LslAxShift final : public Instruction2Byte {
 public:
  LslAxShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL_AX_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFE0;
  static constexpr auto full_name = "LslAxShift";
  static constexpr auto op_name = "lsl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LslAxT final : public Instruction2Byte {
 public:
  LslAxT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL_AX_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "LslAxT";
  static constexpr auto op_name = "lsl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lsl64AccPShift final : public Instruction2Byte {
 public:
  Lsl64AccPShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL64_ACC_P_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "Lsl64AccPShift";
  static constexpr auto op_name = "lsl64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lsl64AccPT final : public Instruction2Byte {
 public:
  Lsl64AccPT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSL64_ACC_P_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lsl64AccPT";
  static constexpr auto op_name = "lsl64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LsllAccT final : public Instruction2Byte {
 public:
  LsllAccT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSLL_ACC_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "LsllAccT";
  static constexpr auto op_name = "lsll";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LsrAxShift final : public Instruction2Byte {
 public:
  LsrAxShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSR_AX_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFE0;
  static constexpr auto full_name = "LsrAxShift";
  static constexpr auto op_name = "lsr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LsrAxT final : public Instruction2Byte {
 public:
  LsrAxT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSR_AX_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "LsrAxT";
  static constexpr auto op_name = "lsr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lsr64AccPShift final : public Instruction2Byte {
 public:
  Lsr64AccPShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSR64_ACC_P_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "Lsr64AccPShift";
  static constexpr auto op_name = "lsr64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Lsr64AccPT final : public Instruction2Byte {
 public:
  Lsr64AccPT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSR64_ACC_P_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Lsr64AccPT";
  static constexpr auto op_name = "lsr64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class LsrlAccT final : public Instruction2Byte {
 public:
  LsrlAccT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::LSRL_ACC_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "LsrlAccT";
  static constexpr auto op_name = "lsrl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MacPLoc16Pma final : public Instruction4Byte {
 public:
  MacPLoc16Pma() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAC_P_LOC16_PMA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MacPLoc16Pma";
  static constexpr auto op_name = "mac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MacPLoc16Xar7 final : public Instruction4Byte {
 public:
  MacPLoc16Xar7() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAC_P_LOC16_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MacPLoc16Xar7";
  static constexpr auto op_name = "mac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MacPLoc16Xar7Postinc final : public Instruction4Byte {
 public:
  MacPLoc16Xar7Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAC_P_LOC16_XAR7_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MacPLoc16Xar7Postinc";
  static constexpr auto op_name = "mac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MaxAxLoc16 final : public Instruction4Byte {
 public:
  MaxAxLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAX_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFEFF00;
  static constexpr auto full_name = "MaxAxLoc16";
  static constexpr auto op_name = "max";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MaxculPLoc32 final : public Instruction4Byte {
 public:
  MaxculPLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAXCUL_P_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MaxculPLoc32";
  static constexpr auto op_name = "maxcul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MaxlAccLoc32 final : public Instruction4Byte {
 public:
  MaxlAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAXL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MaxlAccLoc32";
  static constexpr auto op_name = "maxl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MinAxLoc16 final : public Instruction4Byte {
 public:
  MinAxLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MIN_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFEFF00;
  static constexpr auto full_name = "MinAxLoc16";
  static constexpr auto op_name = "min";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MinculPLoc32 final : public Instruction4Byte {
 public:
  MinculPLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MINCUL_P_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "MinculPLoc32";
  static constexpr auto op_name = "mincul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MinlAccLoc32 final : public Instruction4Byte {
 public:
  MinlAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MINL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MinlAccLoc32";
  static constexpr auto op_name = "minl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovMem16Loc16 final : public Instruction4Byte {
 public:
  MovMem16Loc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_MEM16_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MovMem16Loc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccConst16Shift final : public Instruction4Byte {
 public:
  MovAccConst16Shift() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_CONST16_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "MovAccConst16Shift";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16ShiftT final : public Instruction4Byte {
 public:
  MovAccLoc16ShiftT() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_SHIFT_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MovAccLoc16ShiftT";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16Objmode1 final : public Instruction2Byte {
 public:
  MovAccLoc16Objmode1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_OBJMDOE1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovAccLoc16Objmode1";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16Objmode0 final : public Instruction2Byte {
 public:
  MovAccLoc16Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_OBJMDOE0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovAccLoc16Objmode0";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16Shift1_15Objmode1 final : public Instruction4Byte {
 public:
  MovAccLoc16Shift1_15Objmode1() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_SHIFT1_15_OBJMODE1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "MovAccLoc16Shift1_15Objmode1";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16Shift1_15Objmode0 final : public Instruction2Byte {
 public:
  MovAccLoc16Shift1_15Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_SHIFT1_15_OBJMODE0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F000;
  static constexpr auto full_name = "MovAccLoc16Shift1_15Objmode0";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAccLoc16Shift16 final : public Instruction2Byte {
 public:
  MovAccLoc16Shift16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_ACC_LOC16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovAccLoc16Shift16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAr6Loc16 final : public Instruction2Byte {
 public:
  MovAr6Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_AR6_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovAr6Loc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAr7Loc16 final : public Instruction2Byte {
 public:
  MovAr7Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_AR7_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovAr7Loc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovAxLoc16 final : public Instruction2Byte {
 public:
  MovAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovAxLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovDpConst10 final : public Instruction2Byte {
 public:
  MovDpConst10() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_DP_CONST10;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FC00;
  static constexpr auto full_name = "MovDpConst10";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst10(uint32_t data);
  static uint32_t SetConst10(uint16_t const10);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovIerLoc16 final : public Instruction2Byte {
 public:
  MovIerLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_IER_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovIerLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Const16 final : public Instruction4Byte {
 public:
  MovLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MovLoc16Const16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Mem16 final : public Instruction4Byte {
 public:
  MovLoc16Mem16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_MEM16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MovLoc16Mem16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16_0 final : public Instruction2Byte {
 public:
  MovLoc16_0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovLoc16_0";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16AccShift1 final : public Instruction2Byte {
 public:
  MovLoc16AccShift1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_ACC_SHIFT1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovLoc16AccShift1";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16AccShift2_8Objmode1 final : public Instruction4Byte {
 public:
  MovLoc16AccShift2_8Objmode1() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_ACC_SHIFT2_8_OBJMODE_1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF800;
  static constexpr auto full_name = "MovLoc16AccShift2_8Objmode1";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16AccShift2_8Objmode0 final : public Instruction2Byte {
 public:
  MovLoc16AccShift2_8Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_ACC_SHIFT2_8_OBJMODE_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F800;
  static constexpr auto full_name = "MovLoc16AccShift2_8Objmode0";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Arn final : public Instruction2Byte {
 public:
  MovLoc16Arn() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_ARN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F800;
  static constexpr auto full_name = "MovLoc16Arn";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Ax final : public Instruction2Byte {
 public:
  MovLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovLoc16Ax";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16AxCond final : public Instruction4Byte {
 public:
  MovLoc16AxCond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_AX_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFEF000;
  static constexpr auto full_name = "MovLoc16AxCond";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Ier final : public Instruction2Byte {
 public:
  MovLoc16Ier() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_IER;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovLoc16Ier";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16Ovc final : public Instruction4Byte {
 public:
  MovLoc16Ovc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_OVC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MovLoc16Ovc";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16P final : public Instruction2Byte {
 public:
  MovLoc16P() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovLoc16P";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovLoc16T final : public Instruction2Byte {
 public:
  MovLoc16T() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_LOC16_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovLoc16T";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovOvcLoc16 final : public Instruction4Byte {
 public:
  MovOvcLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_OVC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MovOvcLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovPhLoc16 final : public Instruction2Byte {
 public:
  MovPhLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_PH_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovPhLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovPlLoc16 final : public Instruction2Byte {
 public:
  MovPlLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_PL_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovPlLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovPmAx final : public Instruction2Byte {
 public:
  MovPmAx() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_PM_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "MovPmAx";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovTLoc16 final : public Instruction2Byte {
 public:
  MovTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovTLoc16";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovTl0 final : public Instruction2Byte {
 public:
  MovTl0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_TL_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "MovTl0";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovXarnPc final : public Instruction2Byte {
 public:
  MovXarnPc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOV_XARN_PC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "MovXarnPc";
  static constexpr auto op_name = "mov";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovaTLoc16 final : public Instruction2Byte {
 public:
  MovaTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVA_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovaTLoc16";
  static constexpr auto op_name = "mova";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovadTLoc16 final : public Instruction2Byte {
 public:
  MovadTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVAD_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovadTLoc16";
  static constexpr auto op_name = "movad";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAccConst8 final : public Instruction2Byte {
 public:
  MovbAccConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_ACC_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbAccConst8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAr6Const8 final : public Instruction2Byte {
 public:
  MovbAr6Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_AR6_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbAr6Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAr7Const8 final : public Instruction2Byte {
 public:
  MovbAr7Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_AR7_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbAr7Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAxConst8 final : public Instruction2Byte {
 public:
  MovbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovbAxConst8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAxlsbLoc16 final : public Instruction2Byte {
 public:
  MovbAxlsbLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_AXLSB_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovbAxlsbLoc16";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbAxmsbLoc16 final : public Instruction2Byte {
 public:
  MovbAxmsbLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_AXMSB_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovbAxmsbLoc16";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbLoc16Const8Cond final : public Instruction4Byte {
 public:
  MovbLoc16Const8Cond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_LOC16_CONST8_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "MovbLoc16Const8Cond";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbLoc16Axlsb final : public Instruction2Byte {
 public:
  MovbLoc16Axlsb() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_LOC16_AXLSB;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovbLoc16Axlsb";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbLoc16Axmsb final : public Instruction2Byte {
 public:
  MovbLoc16Axmsb() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_LOC16_AXMSB;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "MovbLoc16Axmsb";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar0Const8 final : public Instruction2Byte {
 public:
  MovbXar0Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR0_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar0Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar1Const8 final : public Instruction2Byte {
 public:
  MovbXar1Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR1_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar1Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar2Const8 final : public Instruction2Byte {
 public:
  MovbXar2Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR2_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar2Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar3Const8 final : public Instruction2Byte {
 public:
  MovbXar3Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR3_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar3Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar4Const8 final : public Instruction2Byte {
 public:
  MovbXar4Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR4_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar4Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar5Const8 final : public Instruction2Byte {
 public:
  MovbXar5Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR5_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar5Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar6Const8 final : public Instruction2Byte {
 public:
  MovbXar6Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR6_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar6Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovbXar7Const8 final : public Instruction2Byte {
 public:
  MovbXar7Const8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVB_XAR7_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovbXar7Const8";
  static constexpr auto op_name = "movb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovdlXtLoc32 final : public Instruction2Byte {
 public:
  MovdlXtLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVDL_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovdlXtLoc32";
  static constexpr auto op_name = "movdl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovhLoc16AccShift1 final : public Instruction2Byte {
 public:
  MovhLoc16AccShift1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVH_LOC16_ACC_SHIFT1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovhLoc16AccShift1";
  static constexpr auto op_name = "movh";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovhLoc16AccShift2_8Objmode1 final : public Instruction4Byte {
 public:
  MovhLoc16AccShift2_8Objmode1() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVH_LOC16_ACC_SHIFT2_8_OBJMODE_1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF800;
  static constexpr auto full_name = "MovhLoc16AccShift2_8Objmode1";
  static constexpr auto op_name = "movh";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovhLoc16AccShift2_8Objmode0 final : public Instruction2Byte {
 public:
  MovhLoc16AccShift2_8Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVH_LOC16_ACC_SHIFT2_8_OBJMODE_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F800;
  static constexpr auto full_name = "MovhLoc16AccShift2_8Objmode0";
  static constexpr auto op_name = "movh";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovhLoc16P final : public Instruction2Byte {
 public:
  MovhLoc16P() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVH_LOC16_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovhLoc16P";
  static constexpr auto op_name = "movh";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlAccLoc32 final : public Instruction2Byte {
 public:
  MovlAccLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlAccLoc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlAccPShiftPm final : public Instruction2Byte {
 public:
  MovlAccPShiftPm() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_ACC_P_SHIFT_PM;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "MovlAccPShiftPm";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Acc final : public Instruction2Byte {
 public:
  MovlLoc32Acc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Acc";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32AccCond final : public Instruction4Byte {
 public:
  MovlLoc32AccCond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_ACC_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "MovlLoc32AccCond";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32P final : public Instruction2Byte {
 public:
  MovlLoc32P() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32P";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar0 final : public Instruction2Byte {
 public:
  MovlLoc32Xar0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar0";
  static constexpr auto op_name = "";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar1 final : public Instruction2Byte {
 public:
  MovlLoc32Xar1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar1";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar2 final : public Instruction2Byte {
 public:
  MovlLoc32Xar2() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR2;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar2";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar3 final : public Instruction2Byte {
 public:
  MovlLoc32Xar3() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR3;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar3";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar4 final : public Instruction2Byte {
 public:
  MovlLoc32Xar4() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR4;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar4";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar5 final : public Instruction2Byte {
 public:
  MovlLoc32Xar5() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR5;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar5";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar6 final : public Instruction2Byte {
 public:
  MovlLoc32Xar6() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR6;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar6";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xar7 final : public Instruction2Byte {
 public:
  MovlLoc32Xar7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xar7";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlLoc32Xt final : public Instruction2Byte {
 public:
  MovlLoc32Xt() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_LOC32_XT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlLoc32Xt";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlPAcc final : public Instruction2Byte {
 public:
  MovlPAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_P_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "MovlPAcc";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlPLoc32 final : public Instruction2Byte {
 public:
  MovlPLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_P_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlPLoc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar0Loc32 final : public Instruction2Byte {
 public:
  MovlXar0Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR0_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar0Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar1Loc32 final : public Instruction2Byte {
 public:
  MovlXar1Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR1_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar1Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar2Loc32 final : public Instruction2Byte {
 public:
  MovlXar2Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR2_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar2Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar3Loc32 final : public Instruction2Byte {
 public:
  MovlXar3Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR3_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar3Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar4Loc32 final : public Instruction2Byte {
 public:
  MovlXar4Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR4_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar4Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar5Loc32 final : public Instruction2Byte {
 public:
  MovlXar5Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR5_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar5Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar6Loc32 final : public Instruction2Byte {
 public:
  MovlXar6Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR6_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar6Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar7Loc32 final : public Instruction2Byte {
 public:
  MovlXar7Loc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR7_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXar7Loc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar0Const22 final : public Instruction4Byte {
 public:
  MovlXar0Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR0_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar0Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar1Const22 final : public Instruction4Byte {
 public:
  MovlXar1Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR1_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar1Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar2Const22 final : public Instruction4Byte {
 public:
  MovlXar2Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR2_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar2Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar3Const22 final : public Instruction4Byte {
 public:
  MovlXar3Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR3_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar3Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar4Const22 final : public Instruction4Byte {
 public:
  MovlXar4Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR4_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar4Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar5Const22 final : public Instruction4Byte {
 public:
  MovlXar5Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR5_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar5Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar6Const22 final : public Instruction4Byte {
 public:
  MovlXar6Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR6_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar6Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXar7Const22 final : public Instruction4Byte {
 public:
  MovlXar7Const22() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XAR7_CONST22;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "MovlXar7Const22";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint32_t GetConst22(uint32_t data);
  static uint32_t SetConst22(uint32_t const22);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovlXtLoc32 final : public Instruction2Byte {
 public:
  MovlXtLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVL_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovlXtLoc32";
  static constexpr auto op_name = "movl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovpTLoc16 final : public Instruction2Byte {
 public:
  MovpTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVP_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovpTLoc16";
  static constexpr auto op_name = "movp";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovsTLoc16 final : public Instruction2Byte {
 public:
  MovsTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVS_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovsTLoc16";
  static constexpr auto op_name = "movs";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovuAccLoc16 final : public Instruction2Byte {
 public:
  MovuAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovuAccLoc16";
  static constexpr auto op_name = "movu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovuLoc16Ovc final : public Instruction4Byte {
 public:
  MovuLoc16Ovc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVU_LOC16_OVC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MovuLoc16Ovc";
  static constexpr auto op_name = "movu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovuOvcLoc16 final : public Instruction4Byte {
 public:
  MovuOvcLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVU_OVC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "MovuOvcLoc16";
  static constexpr auto op_name = "movu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovwDpConst16 final : public Instruction4Byte {
 public:
  MovwDpConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVW_DP_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "MovwDpConst16";
  static constexpr auto op_name = "movw";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovxTlLoc16 final : public Instruction4Byte {
 public:
  MovxTlLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVX_TL_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "MovxTlLoc16";
  static constexpr auto op_name = "movx";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr0Loc16 final : public Instruction2Byte {
 public:
  MovzAr0Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR0_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr0Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr1Loc16 final : public Instruction2Byte {
 public:
  MovzAr1Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR1_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr1Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr2Loc16 final : public Instruction2Byte {
 public:
  MovzAr2Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR2_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr2Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr3Loc16 final : public Instruction2Byte {
 public:
  MovzAr3Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR3_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr3Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr4Loc16 final : public Instruction2Byte {
 public:
  MovzAr4Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR4_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr4Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr5Loc16 final : public Instruction2Byte {
 public:
  MovzAr5Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR5_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr5Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr6Loc16 final : public Instruction2Byte {
 public:
  MovzAr6Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR6_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr6Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzAr7Loc16 final : public Instruction2Byte {
 public:
  MovzAr7Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_AR7_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MovzAr7Loc16";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MovzDpConst10 final : public Instruction2Byte {
 public:
  MovzDpConst10() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MOVZ_DP_CONST10;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FC00;
  static constexpr auto full_name = "MovzDpConst10";
  static constexpr auto op_name = "movz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst10(uint32_t data);
  static uint32_t SetConst10(uint16_t const10);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyAccLoc16Const16 final : public Instruction4Byte {
 public:
  MpyAccLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPY_ACC_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MpyAccLoc16Const16";
  static constexpr auto op_name = "mpy";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyAccTLoc16 final : public Instruction2Byte {
 public:
  MpyAccTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPY_ACC_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyAccTLoc16";
  static constexpr auto op_name = "mpy";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyPLoc16Const16 final : public Instruction4Byte {
 public:
  MpyPLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPY_P_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MpyPLoc16Const16";
  static constexpr auto op_name = "mpy";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyPTLoc16 final : public Instruction2Byte {
 public:
  MpyPTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPY_P_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyPTLoc16";
  static constexpr auto op_name = "mpy";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyaPLoc16Const16 final : public Instruction4Byte {
 public:
  MpyaPLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYA_P_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "MpyaPLoc16Const16";
  static constexpr auto op_name = "mpya";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyaPTLoc16 final : public Instruction2Byte {
 public:
  MpyaPTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYA_P_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyaPTLoc16";
  static constexpr auto op_name = "mpya";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpybAccTConst8 final : public Instruction2Byte {
 public:
  MpybAccTConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYB_ACC_T_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpybAccTConst8";
  static constexpr auto op_name = "mpyb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpybPTConst8 final : public Instruction2Byte {
 public:
  MpybPTConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYB_P_T_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpybPTConst8";
  static constexpr auto op_name = "mpyb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpysPTLoc16 final : public Instruction2Byte {
 public:
  MpysPTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYS_P_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpysPTLoc16";
  static constexpr auto op_name = "mpys";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyuPTLoc16 final : public Instruction2Byte {
 public:
  MpyuPTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYU_P_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyuPTLoc16";
  static constexpr auto op_name = "mpyu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyuAccTLoc16 final : public Instruction2Byte {
 public:
  MpyuAccTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYU_ACC_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyuAccTLoc16";
  static constexpr auto op_name = "mpyu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyxuAccTLoc16 final : public Instruction2Byte {
 public:
  MpyxuAccTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYXU_ACC_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyxuAccTLoc16";
  static constexpr auto op_name = "mpyxu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class MpyxuPTLoc16 final : public Instruction2Byte {
 public:
  MpyxuPTLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MPYXU_P_T_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "MpyxuPTLoc16";
  static constexpr auto op_name = "mpyxu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Nasp final : public Instruction2Byte {
 public:
  Nasp() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NASP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Nasp";
  static constexpr auto op_name = "nasp";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NegAcc final : public Instruction2Byte {
 public:
  NegAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NEG_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NegAcc";
  static constexpr auto op_name = "neg";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NegAx final : public Instruction2Byte {
 public:
  NegAx() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NEG_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "NegAx";
  static constexpr auto op_name = "neg";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Neg64AccP final : public Instruction2Byte {
 public:
  Neg64AccP() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NEG64_ACC_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Neg64AccP";
  static constexpr auto op_name = "neg64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NegtcAcc final : public Instruction2Byte {
 public:
  NegtcAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NEGTC_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NegtcAcc";
  static constexpr auto op_name = "negtc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NopIndArpn final : public Instruction2Byte {
 public:
  NopIndArpn() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NOP_IND_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "NopIndArpn";
  static constexpr auto op_name = "mop";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetInd(uint32_t data);
  static uint32_t SetInd(uint8_t ind);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccInd final : public Instruction2Byte {
 public:
  NormAccInd() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_IND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NormAccInd";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccIndPostinc final : public Instruction2Byte {
 public:
  NormAccIndPostinc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_IND_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NormAccIndPostinc";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccIndPostdec final : public Instruction2Byte {
 public:
  NormAccIndPostdec() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_IND_POSTDEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NormAccIndPostdec";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccInd0Postinc final : public Instruction2Byte {
 public:
  NormAccInd0Postinc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_IND0_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NormAccInd0Postin";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccInd0Postdec final : public Instruction2Byte {
 public:
  NormAccInd0Postdec() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_IND0_POSTDEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NormAccInd0Postdec";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccXarnPostinc final : public Instruction2Byte {
 public:
  NormAccXarnPostinc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_XARN_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "NormAccXarnPostinc";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NormAccXarnPostdec final : public Instruction2Byte {
 public:
  NormAccXarnPostdec() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NORM_ACC_XARN_POSTDEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "NormAccXarnPostdec";
  static constexpr auto op_name = "norm";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NotAcc final : public Instruction2Byte {
 public:
  NotAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NOT_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "NotAcc";
  static constexpr auto op_name = "not";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class NotAx final : public Instruction2Byte {
 public:
  NotAx() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::NOT_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFE;
  static constexpr auto full_name = "NotAx";
  static constexpr auto op_name = "not";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrAccLoc16 final : public Instruction2Byte {
 public:
  OrAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "OrAccLoc16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrAccConst16Shift0_15 final : public Instruction4Byte {
 public:
  OrAccConst16Shift0_15() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_ACC_CONST16_SHIFT0_15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "OrAccConst16Shift0_15";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrAccConst16Shift16 final : public Instruction4Byte {
 public:
  OrAccConst16Shift16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_ACC_CONST16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "OrAccConst16Shift16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrAxLoc16 final : public Instruction2Byte {
 public:
  OrAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "OrAxLoc16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrIerConst16 final : public Instruction4Byte {
 public:
  OrIerConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_IER_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "OrIerConst16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrIfrConst16 final : public Instruction4Byte {
 public:
  OrIfrConst16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_IFR_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "OrIfrConst16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrLoc16Const16 final : public Instruction4Byte {
 public:
  OrLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "OrLoc16Const16";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrLoc16Ax final : public Instruction2Byte {
 public:
  OrLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OR_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "OrLoc16Ax";
  static constexpr auto op_name = "or";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OrbAxConst8 final : public Instruction2Byte {
 public:
  OrbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ORB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "OrbAxConst8";
  static constexpr auto op_name = "orb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class OutPaLoc16 final : public Instruction4Byte {
 public:
  OutPaLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::OUT_PA_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "OutPaLoc16";
  static constexpr auto op_name = "out";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopAr1Ar0 final : public Instruction2Byte {
 public:
  PopAr1Ar0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_AR1_AR0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopAr1Ar0";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopAr3Ar2 final : public Instruction2Byte {
 public:
  PopAr3Ar2() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_AR3_AR2;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopAr3Ar2";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopAr5Ar4 final : public Instruction2Byte {
 public:
  PopAr5Ar4() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_AR5_AR4;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopAr5Ar4";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopAr1hAr0h final : public Instruction2Byte {
 public:
  PopAr1hAr0h() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_AR1H_AR0H;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopAr1hAr0h";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopDbgier final : public Instruction2Byte {
 public:
  PopDbgier() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_DBGIER;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopDbgier";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopDp final : public Instruction2Byte {
 public:
  PopDp() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_DP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopDp";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopDpSt1 final : public Instruction2Byte {
 public:
  PopDpSt1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_DP_ST1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopDpSt1";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopIfr final : public Instruction2Byte {
 public:
  PopIfr() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_IFR;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopIfr";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopLoc16 final : public Instruction2Byte {
 public:
  PopLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "PopLoc16";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopP final : public Instruction2Byte {
 public:
  PopP() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopP";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopRpc final : public Instruction2Byte {
 public:
  PopRpc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_RPC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopRpc";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopSt0 final : public Instruction2Byte {
 public:
  PopSt0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_ST0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopSt0";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopSt1 final : public Instruction2Byte {
 public:
  PopSt1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_ST1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopSt1";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopTSt0 final : public Instruction2Byte {
 public:
  PopTSt0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_T_ST0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopTSt0";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PopXt final : public Instruction2Byte {
 public:
  PopXt() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::POP_XT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PopXt";
  static constexpr auto op_name = "pop";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PreadLoc16Xar7 final : public Instruction2Byte {
 public:
  PreadLoc16Xar7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PREAD_LOC16_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "PreadLoc16Xar7";
  static constexpr auto op_name = "pread";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushAr1Ar0 final : public Instruction2Byte {
 public:
  PushAr1Ar0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_AR1_AR0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushAr1Ar0";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushAr3Ar2 final : public Instruction2Byte {
 public:
  PushAr3Ar2() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_AR3_AR2;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushAr3Ar2";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushAr5Ar4 final : public Instruction2Byte {
 public:
  PushAr5Ar4() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_AR5_AR4;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushAr5Ar4";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushAr1hAr0h final : public Instruction2Byte {
 public:
  PushAr1hAr0h() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_AR1H_AR0H;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushAr1hAr0h";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushDbgier final : public Instruction2Byte {
 public:
  PushDbgier() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_DBGIER;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushDbgier";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushDp final : public Instruction2Byte {
 public:
  PushDp() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_DP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushDp";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushDpSt1 final : public Instruction2Byte {
 public:
  PushDpSt1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_DP_ST1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushDpSt1";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushIfr final : public Instruction2Byte {
 public:
  PushIfr() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_IFR;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushIfr";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushLoc16 final : public Instruction2Byte {
 public:
  PushLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "PushLoc16";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushP final : public Instruction2Byte {
 public:
  PushP() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushP";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushRpc final : public Instruction2Byte {
 public:
  PushRpc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_RPC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushRpc";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushSt0 final : public Instruction2Byte {
 public:
  PushSt0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_ST0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushSt0";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushSt1 final : public Instruction2Byte {
 public:
  PushSt1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_ST1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushSt1";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushTSt0 final : public Instruction2Byte {
 public:
  PushTSt0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_T_ST0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushTSt0";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PushXt final : public Instruction2Byte {
 public:
  PushXt() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PUSH_XT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "PushXt";
  static constexpr auto op_name = "push";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class PwriteXar7Loc16 final : public Instruction2Byte {
 public:
  PwriteXar7Loc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::PWRITE_XAR7_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "PwriteXar7Loc16";
  static constexpr auto op_name = "pwrite";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmaclPLoc32Xar7 final : public Instruction4Byte {
 public:
  QmaclPLoc32Xar7() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMACL_P_LOC32_XAR7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmaclPLoc32Xar7";
  static constexpr auto op_name = "qmacl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmaclPLoc32Xar7Postinc final : public Instruction4Byte {
 public:
  QmaclPLoc32Xar7Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMACL_P_LOC32_XAR7_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmaclPLoc32Xar7Postinc";
  static constexpr auto op_name = "qmacl";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpyalPXtLoc32 final : public Instruction4Byte {
 public:
  QmpyalPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYAL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpyalPXtLoc32";
  static constexpr auto op_name = "qmpyal";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpylPXtLoc32 final : public Instruction4Byte {
 public:
  QmpylPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpylPXtLoc32";
  static constexpr auto op_name = "qmpyl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpylAccXtLoc32 final : public Instruction4Byte {
 public:
  QmpylAccXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYL_ACC_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpylAccXtLoc32";
  static constexpr auto op_name = "qmpyl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpyslPXtLoc32 final : public Instruction4Byte {
 public:
  QmpyslPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYSL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpyslPXtLoc32";
  static constexpr auto op_name = "qmpysl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpyulPXtLoc32 final : public Instruction4Byte {
 public:
  QmpyulPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYUL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpyulPXtLoc32";
  static constexpr auto op_name = "qmpyul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class QmpyxulPXtLoc32 final : public Instruction4Byte {
 public:
  QmpyxulPXtLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::QMPYXUL_P_XT_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "QmpyxulPXtLoc32";
  static constexpr auto op_name = "qmpyxul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class RolAcc final : public Instruction2Byte {
 public:
  RolAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ROL_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "RolAcc";
  static constexpr auto op_name = "rol";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class RorAcc final : public Instruction2Byte {
 public:
  RorAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ROR_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "RorAcc";
  static constexpr auto op_name = "ror";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class RptConst8 final : public Instruction2Byte {
 public:
  RptConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::RPT_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "RptConst8";
  static constexpr auto op_name = "rpt";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class RptLoc16 final : public Instruction2Byte {
 public:
  RptLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::RPT_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "RptLoc16";
  static constexpr auto op_name = "rpt";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SatAcc final : public Instruction2Byte {
 public:
  SatAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SAT_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "SatAcc";
  static constexpr auto op_name = "sat";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Sat64AccP final : public Instruction2Byte {
 public:
  Sat64AccP() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SAT64_ACC_P;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Sat64AccP";
  static constexpr auto op_name = "sat64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbOff8Cond final : public Instruction2Byte {
 public:
  SbOff8Cond() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SB_OFF8_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F000;
  static constexpr auto full_name = "SbOff8Cond";
  static constexpr auto op_name = "sb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetOff8(uint32_t data);
  static uint32_t SetOff8(uint8_t off8);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbbuAccLoc16 final : public Instruction2Byte {
 public:
  SbbuAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBBU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbbuAccLoc16";
  static constexpr auto op_name = "sbbu";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbfOff8Eq final : public Instruction2Byte {
 public:
  SbfOff8Eq() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBF_OFF8_EQ;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbfOff8Eq";
  static constexpr auto op_name = "sbf";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetOff8(uint32_t data);
  static uint32_t SetOff8(uint8_t off8);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbfOff8Neq final : public Instruction2Byte {
 public:
  SbfOff8Neq() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBF_OFF8_NEQ;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbfOff8Neq";
  static constexpr auto op_name = "sbf";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetOff8(uint32_t data);
  static uint32_t SetOff8(uint8_t off8);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbfOff8Tc final : public Instruction2Byte {
 public:
  SbfOff8Tc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBF_OFF8_TC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbfOff8Tc";
  static constexpr auto op_name = "sbf";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetOff8(uint32_t data);
  static uint32_t SetOff8(uint8_t off8);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbfOff8Ntc final : public Instruction2Byte {
 public:
  SbfOff8Ntc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBF_OFF8_NTC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbfOff8Ntc";
  static constexpr auto op_name = "sbf";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetOff8(uint32_t data);
  static uint32_t SetOff8(uint8_t off8);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SbrkConst8 final : public Instruction2Byte {
 public:
  SbrkConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SBRK_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SbrkConst8";
  static constexpr auto op_name = "sbrk";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SetcMode final : public Instruction2Byte {
 public:
  SetcMode() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SETC_MODE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SetcMode";
  static constexpr auto op_name = "setc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetMode(uint32_t data);
  static uint32_t SetMode(uint8_t mode);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SetcM0M1Map final : public Instruction2Byte {
 public:
  SetcM0M1Map() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SETC_M0M1MAP;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "SetcM0M1Map";
  static constexpr auto op_name = "setc m0m1map";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SetcObjmode final : public Instruction2Byte {
 public:
  SetcObjmode() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SETC_OBJMODE;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "SetcObjmode";
  static constexpr auto op_name = "setc objmode";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SetcXf final : public Instruction2Byte {
 public:
  SetcXf() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SETC_XF;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "SetcXf";
  static constexpr auto op_name = "setc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SfrAccShift final : public Instruction2Byte {
 public:
  SfrAccShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SFR_ACC_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "SfrAccShift";
  static constexpr auto op_name = "sfr";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SfrAccT final : public Instruction2Byte {
 public:
  SfrAccT() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SFR_ACC_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "SfrAccT";
  static constexpr auto op_name = "sfr";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SpmShift final : public Instruction2Byte {
 public:
  SpmShift() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SPM_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "SpmShift";
  static constexpr auto op_name = "spm";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper functions */
  static uint8_t GetPM(uint32_t data);
  static uint32_t SetPM(uint8_t pm);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SqraLoc16 final : public Instruction4Byte {
 public:
  SqraLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SQRA_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SqraLoc16";
  static constexpr auto op_name = "sqra";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SqrsLoc16 final : public Instruction4Byte {
 public:
  SqrsLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SQRS_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "SqrsLoc16";
  static constexpr auto op_name = "sqrs";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16Objmode1 final : public Instruction2Byte {
 public:
  SubAccLoc16Objmode1() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_OBJMODE1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubAccLoc16Objmode1";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16Objmode0 final : public Instruction2Byte {
 public:
  SubAccLoc16Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_OBJMODE0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubAccLoc16Objmode0";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16Shift1_15Objmode1 final : public Instruction4Byte {
 public:
  SubAccLoc16Shift1_15Objmode1() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_SHIFT1_15_OBJMODE1;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "SubAccLoc16Shift1_15Objmode1";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16Shift1_15Objmode0 final : public Instruction2Byte {
 public:
  SubAccLoc16Shift1_15Objmode0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_SHIFT1_15_OBJMODE0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F000;
  static constexpr auto full_name = "SubAccLoc16Shift1_15Objmode0";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_0;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16Shift16 final : public Instruction2Byte {
 public:
  SubAccLoc16Shift16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubAccLoc16Shift16";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccLoc16ShiftT final : public Instruction4Byte {
 public:
  SubAccLoc16ShiftT() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_LOC16_SHIFT_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubAccLoc16ShiftT";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAccConst16Shift final : public Instruction4Byte {
 public:
  SubAccConst16Shift() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_ACC_CONST16_SHIFT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "SubAccConst16Shift";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubAxLoc16 final : public Instruction2Byte {
 public:
  SubAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "SubAxLoc16";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubLoc16Ax final : public Instruction2Byte {
 public:
  SubLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUB_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "SubLoc16Ax";
  static constexpr auto op_name = "sub";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubbAccConst8 final : public Instruction2Byte {
 public:
  SubbAccConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBB_ACC_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubbAccConst8";
  static constexpr auto op_name = "subb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubbSpConst7 final : public Instruction2Byte {
 public:
  SubbSpConst7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBB_SP_CONST7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF80;
  static constexpr auto full_name = "SubbSpConst7";
  static constexpr auto op_name = "subb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetConst7(uint32_t data);
  static uint32_t SetConst7(uint8_t const7);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubbXarnConst7 final : public Instruction2Byte {
 public:
  SubbXarnConst7() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBB_XARN_CONST7;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F880;
  static constexpr auto full_name = "SubbXarnConst7";
  static constexpr auto op_name = "subb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint8_t GetConst7(uint32_t data);
  static uint32_t SetConst7(uint8_t const7);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubblAccLoc32 final : public Instruction4Byte {
 public:
  SubblAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBBL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubblAccLoc32";
  static constexpr auto op_name = "subbl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubcuAccLoc16 final : public Instruction2Byte {
 public:
  SubcuAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBCU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubcuAccLoc16";
  static constexpr auto op_name = "subcu";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubculAccLoc32 final : public Instruction4Byte {
 public:
  SubculAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBCUL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubculAccLoc32";
  static constexpr auto op_name = "subcul";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SublAccLoc32 final : public Instruction2Byte {
 public:
  SublAccLoc32() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SublAccLoc32";
  static constexpr auto op_name = "subl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SublLoc32Acc final : public Instruction4Byte {
 public:
  SublLoc32Acc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBL_LOC32_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SublLoc32Acc";
  static constexpr auto op_name = "subl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubrLoc16Ax final : public Instruction2Byte {
 public:
  SubrLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBR_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "SubrLoc16Ax";
  static constexpr auto op_name = "subr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubrlLoc32Acc final : public Instruction4Byte {
 public:
  SubrlLoc32Acc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBRL_LOC32_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubrlLoc32Acc";
  static constexpr auto op_name = "subrl";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubuAccLoc16 final : public Instruction2Byte {
 public:
  SubuAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBU_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "SubuAccLoc16";
  static constexpr auto op_name = "subu";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubulAccLoc32 final : public Instruction4Byte {
 public:
  SubulAccLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBUL_ACC_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubulAccLoc32";
  static constexpr auto op_name = "subul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class SubulPLoc32 final : public Instruction4Byte {
 public:
  SubulPLoc32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::SUBUL_P_LOC32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "SubulPLoc32";
  static constexpr auto op_name = "subul";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc32(uint32_t data);
  static uint32_t SetLoc32(uint8_t loc32);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TbitLoc16Bit final : public Instruction2Byte {
 public:
  TbitLoc16Bit() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TBIT_LOC16_BIT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_F000;
  static constexpr auto full_name = "TbitLoc16Bit";
  static constexpr auto op_name = "tbit";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetBitIndex(uint32_t data);
  static uint32_t SetBitIndex(uint8_t bit);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TbitLoc16T final : public Instruction4Byte {
 public:
  TbitLoc16T() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TBIT_LOC16_T;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "TbitLoc16T";
  static constexpr auto op_name = "tbit";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TclrLoc16Bit final : public Instruction4Byte {
 public:
  TclrLoc16Bit() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TCLR_LOC16_BIT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "TclrLoc16Bit";
  static constexpr auto op_name = "tclr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetBitIndex(uint32_t data);
  static uint32_t SetBitIndex(uint8_t bit);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TestAcc final : public Instruction2Byte {
 public:
  TestAcc() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TEST_ACC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "TestAcc";
  static constexpr auto op_name = "test";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TrapVec final : public Instruction2Byte {
 public:
  TrapVec() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TRAP_VEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFE0;
  static constexpr auto full_name = "TrapVec";
  static constexpr auto op_name = "trap";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetVec(uint32_t data);
  static uint32_t SetVec(uint8_t vec);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class TsetLoc16Bit final : public Instruction4Byte {
 public:
  TsetLoc16Bit() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::TSET_LOC16_BIT;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "TsetLoc16Bit";
  static constexpr auto op_name = "tset";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint8_t GetBitIndex(uint32_t data);
  static uint32_t SetBitIndex(uint8_t bit);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class UoutPaLoc16 final : public Instruction4Byte {
 public:
  UoutPaLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::UOUT_PA_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "UoutPaLoc16";
  static constexpr auto op_name = "uout";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbAl final : public Instruction2Byte {
 public:
  XbAl() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XB_AL;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "XbAl";
  static constexpr auto op_name = "xb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbPmaArpn final : public Instruction4Byte {
 public:
  XbPmaArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XB_PMA_APRN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbPmaArpn";
  static constexpr auto op_name = "xb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbPmaCond final : public Instruction4Byte {
 public:
  XbPmaCond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XB_PMA_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "XbPmaCond";
  static constexpr auto op_name = "xb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaInd final : public Instruction4Byte {
 public:
  XbanzPmaInd() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XbanzPmaInd";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaIndPostinc final : public Instruction4Byte {
 public:
  XbanzPmaIndPostinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XbanzPmaIndPostinc";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaIndPostdec final : public Instruction4Byte {
 public:
  XbanzPmaIndPostdec() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND_POSTDEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XbanzPmaIndPostdec";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaInd0Postinc final : public Instruction4Byte {
 public:
  XbanzPmaInd0Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND0_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XbanzPmaInd0Postinc";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaInd0Postdec final : public Instruction4Byte {
 public:
  XbanzPmaInd0Postdec() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND0_POSTDEC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XbanzPmaInd0Postdec";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaIndArpn final : public Instruction4Byte {
 public:
  XbanzPmaIndArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbanzPmaIndArpn";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaIndPostincArpn final : public Instruction4Byte {
 public:
  XbanzPmaIndPostincArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND_POSTINC_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbanzPmaIndPostincArpn";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaIndPostdecArpn final : public Instruction4Byte {
 public:
  XbanzPmaIndPostdecArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND_POSTDEC_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbanzPmaIndPostdecArpn";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaInd0PostincArpn final : public Instruction4Byte {
 public:
  XbanzPmaInd0PostincArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND0_POSTINC_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbanzPmaInd0PostincArpn";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XbanzPmaInd0PostdecArpn final : public Instruction4Byte {
 public:
  XbanzPmaInd0PostdecArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XBANZ_PMA_IND0_POSTDEC_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XbanzPmaInd0PostdecArpn";
  static constexpr auto op_name = "xbanz";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XcallAl final : public Instruction2Byte {
 public:
  XcallAl() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XCALL_AL;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "XcallAl";
  static constexpr auto op_name = "xcall";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XcallPmaArpn final : public Instruction4Byte {
 public:
  XcallPmaArpn() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XCALL_PMA_ARPN;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "XcallPmaArpn";
  static constexpr auto op_name = "xcall";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetRegN(uint32_t data);
  static uint32_t SetRegN(uint8_t n);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XcallPmaCond final : public Instruction4Byte {
 public:
  XcallPmaCond() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XCALL_PMA_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "XcallPmaCond";
  static constexpr auto op_name = "xcall";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);
  static uint64_t GetTarget(uint32_t data, uint64_t addr);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XmacPLoc16Pma final : public Instruction4Byte {
 public:
  XmacPLoc16Pma() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XMAC_P_LOC16_PMA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "XmacPLoc16Pma";
  static constexpr auto op_name = "xmac";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XmacdPLoc16Pma final : public Instruction4Byte {
 public:
  XmacdPLoc16Pma() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XMACD_P_LOC16_PMA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "XmacdPLoc16Pma";
  static constexpr auto op_name = "xmacd";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorAccLoc16 final : public Instruction2Byte {
 public:
  XorAccLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF00;
  static constexpr auto full_name = "XorAccLoc16";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorAccConst16Shift0_15 final : public Instruction4Byte {
 public:
  XorAccConst16Shift0_15() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_ACC_CONST16_SHIFT0_15;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "XorAccConst16Shift0_15";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetShift(uint32_t data);
  static uint32_t SetShift(uint8_t shift);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorAccConst16Shift16 final : public Instruction4Byte {
 public:
  XorAccConst16Shift16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_ACC_CONST16_SHIFT16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF0000;
  static constexpr auto full_name = "XorAccConst16Shift16";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorAxLoc16 final : public Instruction2Byte {
 public:
  XorAxLoc16() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_AX_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "XorAxLoc16";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorLoc16Ax final : public Instruction2Byte {
 public:
  XorLoc16Ax() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_LOC16_AX;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "XorLoc16Ax";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorLoc16Const16 final : public Instruction4Byte {
 public:
  XorLoc16Const16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XOR_LOC16_CONST16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "XorLoc16Const16";
  static constexpr auto op_name = "xor";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XorbAxConst8 final : public Instruction2Byte {
 public:
  XorbAxConst8() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XORB_AX_CONST8;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FE00;
  static constexpr auto full_name = "XorbAxConst8";
  static constexpr auto op_name = "xorb";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_ANY;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegAx(uint32_t data);
  static uint32_t SetRegAx(uint8_t x);
  static uint8_t GetConst8(uint32_t data);
  static uint32_t SetConst8(uint8_t const8);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XpreadLoc16Pma final : public Instruction4Byte {
 public:
  XpreadLoc16Pma() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XPREAD_LOC16_PMA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FF000000;
  static constexpr auto full_name = "XpreadLoc16Pma";
  static constexpr auto op_name = "xpread";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);
  static uint16_t GetConst16(uint32_t data);
  static uint32_t SetConst16(uint16_t const16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XpreadLoc16Al final : public Instruction4Byte {
 public:
  XpreadLoc16Al() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XPREAD_LOC16_AL;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "XpreadLoc16Al";
  static constexpr auto op_name = "xpread";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XpwriteAlLoc16 final : public Instruction4Byte {
 public:
  XpwriteAlLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XPWRITE_AL_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "XpwriteAlLoc16";
  static constexpr auto op_name = "xpwrite";
  static constexpr bool repeatable = true;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class XretcCond final : public Instruction2Byte {
 public:
  XretcCond() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::XRETC_COND;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF0;
  static constexpr auto full_name = "XretcCond";
  static constexpr auto op_name = "xretc";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetCond(uint32_t data);
  static uint32_t SetCond(uint8_t cond);

  /* Binary Ninja Function Implementations */
  bool Info(const uint8_t* data, uint64_t addr, BN::InstructionInfo& result,
            TIC28XArchitecture* arch) override;

  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class ZalrAccLoc16 final : public Instruction4Byte {
 public:
  ZalrAccLoc16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ZALR_ACC_LOC16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "ZalrAccLoc16";
  static constexpr auto op_name = "zalr";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetLoc16(uint32_t data);
  static uint32_t SetLoc16(uint8_t loc16);

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Zapa final : public Instruction2Byte {
 public:
  Zapa() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ZAPA;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFF;
  static constexpr auto full_name = "Zapa";
  static constexpr auto op_name = "zapa";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Binary Ninja Function Implementations */
  bool Text(const uint8_t* data, uint64_t addr, size_t& len,
            std::vector<BN::InstructionTextToken>& result,
            AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

/**
 * C28x Extended Instructions
 *
 * OBJMODE = 1, non-repeatable for all instructions
 *
 * The extended instruction set consists of instructions for 7 different
 * processor accelerators:
 * - FPU32 (32-bit floating point unit)
 * - FPU64 (64-bit floating point unit)
 * - VCU (Viterbi, complex math & CRC unit)
 * - VCRC (Cyclic redundancy check unit)
 * - VCU-II (C28 Viterbi, complex cath & CRC unit II)
 * - FINTDIV (Fast integer division unit)
 * - TMU (Trigonometric math unit)
 */

/* FPU Instructions */

class Absf32RahRbh final : public Instruction4Byte {
 public:
  Absf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ABSF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Absf32RahRbh";
  static constexpr auto op_name = "absf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Addf32Rah16fhiRbh final : public Instruction4Byte {
 public:
  Addf32Rah16fhiRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDF32_RAH_16FHI_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFC00000;
  static constexpr auto full_name = "Addf32Rah16fhiRbh";
  static constexpr auto op_name = "addf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);
  static uint16_t Get16fhi(uint32_t data);
  static uint32_t Set16fhi(uint16_t i);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Addf32RahRbhRch final : public Instruction4Byte {
 public:
  Addf32RahRbhRch() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::ADDF32_RAH_RBH_RCH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFE00;
  static constexpr auto full_name = "Addf32RahRbhRch";
  static constexpr auto op_name = "addf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);
  static uint8_t GetRegC(uint32_t data);
  static uint32_t SetRegC(uint8_t c);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Addf32RdhRehRfhMov32Mem32Rah final : public Instruction4Byte {
 public:
  Addf32RdhRehRfhMov32Mem32Rah() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode =
      Opcodes::ADDF32_RDH_REH_RFH_MOV32_MEM32_RAH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "Addf32RdhRehRfhMov32Mem32Rah";
  static constexpr auto op_name = "addf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Addf32RdhRehRfhMov32RahMem32 final : public Instruction4Byte {
 public:
  Addf32RdhRehRfhMov32RahMem32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode =
      Opcodes::ADDF32_RDH_REH_RFH_MOV32_RAH_MEM32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "Addf32RdhRehRfhMov32RahMem32";
  static constexpr auto op_name = "addf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpf32RahRbh final : public Instruction4Byte {
 public:
  Cmpf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Cmpf32RahRbh";
  static constexpr auto op_name = "cmpf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpf32Rah16fhi final : public Instruction4Byte {
 public:
  Cmpf32Rah16fhi() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPF32_RAH_16FHI;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "Cmpf32Rah16fhi";
  static constexpr auto op_name = "cmpf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint16_t Get16fhi(uint32_t data);
  static uint32_t Set16fhi(uint16_t i);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Cmpf32Rah0 final : public Instruction2Byte {
 public:
  Cmpf32Rah0() : Instruction2Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::CMPF32_RAH_0;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF8;
  static constexpr auto full_name = "Cmpf32Rah0";
  static constexpr auto op_name = "cmpf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Einvf32RahRbh final : public Instruction4Byte {
 public:
  Einvf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::EINVF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Einvf32RahRbh";
  static constexpr auto op_name = "einvf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Eisqrtf32RahRbh final : public Instruction4Byte {
 public:
  Eisqrtf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::EISQRTF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Eisqrtf32RahRbh";
  static constexpr auto op_name = "eisqrtf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toi16RahRbh final : public Instruction4Byte {
 public:
  F32toi16RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOI16_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toi16RahRbh";
  static constexpr auto op_name = "f32toi16";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toi16rRahRbh final : public Instruction4Byte {
 public:
  F32toi16rRahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOI16R_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toi16rRahRbh";
  static constexpr auto op_name = "f32toi16r";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toi32RahRbh final : public Instruction4Byte {
 public:
  F32toi32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOI32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toi32RahRbh";
  static constexpr auto op_name = "f32toi32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toui16RahRbh final : public Instruction4Byte {
 public:
  F32toui16RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOUI16_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toui16RahRbh";
  static constexpr auto op_name = "f32toui16";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toui16rRahRbh final : public Instruction4Byte {
 public:
  F32toui16rRahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOUI16R_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toui16rRahRbh";
  static constexpr auto op_name = "f32toui16r";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class F32toui32RahRbh final : public Instruction4Byte {
 public:
  F32toui32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::F32TOUI32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "F32toui32RahRbh";
  static constexpr auto op_name = "f32toui32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Fracf32RahRbh final : public Instruction4Byte {
 public:
  Fracf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::FRACF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Fracf32RahRbh";
  static constexpr auto op_name = "fracf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class I16tof32RahRbh final : public Instruction4Byte {
 public:
  I16tof32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::I16TOF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "I16tof32RahRbh";
  static constexpr auto op_name = "i16tof32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class I16tof32RahMem16 final : public Instruction4Byte {
 public:
  I16tof32RahMem16() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::I16TOF32_RAH_MEM16;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF800;
  static constexpr auto full_name = "I16tof32RahMem16";
  static constexpr auto op_name = "i16tof32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint16_t GetMem16(uint32_t data);
  static uint32_t SetMem16(uint16_t mem16);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class I32tof32RahMem32 final : public Instruction4Byte {
 public:
  I32tof32RahMem32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::I32TOF32_RAH_MEM32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF800;
  static constexpr auto full_name = "I32tof32RahMem32";
  static constexpr auto op_name = "i32tof32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class I32tof32RahRbh final : public Instruction4Byte {
 public:
  I32tof32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::I32TOF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "I32tof32RahRbh";
  static constexpr auto op_name = "i32tof32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Macf32R3hR2hRdhRehRfh final : public Instruction4Byte {
 public:
  Macf32R3hR2hRdhRehRfh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MACF32_R3H_R2H_RDH_REH_RFH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFC0000;
  static constexpr auto full_name = "Macf32R3hR2hRdhRehRfh";
  static constexpr auto op_name = "macf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Macf32R3hR2hRdhRehRfhMov32RahMem32 final : public Instruction4Byte {
 public:
  Macf32R3hR2hRdhRehRfhMov32RahMem32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode =
      Opcodes::MACF32_R3H_R2H_RDH_REH_RFH_MOV32_RAH_MEM32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "Macf32R3hR2hRdhRehRfhMov32RahMem32";
  static constexpr auto op_name = "macf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Macf32R7hR3hMem32Xar7Postinc final : public Instruction4Byte {
 public:
  Macf32R7hR3hMem32Xar7Postinc() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MACF32_R7H_R3H_MEM32_XAR7_POSTINC;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFF00;
  static constexpr auto full_name = "Macf32R7hR3hMem32Xar7Postinc";
  static constexpr auto op_name = "macf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Macf32R7hR6hRdhRehRfh final : public Instruction4Byte {
 public:
  Macf32R7hR6hRdhRehRfh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MACF32_R7H_R6H_RDH_REH_RFH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFC0000;
  static constexpr auto full_name = "Macf32R7hR6hRdhRehRfh";
  static constexpr auto op_name = "macf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Macf32R7hR6hRdhRehRfhMov32RahMem32 final : public Instruction4Byte {
 public:
  Macf32R7hR6hRdhRehRfhMov32RahMem32() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode =
      Opcodes::MACF32_R7H_R6H_RDH_REH_RFH_MOV32_RAH_MEM32;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF00000;
  static constexpr auto full_name = "Macf32R7hR6hRdhRehRfhMov32RahMem32";
  static constexpr auto op_name = "macf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);
  static uint8_t GetRegE(uint32_t data);
  static uint32_t SetRegE(uint8_t e);
  static uint8_t GetRegF(uint32_t data);
  static uint32_t SetRegF(uint8_t f);
  static uint16_t GetMem32(uint32_t data);
  static uint32_t SetMem32(uint16_t mem32);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Maxf32RahRbh final : public Instruction4Byte {
 public:
  Maxf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAXF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Maxf32RahRbh";
  static constexpr auto op_name = "maxf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Maxf32Rah16fhi final : public Instruction4Byte {
 public:
  Maxf32Rah16fhi() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAXF32_RAH_16FHI;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "Maxf32Rah16fhi";
  static constexpr auto op_name = "maxf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint16_t Get16fhi(uint32_t data);
  static uint32_t Set16fhi(uint16_t i);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Maxf32RahRbhMov32RchRdh final : public Instruction4Byte {
 public:
  Maxf32RahRbhMov32RchRdh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MAXF32_RAH_RBH_MOV32_RCH_RDH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "Maxf32RahRbhMov32RchRdh";
  static constexpr auto op_name = "maxf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);
  static uint8_t GetRegC(uint32_t data);
  static uint32_t SetRegC(uint8_t c);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Minf32RahRbh final : public Instruction4Byte {
 public:
  Minf32RahRbh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MINF32_RAH_RBH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "Minf32RahRbh";
  static constexpr auto op_name = "minf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Minf32Rah16fhi final : public Instruction4Byte {
 public:
  Minf32Rah16fhi() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MINF32_RAH_16FHI;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFF80000;
  static constexpr auto full_name = "Minf32Rah16fhi";
  static constexpr auto op_name = "minf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint16_t Get16fhi(uint32_t data);
  static uint32_t Set16fhi(uint16_t i);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

class Minf32RahRbhMov32RchRdh final : public Instruction4Byte {
 public:
  Minf32RahRbhMov32RchRdh() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::MINF32_RAH_RBH_MOV32_RCH_RDH;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFF000;
  static constexpr auto full_name = "Minf32RahRbhMov32RchRdh";
  static constexpr auto op_name = "minf32";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);
  static uint8_t GetRegC(uint32_t data);
  static uint32_t SetRegC(uint8_t c);
  static uint8_t GetRegD(uint32_t data);
  static uint32_t SetRegD(uint8_t d);

  /* Binary Ninja Function Implementations */
  // bool Text(const uint8_t* data, uint64_t addr, size_t& len,
  //           std::vector<BN::InstructionTextToken>& result,
  //           AddressMode amode) override;

  // bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
  //           BN::LowLevelILFunction& il, TIC28XArchitecture* arch) override;
  //
};

}  // namespace TIC28X

#endif  // TIC28X_INSTRUCTIONS_H
