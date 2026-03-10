// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_LIFT_H
#define TIC28X_LIFT_H

#include <binaryninjaapi.h>

#include <cstdint>

#include "architecture.h"

namespace BN = BinaryNinja;

namespace TIC28X {

// Convert a 3-bit VR register index (0-7) to the corresponding register enum
uint8_t VrIndexToReg(uint8_t index);

// Saturation constants for 32-bit signed values
constexpr int32_t SAT_MAX_32 = 0x7FFFFFFF;  // 2^31 - 1
constexpr int32_t SAT_MIN_32 = 0x80000000;  // -2^31

// Helper to generate LLIL for extracting the SAT bit from VSTATUS
// Returns an IL expression that is non-zero if saturation mode is enabled
BN::ExprId GetVstatusSatBit(BN::LowLevelILFunction& il);

// Helper to generate LLIL for extracting the RND bit from VSTATUS
// Returns an IL expression that is non-zero if rounding mode is enabled
BN::ExprId GetVstatusRndBit(BN::LowLevelILFunction& il);

// Helper to generate LLIL for setting the OVFR flag in VSTATUS
// Sets VSTATUS.OVFR = 1 if overflow occurred
void SetVstatusOvfr(BN::LowLevelILFunction& il);

// Helper to generate LLIL for clearing the OVFR flag in VSTATUS
void ClearVstatusOvfr(BN::LowLevelILFunction& il);

// Helper to detect if a left shift will overflow (for 32-bit signed)
// Returns IL expression that is true if shifting 'value' left by 'shiftAmt'
// overflows
BN::ExprId WillShiftOverflow32(BN::LowLevelILFunction& il, BN::ExprId value,
                               uint8_t shiftAmt);

}  // namespace TIC28X

#endif  // TIC28X_LIFT_H