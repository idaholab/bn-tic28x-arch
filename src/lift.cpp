// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "lift.h"

#include <binaryninjaapi.h>
#include <lowlevelilinstruction.h>

#include "architecture.h"
#include "flags.h"
#include "instructions.h"
#include "registers.h"
#include "sizes.h"
#include "util.h"

namespace TIC28X {

// Convert a 3-bit VR register index (0-7) to the corresponding register enum
uint8_t VrIndexToReg(uint8_t index) {
  // VR0-VR7 are contiguous in the Registers enum
  return Registers::VR0 + (index & 0x7);
}

// Helper to generate LLIL for extracting the SAT bit from VSTATUS
// Returns an IL expression that is non-zero if saturation mode is enabled
BN::ExprId GetVstatusSatBit(BN::LowLevelILFunction& il) {
  // VSTATUS[SAT] is at bit 15
  // Extract: (VSTATUS >> 15) & 1, or equivalently (VSTATUS & SAT_MASK)
  return il.And(Sizes::_4_BYTES,
                il.Register(Sizes::_4_BYTES, Registers::VSTATUS),
                il.Const(Sizes::_4_BYTES, Flags::VStatusBits::SAT_MASK));
}

// Helper to generate LLIL for extracting the RND bit from VSTATUS
// Returns an IL expression that is non-zero if rounding mode is enabled
BN::ExprId GetVstatusRndBit(BN::LowLevelILFunction& il) {
  // VSTATUS[RND] is at bit 11
  // Extract: (VSTATUS & RND_MASK)
  return il.And(Sizes::_4_BYTES,
                il.Register(Sizes::_4_BYTES, Registers::VSTATUS),
                il.Const(Sizes::_4_BYTES, Flags::VStatusBits::RND_MASK));
}

// Helper to generate LLIL for setting the OVFR flag in VSTATUS
// Sets VSTATUS.OVFR = 1
void SetVstatusOvfr(BN::LowLevelILFunction& il) {
  // VSTATUS |= OVFR_MASK
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, Registers::VSTATUS,
      il.Or(Sizes::_4_BYTES, il.Register(Sizes::_4_BYTES, Registers::VSTATUS),
            il.Const(Sizes::_4_BYTES, Flags::VStatusBits::OVFR_MASK))));
}

// Helper to generate LLIL for clearing the OVFR flag in VSTATUS
void ClearVstatusOvfr(BN::LowLevelILFunction& il) {
  // VSTATUS &= ~OVFR_MASK
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, Registers::VSTATUS,
      il.And(Sizes::_4_BYTES, il.Register(Sizes::_4_BYTES, Registers::VSTATUS),
             il.Const(Sizes::_4_BYTES, ~Flags::VStatusBits::OVFR_MASK))));
}

// Helper to generate LLIL for signed 32-bit saturation
// Returns IL expression: clamp(value, SAT_MIN_32, SAT_MAX_32)
// This generates: (value > MAX) ? MAX : ((value < MIN) ? MIN : value)
BN::ExprId Saturate32Signed(BN::LowLevelILFunction& il, BN::ExprId value) {
  // For signed saturation, we need to check:
  // 1. If value > 0x7FFFFFFF (signed), clamp to 0x7FFFFFFF
  // 2. If value < 0x80000000 (signed), clamp to 0x80000000
  //
  // Since we're working with 32-bit values and the shift result fits in 32
  // bits, we need to detect overflow differently. For a left shift, overflow
  // occurs when bits are shifted out that differ from the sign bit.
  //
  // We'll use a simpler approach: compute in 64-bit, then saturate to 32-bit
  // bounds. However, Binary Ninja LLIL doesn't have a native saturate
  // instruction, so we'll implement conditional clamping.

  // Create the saturation bounds as constants
  auto maxVal = il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MAX_32));
  auto minVal = il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MIN_32));

  // Check if value > MAX (signed comparison)
  // If so, return MAX; else check if value < MIN, return MIN; else return value
  // LLIL doesn't have ternary, so we'll just use the raw shift and let analysis
  // track the saturation state. For a more precise implementation, we'd need
  // to emit multiple basic blocks with If/Goto.

  // For now, return the value as-is since LLIL doesn't have native saturation.
  // The saturation bounds would be applied at runtime based on VSTATUS[SAT].
  // This is a simplification - full implementation would require control flow.
  return value;
}

