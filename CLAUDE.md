# TI C28x Architecture Plugin - Developer Documentation

## Overview

This is a Binary Ninja architecture plugin for the Texas Instruments C28x family of DSP microcontrollers. The plugin provides disassembly support for the standard TI C28x instruction set with compatibility mode support for C27x and C2xLP architectures.

**Status**: Active development. Currently supports disassembly only; LLIL lifting is planned but not yet implemented.

**Authors**: Garrett Larsen, Matthew Crepeau (Idaho National Laboratory / Battelle Energy Alliance, LLC)

## Architecture Background

### TI C28x Processor

The TI C28x is a 32-bit fixed-point DSP architecture used in embedded control applications, particularly in motor control, power electronics, and real-time control systems. Key characteristics:

- **Address Space**: 32-bit addressing (22-bit effective address space)
- **Endianness**: Little-endian
- **Instruction Length**: Variable (16-bit and 32-bit instructions)
- **Instruction Alignment**: 16-bit (2-byte) boundaries
- **Default Integer Size**: 16-bit

### Compatibility Modes

The C28x processor supports three compatibility modes controlled by the `OBJMODE` and `AMODE` status bits:

1. **C28x Mode** (default): `OBJMODE=1, AMODE=0`
   - Full C28x instruction set and addressing

2. **C2xLP Mode**: `OBJMODE=1, AMODE=1`
   - Source-compatible with C2xLP architecture

3. **C27x Mode**: `OBJMODE=0, AMODE=0`
   - Object-compatible with C27x architecture (default at RESET)

## Project Structure

```
bn-tic28x-arch/
├── src/
│   ├── architecture.cpp/h    # Main architecture class implementation
│   ├── info.cpp/h            # Instruction info methods (branch targets, length)
│   ├── instructions.cpp/h    # Instruction class definitions
│   ├── lift.cpp/h            # LLIL lifting (stub/planned)
│   ├── text.cpp/h            # Text generation (disassembly)
│   ├── opcodes.h             # Opcode constants
│   ├── registers.h           # Register definitions and metadata
│   ├── flags.h               # Status flag definitions
│   ├── conditions.h          # Conditional codes
│   ├── sizes.h               # Size constants
│   ├── util.cpp/h            # Utility functions
│   ├── *_test.cpp/h          # Unit tests
├── CMakeLists.txt            # Build configuration
├── README.md                 # User documentation
├── NOTICE.txt                # Copyright and licensing
└── LICENSE                   # MIT license
```

## Core Components

### 1. Architecture Class (`architecture.h/cpp`)

**`TIC28XArchitecture`** extends `BinaryNinja::Architecture` and provides the main interface to Binary Ninja's analysis engine.

Key responsibilities:
- Register and flag definitions
- Instruction decoding dispatch
- Text generation dispatch
- LLIL generation dispatch (future)
- Mode tracking (OBJMODE, AMODE)

Important members:
```cpp
std::vector<std::optional<std::shared_ptr<Instruction>>> INSTRUCTIONS;
ObjectMode OBJMODE;
AddressMode AMODE;
std::unordered_set<uint64_t> REPEAT_ADDRS;  // Tracks RPT instructions
```

### 2. Instruction System (`instructions.h/cpp`)

**Abstract Base Classes**:
- `Instruction` - Pure virtual base for all instructions
- `Instruction2Byte` - 16-bit instructions
- `Instruction4Byte` - 32-bit instructions

**Concrete Instruction Classes**: Each CPU instruction has its own class (e.g., `Aborti`, `Add`, `Mul`, etc.) that implements:
- `GetOpcode()` - Returns the opcode constant
- `GetOpcodeMask()` - Returns the bit mask for matching
- `GetLength()` - Returns instruction length (2 or 4 bytes)
- `Info()` - Provides branch/flow information
- `Text()` - Generates disassembly text
- `Lift()` - LLIL generation (planned)

**Instruction Matching**: Instructions are stored in a vector and matched by:
1. Masking the raw bytes with the instruction's opcode mask
2. Comparing against the instruction's opcode
3. Checking compatibility mode requirements

