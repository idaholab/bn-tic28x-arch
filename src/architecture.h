// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <binaryninjaapi.h>

namespace BN = BinaryNinja;

namespace TIC28X {
class TIC28XArchitecture;
/**
 * Compatibility Modes
 * C28x Mode: OBJMODE = 1; AMODE = 0;
 * C2xLP Source-compatible Mode: OBJMODE = 1; AMODE = 1;
 * C27x Object-compatible Mode: OBJMODE = 0; AMODE = 0; (default mode @ RESET)
 */
enum ObjectMode { OBJMODE_0 = 0, OBJMODE_1 = 1, OBJMODE_ANY = 2 };
enum AddressMode { AMODE_0 = 0, AMODE_1 = 1 };

/**
 * Abstract instruction class that all instructions must use.
 * This allows the return type of the decoder function (DecodeInstruction) to
 * be generic.
 */
class Instruction {
 public:
  explicit Instruction() = default;
  virtual ~Instruction() = default;

  /* Instruction Data Getters */
  virtual uint32_t GetOpcode() = 0;
  virtual uint32_t GetOpcodeMask() = 0;
  virtual const char* GetFullName() = 0;  // for testing purposes
  virtual const char* GetOpName() = 0;    // for text generation
  virtual size_t GetLength() = 0;
  virtual bool IsRepeatable() = 0;
  virtual ObjectMode GetObjmode() = 0;

  /* Binary Ninja Functions */
  // TODO: make pure virtual
  virtual bool Text(const uint8_t* data, uint64_t addr, size_t& len,
                    std::vector<BN::InstructionTextToken>& result,
                    AddressMode amode);

  virtual bool Info(const uint8_t* data, uint64_t addr,
                    BN::InstructionInfo& result, TIC28XArchitecture* arch) = 0;

  // TODO: make pure virtual
  virtual bool Lift(const uint8_t* data, uint64_t addr, size_t& len,
                    BN::LowLevelILFunction& il, TIC28XArchitecture* arch);
};

class TIC28XArchitecture final : public BN::Architecture {
 protected:
  static BNRegisterInfo RegisterInfo(uint32_t fullWidthReg, size_t offset,
                                     size_t size, bool zeroExtend = false);
  // INSTRUCTIONS is a vector of pointers to all possible CPU instructions.
  std::vector<std::optional<std::shared_ptr<Instruction>>> INSTRUCTIONS;

  // Object and address modes are set based on the desired compatibility mode
  ObjectMode OBJMODE;
  AddressMode AMODE;

  // Track instruction repeats (RPT inst)
  // TODO: do these need to be tracked in the view?
  // Will opening multiple samples in the same binja window share this set?
  std::unordered_set<uint64_t> REPEAT_ADDRS;
  std::mutex REPEAT_ADDR_MUTEX;

 public:
  void SetObjmode(ObjectMode mode);
  void SetAmode(AddressMode mode);
  void AddRepeatAddr(uint64_t addr);
  bool IsRepeatAddr(uint64_t addr);

  explicit TIC28XArchitecture(const std::string& name);
  ~TIC28XArchitecture() override;

  size_t GetAddressSize() const override;
  size_t GetDefaultIntegerSize() const override;
  BNEndianness GetEndianness() const override;
  uint32_t GetStackPointerRegister() override;
  size_t GetMaxInstructionLength() const override;
  size_t GetInstructionAlignment() const override;
  std::vector<uint32_t> GetAllRegisters() override;
  BNRegisterInfo GetRegisterInfo(uint32_t reg) override;
  std::string GetRegisterName(uint32_t reg) override;
  std::vector<uint32_t> GetAllFlags() override;
  std::string GetFlagName(uint32_t flag) override;
  BNFlagRole GetFlagRole(uint32_t flag, uint32_t semClass) override;
  std::vector<uint32_t> GetFlagsWrittenByFlagWriteType(uint32_t flags) override;
  std::string GetFlagWriteTypeName(uint32_t flags) override;
  std::vector<uint32_t> GetFlagsRequiredForFlagCondition(
      BNLowLevelILFlagCondition cond, uint32_t semClass) override;

  bool GetInstructionInfo(const uint8_t* data, uint64_t addr, size_t maxLen,
                          BN::InstructionInfo& result) override;
  bool GetInstructionText(
      const uint8_t* data, uint64_t addr, size_t& len,
      std::vector<BN::InstructionTextToken>& result) override;
  bool GetInstructionLowLevelIL(const uint8_t* data, uint64_t addr, size_t& len,
                                BN::LowLevelILFunction& il) override;
};
}  // namespace TIC28X

#endif  // ARCHITECTURE_H