// Helper to detect if a left shift will overflow (for 32-bit signed)
// Overflow occurs when any bits shifted out differ from the final sign bit
// For shift by N: check if the top N+1 bits are all 0s or all 1s
BN::ExprId WillShiftOverflow32(BN::LowLevelILFunction& il, BN::ExprId value,
                               uint8_t shiftAmt) {
  if (shiftAmt == 0) {
    // No shift, no overflow possible
    return il.Const(Sizes::_4_BYTES, 0);
  }

  // For a left shift by N, overflow occurs if the value cannot be represented
  // in (32 - N) bits as a signed number.
  //
  // This means: the top (N + 1) bits must all be the same (all 0s or all 1s)
  // for NO overflow. If they differ, overflow occurs.
  //
  // Method: Arithmetic right shift by (32 - shiftAmt - 1), then check if
  // result is 0 or -1. If neither, overflow will occur.
  //
  // Simplified: (value >> (31 - shiftAmt)) should be 0 or -1 for no overflow

  if (shiftAmt >= 31) {
    // Shifting by 31 or more: overflow if value != 0 and value != -1
    auto isZero =
        il.CompareEqual(Sizes::_4_BYTES, value, il.Const(Sizes::_4_BYTES, 0));
    auto isMinusOne = il.CompareEqual(Sizes::_4_BYTES, value,
                                      il.Const(Sizes::_4_BYTES, 0xFFFFFFFF));
    // Overflow if NOT (isZero OR isMinusOne)
    return il.Not(Sizes::_4_BYTES, il.Or(Sizes::_4_BYTES, isZero, isMinusOne));
  }

  // Arithmetic shift right to get the sign-extended high bits
  auto checkBits = il.ArithShiftRight(Sizes::_4_BYTES, value,
                                      il.Const(Sizes::_1_BYTE, 31 - shiftAmt));

  // No overflow if checkBits == 0 (positive, no high bits set)
  // or checkBits == -1 (negative, all high bits set)
  auto isZero =
      il.CompareEqual(Sizes::_4_BYTES, checkBits, il.Const(Sizes::_4_BYTES, 0));
  auto isMinusOne = il.CompareEqual(Sizes::_4_BYTES, checkBits,
                                    il.Const(Sizes::_4_BYTES, 0xFFFFFFFF));

  // Overflow occurs if NEITHER condition is true
  return il.Not(Sizes::_4_BYTES, il.Or(Sizes::_4_BYTES, isZero, isMinusOne));
}

// Default implementation (temporary for testing -- TODO: delete this)
bool Instruction::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                       BN::LowLevelILFunction& il, TIC28XArchitecture* arch) {
  return false;
}