### 3. Registers (`registers.h`)

**Register Categories**:

1. **General Purpose**:
   - `ACC` (32-bit accumulator) with sub-registers `AH` (high 16), `AL` (low 16)
   - `XAR0-XAR7` (32-bit auxiliary registers)
   - `AR0-AR7` (low 16 bits of XARn)
   - `AR0H-AR7H` (high 16 bits of XARn)

2. **System Registers**:
   - `PC` - Program counter (22-bit, stored as 32-bit)
   - `SP` - Stack pointer (16-bit)
   - `ST0`, `ST1` - Status registers
   - `DP` - Data page pointer
   - `P` (32-bit product) with `PH`, `PL` sub-registers
   - `XT` (32-bit temp) with `T`/`TH`, `TL` sub-registers

3. **FPU Registers** (if present):
   - `R0H-R7H`, `R0L-R7L` (floating-point registers)
   - `STF` - FPU status
   - `RB` - FPU repeat block

**Register Info**: The `INFO` map provides metadata for each register:
- Full-width register reference
- Offset within full-width register
- Size in bytes
- Zero-extend behavior

### 4. Flags (`flags.h`)

**ST0 Flags** (Status Register 0):
- `SXM` - Sign extension mode
- `OVM` - Overflow mode
- `TC` - Test/control flag
- `C` - Carry flag
- `Z` - Zero flag
- `N` - Negative flag
- `V` - Overflow flag
- `PM` - Product shift mode
- `OVC` - Overflow counter

**ST1 Flags** (Status Register 1):
- `INTM` - Interrupt mask
- `DBGM` - Debug enable mask
- `AMODE` - Address mode bit
- `OBJMODE` - Object mode bit
- `EALLOW` - Emulation access enable
- `LOOP` - Loop flag
- `ARP` - Auxiliary register pointer
- Others for memory mapping and control

### 5. Text Generation (`text.h/cpp`)

The text generation system converts raw instruction bytes into human-readable disassembly. It handles:

**Operand Types**:
- **Registers**: Direct, indirect, with pre/post increment/decrement
- **Constants**: Immediate values (8, 16, 22-bit), addresses, offsets
- **Location Codes**: 8-bit encoding specifying addressing modes
- **Conditions**: Conditional codes (EQ, NEQ, GT, LT, etc.)

**Helper Functions**:
- `RegText()` - Format register operands with decorators (@, *, --, ++, %)
- `ConstText()` - Format constants with # or @ prefix
- `Loc16Text()` / `Loc32Text()` - Decode 16/32-bit location codes
- `CondText()` - Format conditional suffixes

**Example Disassembly**:
```
add    acc, #1234h << 8
mov    *xar2++, al
b      label, eq
```

### 6. Info Methods (`info.cpp`)

Info methods provide control flow information to Binary Ninja's analysis engine:
- Instruction length
- Branch targets (for jumps, calls)
- Branch types (UnconditionalBranch, TrueBranch, FalseBranch, CallDestination)

**Branch Handling Example**:
```cpp
bool BOff16Cond::Info(...) {
    result.length = GetLength();
    result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
    result.AddBranch(FalseBranch, addr + result.length);
    return true;
}
```

### 7. Opcodes (`opcodes.h`)

Contains constants for all instruction opcodes and their corresponding bit masks:

```cpp
constexpr uint32_t ABORTI = 0x1;
constexpr uint32_t ADD_ACC_CONST16_SHIFT = 0xFF100000;
constexpr uint32_t MOV_LOC16_AX = 0x8E00;
// ... hundreds more
```

**Opcode Masks**:
- `MASK_FFFF` - 16-bit exact match
- `MASK_FFFF_0000` - 32-bit instruction, high 16 bits
- `MASK_FF00`, `MASK_FE00`, etc. - Various partial matches

### 8. Utilities (`util.h/cpp`)

