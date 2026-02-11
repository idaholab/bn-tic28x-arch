# TI C28x Architecture Plugin - Developer Documentation

## Overview

This is a Binary Ninja architecture plugin for the Texas Instruments C28x family of DSP microcontrollers. The plugin provides disassembly support for the standard TI C28x instruction set with compatibility mode support for C27x and C2xLP architectures.

**Status**: Active development. Supports disassembly and partial LLIL lifting for VCU instructions.

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
│   ├── lift.cpp/h            # LLIL lifting
│   ├── text.cpp/h            # Text generation (disassembly)
│   ├── opcodes.h             # Opcode constants
│   ├── registers.h           # Register definitions and metadata
│   ├── flags.h               # Status flag definitions
│   ├── conditions.h          # Conditional codes
│   ├── sizes.h               # Size constants
│   ├── util.cpp/h            # Utility functions
│   ├── *_test.cpp            # Unit tests
├── CMakeLists.txt            # Build configuration
├── README.md                 # User documentation
├── NOTICE.txt                # Copyright and licensing
└── LICENSE                   # MIT license
```

## Core Components

### Architecture Class (`architecture.h/cpp`)

**`TIC28XArchitecture`** extends `BinaryNinja::Architecture` and provides the main interface to Binary Ninja's analysis engine.

Key responsibilities:
- Register and flag definitions
- Instruction decoding dispatch
- Text generation dispatch
- LLIL generation dispatch
- Mode tracking (OBJMODE, AMODE)

Important members:
```cpp
std::vector<std::optional<std::shared_ptr<Instruction>>> INSTRUCTIONS;
ObjectMode OBJMODE;
AddressMode AMODE;
std::unordered_set<uint64_t> REPEAT_ADDRS;  // Tracks RPT instructions
```

### Instruction System (`instructions.h/cpp`)

**Abstract Base Classes**:
- `Instruction` - Pure virtual base for all instructions
- `Instruction2Byte` - 16-bit instructions
- `Instruction4Byte` - 32-bit instructions

**Concrete Instruction Classes**: Each CPU instruction has its own class that implements:
- `GetOpcode()` / `GetOpcodeMask()` - Matching
- `GetLength()` - Instruction length (2 or 4 bytes)
- `Info()` - Control flow information
- `Text()` - Disassembly text
- `Lift()` - LLIL generation

**Instruction Matching**: Instructions are stored in a vector and matched by:
1. Masking the raw bytes with the instruction's opcode mask
2. Comparing against the instruction's opcode
3. Checking compatibility mode requirements

### Registers (`registers.h`)

1. **General Purpose**:
   - `ACC` (32-bit accumulator) with sub-registers `AH` (high 16), `AL` (low 16)
   - `XAR0-XAR7` (32-bit auxiliary registers)
   - `AR0-AR7` (low 16 bits of XARn), `AR0H-AR7H` (high 16 bits)

2. **System Registers**:
   - `PC` (22-bit program counter), `SP` (16-bit stack pointer)
   - `ST0`, `ST1` (status registers), `DP` (data page pointer)
   - `P` (32-bit product) with `PH`, `PL`
   - `XT` (32-bit temp) with `T`/`TH`, `TL`

3. **FPU Registers**: `R0H-R7H`, `R0L-R7L`, `STF`, `RB`

4. **VCU Registers**: `VR0-VR8`, `VSTATUS`

### Flags (`flags.h`)

**ST0 Flags**: `SXM`, `OVM`, `TC`, `C`, `Z`, `N`, `V`, `PM`, `OVC`

**ST1 Flags**: `INTM`, `DBGM`, `AMODE`, `OBJMODE`, `EALLOW`, `LOOP`, `ARP`

**VSTATUS Flags** (VCU Status Register - TI SPRUHS1C Section 5.3.2):
- `VSTATUS_SAT` (bit 10) - Saturation mode enable
- `VSTATUS_RND` (bit 11) - Rounding mode
- `VSTATUS_OVFR` (bit 12) - Overflow flag: Real
- `VSTATUS_OVRI` (bit 13) - Overflow flag: Imaginary
- Additional fields: `SHIFTR` (bits 4-0), `SHIFTL` (bits 9-5), `CPACK` (bit 14), `OPACK` (bit 15)

The `VStatusBits` namespace in `flags.h` provides bit positions and masks.

### Opcodes (`opcodes.h`)

Opcode constants and masks for all instructions. Key mask patterns:

| Mask | Use Case |
|------|----------|
| `MASK_FFFF` | 2-byte exact match |
| `MASK_FFFFFFC0` | 4-byte, bits 5-0 variable |
| `MASK_FFFFF800` | 4-byte, bits 10-0 variable |
| `MASK_FFF80000` | 4-byte, bits 18-0 variable |

### Utilities (`util.h/cpp`)

- `DataToOpcode()` - Converts raw byte array to opcode value
- `CreateLowerBitMask()` - Generates bit masks for field extraction

## Key Design Patterns

### Instruction Factory
Instructions are generated once during architecture initialization via `GenerateInstructionVector()`.

### Virtual Dispatch
Each instruction class overrides virtual methods from `Instruction`, enabling generic dispatch without large switch statements.

### Bit Field Extraction
Instruction operands use static `Get*`/`Set*` methods with bit manipulation. Shared helpers exist for common formats (FPU Format I-IV, VCU formats).

### Thread-Safe Repeat Tracking
RPT instructions are tracked with a mutex-protected set (`REPEAT_ADDRS`).

## Implementing New Instructions

Use the specialized agents in `.claude/agents/`. The workflow is:

1. **`c28x-instruction-definer`** (always first) — creates opcode, class, helpers, decoder test
2. **`c28x-text-generator`** + **`c28x-llil-lifter`** (parallel) — implements Text() and Lift()
3. **`c28x-info-generator`** (control flow instructions only) — implements Info()

Each agent references Binary Ninja API skills in `.claude/skills/` (`binja-info`, `binja-text`, `binja-lift`) for authoritative API documentation.

Provide a screenshot from the TI manual (SPRU430F or SPRUHS1C) showing the instruction mnemonic, opcode encoding, description, pseudocode, and affected flags.

## Current Limitations

1. **Partial LLIL Lifting**: Only VCU instructions have LLIL lifting implemented
2. **Standard Instruction Set Only**: Extended instruction set not fully supported
3. **FPU Instructions**: Floating-point instructions may not be fully tested
4. **VSTATUS Saturation**: Saturation mode implemented but VSATON/VSATOFF lifting pending

## Testing

```bash
cmake -DCMAKE_BUILD_TYPE=debug -DHEADLESS=yes .
cmake --build . --target tic28x_architecture_test
./tic28x_architecture_test
```

## References

- [TIC28x CPU and Instruction Set Reference](https://www.ti.com/lit/ug/spru430f/spru430f.pdf)
- [TIC28x Extended Instruction Set Reference](https://www.ti.com/lit/ug/spruhs1c/spruhs1c.pdf)
- [Binary Ninja C++ API Documentation](https://api.binary.ninja/cpp/index.html)

## License

Licensed under the MIT License. Copyright 2025 Battelle Energy Alliance, LLC.

Developed at Idaho National Laboratory under contract with the U.S. Department of Energy.

See `LICENSE` and `NOTICE.txt` for full details.