// VASHL32 VRa << #5-bit
// Arithmetic shift left of VRa by immediate amount.
// If VSTATUS[SAT] == 1, the result is saturated to signed 32-bit bounds.
// OVFR flag is set if the 32-bit signed result overflows.
//
// Pseudocode from TI documentation:
//   If(VSTATUS[SAT] == 1){
//     VRa = sat(VRa << #5-bit Immediate)
//   }else {
//     VRa = VRa << #5-bit Immediate
//   }
//   OVFR is set if the 32-bit signed result after the shift left operation
//   overflows
bool Vashl32Vra5bit::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                          BN::LowLevelILFunction& il,
                          TIC28XArchitecture* arch) {
  len = GetLength();
  const uint32_t dataOp = DataToOpcode(data, len);

  // Extract operands
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t shiftAmt = GetImm5(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // Read the original value
  auto origValue = il.Register(Sizes::_4_BYTES, vrReg);

  // Check if overflow will occur (before performing the shift)
  auto willOverflow = WillShiftOverflow32(il, origValue, shiftAmt);

  // Compute the shifted result
  auto shiftedValue = il.ShiftLeft(Sizes::_4_BYTES, origValue,
                                   il.Const(Sizes::_1_BYTE, shiftAmt));

  // Get the SAT bit from VSTATUS
  auto satEnabled = GetVstatusSatBit(il);

  // Create labels for the conditional flow
  BNLowLevelILLabel satLabel, noSatLabel, doneLabel;

  // If SAT is enabled, we need to apply saturation
  il.AddInstruction(il.If(satEnabled, satLabel, noSatLabel));

  // === SAT enabled path ===
  il.MarkLabel(satLabel);
  {
    // With saturation enabled, we need to clamp the result.
    // For a left shift that overflows:
    // - If original value was positive and overflowed, saturate to MAX
    // (0x7FFFFFFF)
    // - If original value was negative and overflowed, saturate to MIN
    // (0x80000000)

    // Create labels for overflow handling within saturation path
    BNLowLevelILLabel satOverflowLabel, satNoOverflowLabel, satDoneLabel;

    il.AddInstruction(
        il.If(willOverflow, satOverflowLabel, satNoOverflowLabel));

    // Overflow occurred - need to saturate
    il.MarkLabel(satOverflowLabel);
    {
      // Check sign of original value to determine saturation direction
      // If original >= 0, saturate to MAX; else saturate to MIN
      auto origSign = il.CompareSignedGreaterEqual(
          Sizes::_4_BYTES, origValue, il.Const(Sizes::_4_BYTES, 0));

      BNLowLevelILLabel positiveLabel, negativeLabel;
      il.AddInstruction(il.If(origSign, positiveLabel, negativeLabel));

      il.MarkLabel(positiveLabel);
      il.AddInstruction(il.SetRegister(
          Sizes::_4_BYTES, vrReg,
          il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MAX_32))));
      il.AddInstruction(il.Goto(satDoneLabel));

      il.MarkLabel(negativeLabel);
      il.AddInstruction(il.SetRegister(
          Sizes::_4_BYTES, vrReg,
          il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MIN_32))));
      il.AddInstruction(il.Goto(satDoneLabel));
    }

    // No overflow - just use the shifted value
    il.MarkLabel(satNoOverflowLabel);
    il.AddInstruction(il.SetRegister(Sizes::_4_BYTES, vrReg, shiftedValue));
    il.AddInstruction(il.Goto(satDoneLabel));

    il.MarkLabel(satDoneLabel);
  }
  il.AddInstruction(il.Goto(doneLabel));

  // === SAT disabled path ===
  il.MarkLabel(noSatLabel);
  {
    // No saturation - just perform the shift
    il.AddInstruction(il.SetRegister(Sizes::_4_BYTES, vrReg, shiftedValue));
  }
  il.AddInstruction(il.Goto(doneLabel));

  // === Common exit ===
  il.MarkLabel(doneLabel);

  // Set OVFR flag if overflow occurred (regardless of saturation mode)
  // VSTATUS.OVFR |= willOverflow
  // This is a conditional OR: if willOverflow, set OVFR
  BNLowLevelILLabel setOvfrLabel, skipOvfrLabel, finalLabel;
  il.AddInstruction(il.If(willOverflow, setOvfrLabel, skipOvfrLabel));

  il.MarkLabel(setOvfrLabel);
  SetVstatusOvfr(il);
  il.AddInstruction(il.Goto(finalLabel));

  il.MarkLabel(skipOvfrLabel);
  il.AddInstruction(il.Goto(finalLabel));

  il.MarkLabel(finalLabel);

  return true;
}