Helper functions:
- `DataToOpcode()` - Converts raw byte array to opcode value
- `CreateLowerBitMask()` - Generates bit masks for field extraction

## Key Design Patterns

### 1. Instruction Factory Pattern

Instructions are generated once during architecture initialization:
```cpp
TIC28XArchitecture::TIC28XArchitecture(const std::string& name) {
    OBJMODE = OBJMODE_0;
    AMODE = AMODE_0;
    INSTRUCTIONS = GenerateInstructionVector();
}
```

The vector contains shared pointers to instruction objects, indexed for fast lookup.

### 2. Virtual Dispatch for Polymorphism

Each instruction class overrides virtual methods from the base `Instruction` class:
- This allows the architecture to work with any instruction generically
- Enables instruction-specific behavior without large switch statements

### 3. Bit Field Extraction

Instruction operands are extracted using bit manipulation:
```cpp
uint8_t FpuGetRegA_I(uint32_t data) {
    return (data >> 16) & 0x7;  // Extract 3-bit field at bit 16
}
```

### 4. Thread-Safe Repeat Tracking

RPT (repeat) instructions are tracked with a mutex-protected set:
```cpp
void TIC28XArchitecture::AddRepeatAddr(uint64_t addr) {
    std::lock_guard lock(REPEAT_ADDR_MUTEX);
    REPEAT_ADDRS.insert(addr);
}
```

## Current Limitations

1. **No LLIL Lifting**: The plugin does not generate Low Level IL, so:
   - No decompilation to High Level IL
   - Limited data flow analysis
   - No type inference
   - No cross-reference generation based on semantics

2. **Standard Instruction Set Only**: Extended instruction set not supported

3. **FPU Instructions**: Floating-point instructions may not be fully tested

4. **Incomplete Flag Semantics**: Flag role and write type methods are stubbed

## Future Work

### High Priority
1. **LLIL Implementation**: Implement `Lift()` methods for all instructions
   - Start with common instructions (MOV, ADD, etc.)
   - Implement control flow (branches, calls, returns)
   - Add arithmetic and logic operations
   - Handle addressing modes properly

2. **Extended Instruction Set**: Add support for extended C28x instructions

### Medium Priority
3. **Flag Semantics**: Complete flag role and write type implementations
4. **FPU Support**: Verify and test floating-point instruction handling
5. **Test Coverage**: Expand unit tests to cover more instructions

### Low Priority
6. **Platform Detection**: Add binary view plugin to detect C28x binaries
7. **Symbol Import**: Support for common C28x binary formats (COFF, etc.)

## Development Tips

### Adding a New Instruction

#### Step 1: Calculate the Opcode from Documentation

TI C28x instructions use LSW/MSW (Least/Most Significant Word) encoding in documentation. For 32-bit instructions:
- **LSW** goes in the upper 16 bits of the opcode
- **MSW** goes in the lower 16 bits of the opcode

**Example** - Converting `I64TOF64 Ra,Rb` from documentation:
```
LSW: 1110 0110 1000 0101 = 0xE685
MSW: 1000 0000 00bb baaa = 0x80xx (base 0x8000, with register fields)
Combined: 0xE6858000
```

For 16-bit instructions, the opcode is simply the 16-bit value.

#### Step 2: Add Opcode Constant in `opcodes.h`

```cpp
// In namespace TIC28X::Opcodes
constexpr uint32_t I64TOF64_RA_RB = 0xE6858000;
```

Use descriptive names matching the instruction syntax. For FPU64 instructions, use uppercase with underscores.

#### Step 3: Choose the Appropriate Mask

Masks define which bits are fixed vs. variable (operand fields). Common masks in `OpcodeMasks`:

