// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "lift.h"

#include <binaryninjaapi.h>

#include "instructions.h"

namespace TIC28X {
// Default constructor (temporary for testing -- TODO: delete this)
bool Instruction::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                       BN::LowLevelILFunction& il, TIC28XArchitecture* arch) {
  return false;
}

}  // namespace TIC28X