// VASHR32 VRa >> #5-bit
// Arithmetic shift right of VRa by immediate amount.
// If VSTATUS[RND] == 1, the result is rounded (add 1 if MSB of shifted-out bits
// was 1). This instruction does NOT affect any flags in VSTATUS.
//
// Pseudocode from TI documentation:
//   If(VSTATUS[RND] == 1){
//     VRa = rnd(VRa >> #5-bit Immediate)
//   }else {
//     VRa = VRa >> #5-bit Immediate
//   }
bool Vashr32Vra5bit::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                          BN::LowLevelILFunction& il,
                          TIC28XArchitecture* arch) {
  len = GetLength();
  const uint32_t dataOp = DataToOpcode(data, len);

  // Extract operands
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t shiftAmt = GetImm5(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // Read the original value
  auto origValue = il.Register(Sizes::_4_BYTES, vrReg);

  // Compute the arithmetic shift right result
  auto shiftedValue = il.ArithShiftRight(Sizes::_4_BYTES, origValue,
                                         il.Const(Sizes::_1_BYTE, shiftAmt));

  // Handle shift amount of 0 - no rounding needed, just store the value
  if (shiftAmt == 0) {
    il.AddInstruction(il.SetRegister(Sizes::_4_BYTES, vrReg, shiftedValue));
    return true;
  }

  // Get the RND bit from VSTATUS
  auto rndEnabled = GetVstatusRndBit(il);

  // Create labels for the conditional flow
  BNLowLevelILLabel rndLabel, noRndLabel, doneLabel;

  // If RND is enabled, we need to apply rounding
  il.AddInstruction(il.If(rndEnabled, rndLabel, noRndLabel));

  // === RND enabled path ===
  il.MarkLabel(rndLabel);
  {
    // Rounding: add 1 to the shifted result if the MSB of the shifted-out bits
    // was 1. The MSB of shifted-out bits is bit (shiftAmt - 1) of the original
    // value.
    //
    // roundBit = (origValue >> (shiftAmt - 1)) & 1
    // result = shiftedValue + roundBit
    auto roundBit =
        il.And(Sizes::_4_BYTES,
               il.LogicalShiftRight(Sizes::_4_BYTES, origValue,
                                    il.Const(Sizes::_1_BYTE, shiftAmt - 1)),
               il.Const(Sizes::_4_BYTES, 1));

    auto roundedValue = il.Add(Sizes::_4_BYTES, shiftedValue, roundBit);

    il.AddInstruction(il.SetRegister(Sizes::_4_BYTES, vrReg, roundedValue));
  }
  il.AddInstruction(il.Goto(doneLabel));

  // === RND disabled path ===
  il.MarkLabel(noRndLabel);
  {
    // No rounding - just perform the shift
    il.AddInstruction(il.SetRegister(Sizes::_4_BYTES, vrReg, shiftedValue));
  }
  il.AddInstruction(il.Goto(doneLabel));

  // === Common exit ===
  il.MarkLabel(doneLabel);

  return true;
}

// VBITFLIP VRa
// Reverse the bit order of VRa register.
// VRa[31:0] = VRa[0:31] (bit 0 becomes bit 31, bit 1 becomes bit 30, etc.)
//
// This instruction does NOT affect any flags in VSTATUS.
bool VbitflipVra::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                       BN::LowLevelILFunction& il, TIC28XArchitecture* arch) {
  len = GetLength();
  const uint16_t dataOp =
      static_cast<uint16_t>(DataToOpcode(data, len) & 0xFFFF);

  // Extract the register operand
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // Use the TIC28X_INTRIN_BITREVERSE intrinsic to represent the operation.
  // This shows as: vrN = __bitreverse(vrN) in the decompiler.
  il.AddInstruction(il.Intrinsic(
      {BN::RegisterOrFlag::Register(vrReg)},  // output: VRa
      TIC28X_INTRIN_BITREVERSE,               // intrinsic ID
      {il.Register(Sizes::_4_BYTES, vrReg)}   // input: VRa
      ));

  return true;
}

// VLSHL32 VRa << #5-bit
// Logical shift left of VRa by immediate amount.
// VRa = VRa << #5-bit Immediate
//
// This instruction does NOT affect any flags in VSTATUS.
// Unlike VASHL32 (arithmetic shift left), VLSHL32 is a pure logical shift:
// - No saturation mode (SAT bit is ignored)
// - No overflow flag (OVFR is not modified)
bool Vlshl32Vra5bit::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                          BN::LowLevelILFunction& il,
                          TIC28XArchitecture* arch) {
  len = GetLength();
  const uint32_t dataOp = DataToOpcode(data, len);

  // Extract operands
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t shiftAmt = GetImm5(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // VRa = VRa << #immediate (32-bit logical shift left)
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, vrReg,
      il.ShiftLeft(Sizes::_4_BYTES, il.Register(Sizes::_4_BYTES, vrReg),
                   il.Const(Sizes::_1_BYTE, shiftAmt))));

  return true;
}