| Mask | Use Case |
|------|----------|
| `MASK_FFFF` | 2-byte exact match |
| `MASK_FFFFFFC0` | 4-byte, bits 5-0 variable (Format I: Ra, Rb) |
| `MASK_FFFFFE00` | 4-byte, bits 8-0 variable (Format I: Ra, Rb, Rc) |
| `MASK_FFFFF800` | 4-byte, bits 10-0 variable (Format II: Ra + mem) |
| `MASK_FFF80000` | 4-byte, bits 18-0 variable (Format III: Ra + 16-bit imm) |
| `MASK_FFFFF000` | 4-byte, bits 11-0 variable (Format I + IV: Ra, Rb + Rc, Rd) |
| `MASK_FFC00000` | 4-byte, bits 21-0 variable (Format III: Ra, Rb + 16-bit imm) |

#### Step 4: Define Instruction Class in `instructions.h`

```cpp
class I64tof64RaRb final : public Instruction4Byte {
 public:
  I64tof64RaRb() : Instruction4Byte() {}

  /* Instruction Data */
  static constexpr uint32_t opcode = Opcodes::I64TOF64_RA_RB;
  static constexpr uint32_t opcode_mask = OpcodeMasks::MASK_FFFFFFC0;
  static constexpr auto full_name = "I64tof64RaRb";
  static constexpr auto op_name = "i64tof64";
  static constexpr bool repeatable = false;
  static constexpr ObjectMode objmode = OBJMODE_1;  // FPU requires OBJMODE_1

  /* Overrides for abstract instruction getters */
  uint32_t GetOpcode() override { return opcode; }
  uint32_t GetOpcodeMask() override { return opcode_mask; }
  const char* GetFullName() override { return full_name; }
  const char* GetOpName() override { return op_name; }
  bool IsRepeatable() override { return repeatable; }
  ObjectMode GetObjmode() override { return objmode; }

  /* Helper Functions - declare static Get/Set for each operand */
  static uint8_t GetRegA(uint32_t data);
  static uint32_t SetRegA(uint8_t a);
  static uint8_t GetRegB(uint32_t data);
  static uint32_t SetRegB(uint8_t b);

  /* Binary Ninja Function Implementations */
  // bool Text(...) override;  // Uncomment when implementing disassembly
  // bool Lift(...) override;  // Uncomment when implementing lifting
};
```

**Naming Conventions**:
- `full_name`: PascalCase matching class name (e.g., `"I64tof64RaRb"`)
- `op_name`: lowercase mnemonic (e.g., `"i64tof64"`)

**ObjectMode Values**:
- `OBJMODE_ANY` - Works in any mode
- `OBJMODE_0` - C27x compatibility mode only
- `OBJMODE_1` - C28x mode only (required for FPU/extended instructions)

#### Step 5: Add to Instruction Vector in `instructions.cpp`

Add to `GenerateInstructionVector()`:
```cpp
std::make_shared<I64tof64RaRb>(),
```

#### Step 6: Implement Helper Functions in `instructions.cpp`

Use existing FPU helper functions for common formats:

**Format I** (Ra bits 2-0, Rb bits 5-3, Rc bits 8-6):
```cpp
// I64tof64RaRb - Format I for Ra, Rb
uint8_t I64tof64RaRb::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t I64tof64RaRb::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t I64tof64RaRb::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t I64tof64RaRb::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}
```

**Format II** (Ra bits 10-8, mem bits 7-0):
```cpp
uint8_t MyInst::GetRegA(const uint32_t data) { return FpuGetRegA_II(data); }
uint8_t MyInst::GetMem(const uint32_t data) { return FpuGetMem(data); }
```

**Format III** (Ra bits 2-0, 16-bit imm bits 18-3):
```cpp
uint8_t MyInst::GetRegA(const uint32_t data) { return FpuGetRegA_III(data); }
uint16_t MyInst::Get16F(const uint32_t data) { return FpuGet16FHi_III(data); }
```

**Format IV** (Rd bits 11-9, Re bits 14-12, Rf bits 17-15):
```cpp
uint8_t MyInst::GetRegD(const uint32_t data) { return FpuGetRegD_IV(data); }
uint8_t MyInst::GetRegE(const uint32_t data) { return FpuGetRegE_IV(data); }
uint8_t MyInst::GetRegF(const uint32_t data) { return FpuGetRegF_IV(data); }
```

#### Step 7: Add Test in `instructions_test.cpp`

Add to the test vector in `GenerateTestOpcodes()`:
```cpp
{TIC28X::I64tof64RaRb::SetRegA(TEST_DATA) |
     TIC28X::I64tof64RaRb::SetRegB(TEST_DATA),
 TIC28X::I64tof64RaRb::full_name, TIC28X::I64tof64RaRb::objmode},
```

The test uses `Set*` helpers to construct a valid opcode with test data in all operand fields, then verifies the decoder returns the correct instruction.

#### Step 8 (Optional): Implement Text() in `text.cpp`

```cpp
bool I64tof64RaRb::Text(const uint8_t* data, uint64_t addr, size_t& len,
                        std::vector<BN::InstructionTextToken>& result,
                        AddressMode amode) {
  const uint32_t dataOp = DataToOpcode(data, GetLength());
  len = GetLength();
  OpText(op_name, result);
  FpuRegText64(GetRegA(dataOp), result);  // Ra (64-bit FPU register)
  CommaText(result);
  FpuRegText64(GetRegB(dataOp), result);  // Rb (64-bit FPU register)
  return true;
}
```

#### Step 9 (Optional): Implement Info() in `info.cpp`

Only needed for branch/call instructions:
```cpp
bool MyBranchInst::Info(const uint8_t* data, uint64_t addr,
                        BN::InstructionInfo& result,
                        TIC28XArchitecture* arch) {
  result.length = GetLength();
  result.AddBranch(BN::UnconditionalBranch, GetTarget(data, addr));
  return true;
}
```

### FPU Register Field Summary

| Format | Field | Bits | Helper Functions |
|--------|-------|------|------------------|
| I | RegA | 2-0 | `FpuGetRegA_I` / `FpuSetRegA_I` |
| I | RegB | 5-3 | `FpuGetRegB_I` / `FpuSetRegB_I` |
| I | RegC | 8-6 | `FpuGetRegC_I` / `FpuSetRegC_I` |
| II | RegA | 10-8 | `FpuGetRegA_II` / `FpuSetRegA_II` |
| II | mem | 7-0 | `FpuGetMem` / `FpuSetMem` |
| III | RegA | 2-0 | `FpuGetRegA_III` / `FpuSetRegA_III` |
| III | 16F | 18-3 | `FpuGet16FHi_III` / `FpuSet16FHi_III` |
| IV | RegD | 11-9 | `FpuGetRegD_IV` / `FpuSetRegD_IV` |
| IV | RegE | 14-12 | `FpuGetRegE_IV` / `FpuSetRegE_IV` |
| IV | RegF | 17-15 | `FpuGetRegF_IV` / `FpuSetRegF_IV` |

### Testing

Run the test suite:
```bash
cmake -DCMAKE_BUILD_TYPE=debug -DHEADLESS=yes .
cmake --build . --target tic28x_architecture_test
./tic28x_architecture_test
```

### Debugging

1. **Enable debug build**: Use `-DCMAKE_BUILD_TYPE=debug`
2. **Add logging**: Use `BN::LogWarn()`, `BN::LogDebug()` from Binary Ninja API
3. **Attach debugger**: GDB/LLDB to Binary Ninja process with plugin loaded

## References

### Official TI Documentation
- [TIC28x CPU and Instruction Set Reference](https://www.ti.com/lit/ug/spru430f/spru430f.pdf)
- [TIC28x Extended Instruction Set Reference](https://www.ti.com/lit/ug/spruhs1c/spruhs1c.pdf)

### Binary Ninja Resources
- [Binary Ninja C++ API Documentation](https://api.binary.ninja/cpp/index.html)
- [Architecture Plugin Development Guide](https://docs.binary.ninja/dev/binaryview.html)

## License

Licensed under the MIT License. Copyright 2025 Battelle Energy Alliance, LLC.

Developed at Idaho National Laboratory under contract with the U.S. Department of Energy.

See `LICENSE` and `NOTICE.txt` for full details.