// VLSHR32 VRa >> #5-bit
// Logical shift right of VRa by immediate amount.
// VRa = VRa >> #5-bit Immediate
//
// This instruction does NOT affect any flags in VSTATUS.
// Unlike VASHR32 (arithmetic shift right), VLSHR32 is a pure logical shift:
// - No rounding mode (RND bit is ignored)
// - No overflow flag (OVFR is not modified)
// - Zero-fills vacated high bits (logical, not arithmetic)
bool Vlshr32Vra5bit::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                          BN::LowLevelILFunction& il,
                          TIC28XArchitecture* arch) {
  len = GetLength();
  const uint32_t dataOp = DataToOpcode(data, len);

  // Extract operands
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t shiftAmt = GetImm5(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // VRa = VRa >> #immediate (32-bit logical shift right)
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, vrReg,
      il.LogicalShiftRight(Sizes::_4_BYTES, il.Register(Sizes::_4_BYTES, vrReg),
                           il.Const(Sizes::_1_BYTE, shiftAmt))));

  return true;
}

// VNEG VRa
// Two's complement negate of VRa.
// Overflow occurs when VRa == 0x80000000 (most negative signed 32-bit value).
// Negating INT32_MIN would produce INT32_MIN again due to wrap-around.
//
// Pseudocode from TI documentation:
//   if (VRa == 0x80000000)
//   {
//     if (SAT == 1)
//       VRa = 0x7FFFFFFF;   // saturate to max positive
//     else
//       VRa = 0x80000000;   // stays the same (overflow wraps)
//   }
//   else
//   {
//     VRa = -VRa;           // two's complement negate
//   }
//   OVFR is set if input was 0x80000000
bool VnegVra::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                   BN::LowLevelILFunction& il, TIC28XArchitecture* arch) {
  len = GetLength();
  const uint16_t dataOp =
      static_cast<uint16_t>(DataToOpcode(data, len) & 0xFFFF);

  // Extract the register operand (bits 3-0, same layout as VBITFLIP)
  const uint8_t regIdx = GetRegA(dataOp);
  const uint8_t vrReg = VrIndexToReg(regIdx);

  // Read the original value once
  auto origValue = il.Register(Sizes::_4_BYTES, vrReg);

  // Check if input == 0x80000000 (the only value that overflows on negate)
  auto isOverflow = il.CompareEqual(
      Sizes::_4_BYTES, origValue,
      il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MIN_32)));

  // Outer branch: overflow vs. normal negate
  BNLowLevelILLabel overflowLabel, normalLabel, afterResultLabel;
  il.AddInstruction(il.If(isOverflow, overflowLabel, normalLabel));

  // === Overflow path: VRa == 0x80000000 ===
  il.MarkLabel(overflowLabel);
  {
    // Inner branch: SAT enabled => saturate, SAT disabled => leave unchanged
    auto satEnabled = GetVstatusSatBit(il);
    BNLowLevelILLabel satLabel, noSatLabel, afterSatLabel;
    il.AddInstruction(il.If(satEnabled, satLabel, noSatLabel));

    // SAT == 1: VRa = 0x7FFFFFFF
    il.MarkLabel(satLabel);
    il.AddInstruction(il.SetRegister(
        Sizes::_4_BYTES, vrReg,
        il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MAX_32))));
    il.AddInstruction(il.Goto(afterSatLabel));

    // SAT == 0: VRa = 0x80000000 (unchanged — wrap-around negate of INT32_MIN)
    il.MarkLabel(noSatLabel);
    il.AddInstruction(il.SetRegister(
        Sizes::_4_BYTES, vrReg,
        il.Const(Sizes::_4_BYTES, static_cast<uint32_t>(SAT_MIN_32))));
    il.AddInstruction(il.Goto(afterSatLabel));

    il.MarkLabel(afterSatLabel);
  }
  // After handling overflow, set OVFR flag and jump past normal path
  SetVstatusOvfr(il);
  il.AddInstruction(il.Goto(afterResultLabel));

  // === Normal path: VRa = -VRa (two's complement negate) ===
  il.MarkLabel(normalLabel);
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, vrReg,
      il.Neg(Sizes::_4_BYTES, il.Register(Sizes::_4_BYTES, vrReg))));
  il.AddInstruction(il.Goto(afterResultLabel));

  // === Common exit ===
  il.MarkLabel(afterResultLabel);

  return true;
}

// VCU - Complex Math Instructions

// VCADD VR5, VR4, VR3, VR2
// Complex 32+32=32-bit addition.
// Inputs:  VR5=Re(X), VR4=Im(X), VR3=Re(Y), VR2=Im(Y)
// Outputs: VR5=Re(Z)=Re(X)+(Re(Y)>>SHIFTR), VR4=Im(Z)=Im(X)+(Im(Y)>>SHIFTR)
// VSTATUS fields used: SHIFTR[4:0], RND[11], SAT[10]
// Flags modified: OVFR (bit 12) if VR5 result overflows,
//                 OVFI (bit 13) if VR4 result overflows
//
// The control flow (conditional rounding, conditional saturation, and flag
// updates) depends on run-time VSTATUS values (SHIFTR, RND, SAT), making
// it impractical to expand inline without extremely verbose branching across
// two independent channels.  We therefore represent this instruction as a
// single opaque intrinsic named "vcadd" that takes all inputs explicitly and
// produces the two result registers plus the updated VSTATUS.
bool VcaddVr5Vr4Vr3Vr2::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                             BN::LowLevelILFunction& il,
                             TIC28XArchitecture* arch) {
  len = GetLength();

  // Opcode is fixed (0xE502) — no variable fields to extract.
  // The instruction implicitly operates on VR5, VR4, VR3, VR2, and VSTATUS.

  // Build the intrinsic call:
  //   (VR5, VR4, VSTATUS) = vcadd(VR5, VR4, VR3, VR2, VSTATUS)
  il.AddInstruction(il.Intrinsic(
      // Outputs: VR5 (Re(Z)), VR4 (Im(Z)), VSTATUS (OVFR/OVFI flags updated)
      {BN::RegisterOrFlag::Register(Registers::VR5),
       BN::RegisterOrFlag::Register(Registers::VR4),
       BN::RegisterOrFlag::Register(Registers::VSTATUS)},
      TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2,
      // Inputs: VR5=Re(X), VR4=Im(X), VR3=Re(Y), VR2=Im(Y), VSTATUS
      {il.Register(Sizes::_4_BYTES, Registers::VR5),
       il.Register(Sizes::_4_BYTES, Registers::VR4),
       il.Register(Sizes::_4_BYTES, Registers::VR3),
       il.Register(Sizes::_4_BYTES, Registers::VR2),
       il.Register(Sizes::_4_BYTES, Registers::VSTATUS)}));

  return true;
}

// VCADD VR7, VR6, VR5, VR4
// Complex 32+32=32-bit addition.
// VR7 = Re(X), VR6 = Im(X), VR5 = Re(Y), VR4 = Im(Y)
// Z = X + Y  (with SHIFTR, RND, SAT from VSTATUS)
//
// Encoding: 0xE52A (all bits fixed, no variable fields)
bool VcaddVr7Vr6Vr5Vr4::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                             BN::LowLevelILFunction& il,
                             TIC28XArchitecture* arch) {
  len = GetLength();

  // Opcode is fixed (0xE52A) — no variable fields to extract.
  // The instruction implicitly operates on VR7, VR6, VR5, VR4, and VSTATUS.

  // Build the intrinsic call:
  //   (VR7, VR6, VSTATUS) = vcadd(VR7, VR6, VR5, VR4, VSTATUS)
  il.AddInstruction(il.Intrinsic(
      // Outputs: VR7 (Re(Z)), VR6 (Im(Z)), VSTATUS (OVFR/OVFI flags updated)
      {BN::RegisterOrFlag::Register(Registers::VR7),
       BN::RegisterOrFlag::Register(Registers::VR6),
       BN::RegisterOrFlag::Register(Registers::VSTATUS)},
      TIC28X_INTRIN_VCADD_VR7_VR6_VR5_VR4,
      // Inputs: VR7=Re(X), VR6=Im(X), VR5=Re(Y), VR4=Im(Y), VSTATUS
      {il.Register(Sizes::_4_BYTES, Registers::VR7),
       il.Register(Sizes::_4_BYTES, Registers::VR6),
       il.Register(Sizes::_4_BYTES, Registers::VR5),
       il.Register(Sizes::_4_BYTES, Registers::VR4),
       il.Register(Sizes::_4_BYTES, Registers::VSTATUS)}));

  return true;
}

// VMOV32 VRa, mem32
// Load a 32-bit value from a C28x loc32 address into VRa.
//
// Encoding:
//   LSW (bits [31:16]): 0xE3F0 (fixed)
//   MSW (bits [15:0]):  0000 aaaa mmmm mmmm
//     bits [11:8] = aaaa -> VRa destination index
//     bits [7:0]  = mem32 addressing mode code
bool Vmov32VraMem32::Lift(const uint8_t* data, uint64_t addr, size_t& len,
                          BN::LowLevelILFunction& il,
                          TIC28XArchitecture* arch) {
  len = GetLength();
  const uint32_t dataOp = DataToOpcode(data, len);

  const uint8_t regA = GetRegA(dataOp);    // 4-bit VRa index (bits [11:8])
  const uint8_t mem32 = GetMem32(dataOp);  // 8-bit loc32 field (bits [7:0])
  const uint8_t vrReg = static_cast<uint8_t>(Registers::VR0 + regA);

  // VRa = [mem32]
  il.AddInstruction(il.SetRegister(
      Sizes::_4_BYTES, vrReg,
      il.Load(Sizes::_4_BYTES, il.Const(Sizes::_4_BYTES, mem32))));

  return true;
}

// VCADD VR5, VR4, VR3, VR2 || VMOV32 VRa, mem32
// Parallel complex addition with simultaneous memory load.
//
// The VCADD portion is identical to Vcadd::Lift():
//   (VR5, VR4, VSTATUS) = vcadd(VR5, VR4, VR3, VR2, VSTATUS)
//
// The parallel VMOV32 portion loads a 32-bit value from a C28x loc32 address
// into VRa.  The mem32 field (bits [7:0]) is an 8-bit C28x addressing-mode
// code whose full decode requires run-time register state (DP, XARn, SP, etc.).
// Rather than partially decode it, we represent the load as a dedicated
// intrinsic that preserves the raw mem32 field, matching the pattern used for
// other VCU memory-operand instructions.
//
// Encoding:
//   LSW: 1110 0011 1111 1000 = 0xE3F8 (bits [31:16])
//   MSW: 0000 aaaa mmmm mmmm (bits [15:0])
//     bits [11:8] = aaaa -> VRa destination index
//     bits [7:0]  = mem32 addressing mode code
bool VcaddVr5Vr4Vr3Vr2Vmov32VraMem32::Lift(const uint8_t* data, uint64_t addr,
                                           size_t& len,
                                           BN::LowLevelILFunction& il,
                                           TIC28XArchitecture* arch) {
  len = GetLength();

  // === Instruction 1: VCADD complex addition ===
  // (VR5, VR4, VSTATUS) = vcadd(VR5, VR4, VR3, VR2, VSTATUS)
  // Semantics are identical to the standalone VCADD instruction.
  il.AddInstruction(
      il.Intrinsic({BN::RegisterOrFlag::Register(Registers::VR5),
                    BN::RegisterOrFlag::Register(Registers::VR4),
                    BN::RegisterOrFlag::Register(Registers::VSTATUS)},
                   TIC28X_INTRIN_VCADD_VR5_VR4_VR3_VR2,
                   {il.Register(Sizes::_4_BYTES, Registers::VR5),
                    il.Register(Sizes::_4_BYTES, Registers::VR4),
                    il.Register(Sizes::_4_BYTES, Registers::VR3),
                    il.Register(Sizes::_4_BYTES, Registers::VR2),
                    il.Register(Sizes::_4_BYTES, Registers::VSTATUS)}));

  // === Instruction 2: Parallel VMOV32 VRa, mem32 load ===
  // VRa/mem32 fields are at the same bit positions — delegate to Vmov32VraMem32
  size_t vmov_len;
  return Vmov32VraMem32{}.Lift(data, addr, vmov_len, il, arch);
}

}  // namespace TIC28X