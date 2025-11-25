// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_REGISTERS_H_
#define TIC28X_REGISTERS_H_

namespace TIC28X::Registers {
struct RegInfo {
  uint8_t fullWidthReg;
  uint8_t offset;
  uint8_t size;
  bool zextend;
};

enum Regs : uint8_t {
  // General purpose registers
  ACC,
  AH,
  AL,
  XAR0,
  XAR1,
  XAR2,
  XAR3,
  XAR4,
  XAR5,
  XAR6,
  XAR7,

  // XAR sub-registers (16 LSB)
  AR0,
  AR1,
  AR2,
  AR3,
  AR4,
  AR5,
  AR6,
  AR7,

  // XAR sub-registers (16 MSB)
  AR0H,
  AR1H,
  AR2H,
  AR3H,
  AR4H,
  AR5H,
  AR6H,
  AR7H,

  // XAR reverse carry registers (is this correct?)
  BR0,
  BR1,
  BR2,
  BR3,
  BR4,
  BR5,
  BR6,
  BR7,

  // System registers
  DP,
  IFR,
  IER,
  DBGIER,
  P,
  PH,
  PL,
  PC,
  RPC,
  SP,
  ST0,
  ST1,
  XT,
  T,
  TL,
  TH,

  // FPU registers
  R0H,
  R1H,
  R2H,
  R3H,
  R4H,
  R5H,
  R6H,
  R7H,
  R0L,
  R1L,
  R2L,
  R3L,
  R4L,
  R5L,
  R6L,
  R7L,
  STF,
  RB,
};

inline std::unordered_map<uint8_t, std::string> NAMES = {
    {ACC, "acc"},   {AH, "ah"},     {AL, "al"},         {XAR0, "xar0"},
    {XAR1, "xar1"}, {XAR2, "xar2"}, {XAR3, "xar3"},     {XAR4, "xar4"},
    {XAR5, "xar5"}, {XAR6, "xar6"}, {XAR7, "xar7"},     {AR0, "ar0"},
    {AR1, "ar1"},   {AR2, "ar2"},   {AR3, "ar3"},       {AR4, "ar4"},
    {AR5, "ar5"},   {AR6, "ar6"},   {AR7, "ar7"},       {AR0H, "ar0h"},
    {AR1H, "ar1h"}, {AR2H, "ar2h"}, {AR3H, "ar3h"},     {AR4H, "ar4h"},
    {AR5H, "ar5h"}, {AR6H, "ar6h"}, {AR7H, "ar7h"},     {BR0, "br0"},
    {BR1, "br1"},   {BR2, "br2"},   {BR3, "br3"},       {BR4, "br4"},
    {BR5, "br5"},   {BR6, "br6"},   {BR7, "br7"},       {DP, "dp"},
    {IFR, "ifr"},   {IER, "ier"},   {DBGIER, "dbgier"}, {P, "p"},
    {PH, "ph"},     {PL, "pl"},     {PC, "pc"},         {RPC, "rpc"},
    {SP, "sp"},     {ST0, "st0"},   {ST1, "st1"},       {XT, "xt"},
    {T, "t"},       {TL, "tl"},     {TH, "th"},         {R0H, "r0h"},
    {R1H, "r1h"},   {R2H, "r2h"},   {R3H, "r3h"},       {R4H, "r4h"},
    {R5H, "r5h"},   {R6H, "r6h"},   {R7H, "r7h"},       {R0L, "r0l"},
    {R1L, "r1l"},   {R2L, "r2l"},   {R3L, "r3l"},       {R4L, "r4l"},
    {R5L, "r5l"},   {R6L, "r6l"},   {R7L, "r7l"},       {STF, "stf"},
    {RB, "rb"},

};

inline std::unordered_map<uint8_t, RegInfo> INFO = {
    {ACC, RegInfo{.fullWidthReg = ACC, .size = Sizes::_4_BYTES}},
    {AH, RegInfo{.fullWidthReg = ACC,
                 .offset = Sizes::_2_BYTES,
                 .size = Sizes::_2_BYTES}},
    {AL, RegInfo{.fullWidthReg = ACC, .size = Sizes::_2_BYTES}},

    {XAR0, RegInfo{.fullWidthReg = XAR0, .size = Sizes::_4_BYTES}},
    {XAR1, RegInfo{.fullWidthReg = XAR1, .size = Sizes::_4_BYTES}},
    {XAR2, RegInfo{.fullWidthReg = XAR2, .size = Sizes::_4_BYTES}},
    {XAR3, RegInfo{.fullWidthReg = XAR3, .size = Sizes::_4_BYTES}},
    {XAR4, RegInfo{.fullWidthReg = XAR4, .size = Sizes::_4_BYTES}},
    {XAR5, RegInfo{.fullWidthReg = XAR5, .size = Sizes::_4_BYTES}},
    {XAR6, RegInfo{.fullWidthReg = XAR6, .size = Sizes::_4_BYTES}},
    {XAR7, RegInfo{.fullWidthReg = XAR7, .size = Sizes::_4_BYTES}},

    {AR0, RegInfo{.fullWidthReg = XAR0, .size = Sizes::_2_BYTES}},
    {AR1, RegInfo{.fullWidthReg = XAR1, .size = Sizes::_2_BYTES}},
    {AR2, RegInfo{.fullWidthReg = XAR2, .size = Sizes::_2_BYTES}},
    {AR3, RegInfo{.fullWidthReg = XAR3, .size = Sizes::_2_BYTES}},
    {AR4, RegInfo{.fullWidthReg = XAR4, .size = Sizes::_2_BYTES}},
    {AR5, RegInfo{.fullWidthReg = XAR5, .size = Sizes::_2_BYTES}},
    {AR6, RegInfo{.fullWidthReg = XAR6, .size = Sizes::_2_BYTES}},
    {AR7, RegInfo{.fullWidthReg = XAR7, .size = Sizes::_2_BYTES}},

    {AR0H, RegInfo{.fullWidthReg = XAR0,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR1H, RegInfo{.fullWidthReg = XAR1,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR2H, RegInfo{.fullWidthReg = XAR2,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR3H, RegInfo{.fullWidthReg = XAR3,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR4H, RegInfo{.fullWidthReg = XAR4,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR5H, RegInfo{.fullWidthReg = XAR5,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR6H, RegInfo{.fullWidthReg = XAR6,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},
    {AR7H, RegInfo{.fullWidthReg = XAR7,
                   .offset = Sizes::_2_BYTES,
                   .size = Sizes::_2_BYTES}},

    // TODO: how should we handle these reverse carry regs?
    {BR0, RegInfo{.fullWidthReg = XAR0, .size = Sizes::_2_BYTES}},
    {BR1, RegInfo{.fullWidthReg = XAR1, .size = Sizes::_2_BYTES}},
    {BR2, RegInfo{.fullWidthReg = XAR2, .size = Sizes::_2_BYTES}},
    {BR3, RegInfo{.fullWidthReg = XAR3, .size = Sizes::_2_BYTES}},
    {BR4, RegInfo{.fullWidthReg = XAR4, .size = Sizes::_2_BYTES}},
    {BR5, RegInfo{.fullWidthReg = XAR5, .size = Sizes::_2_BYTES}},
    {BR6, RegInfo{.fullWidthReg = XAR6, .size = Sizes::_2_BYTES}},
    {BR7, RegInfo{.fullWidthReg = XAR7, .size = Sizes::_2_BYTES}},

    // System registers
    {DP, RegInfo{.fullWidthReg = DP, .size = Sizes::_2_BYTES}},
    {IFR, RegInfo{.fullWidthReg = IFR, .size = Sizes::_2_BYTES}},
    {IER, RegInfo{.fullWidthReg = IER, .size = Sizes::_2_BYTES}},
    {DBGIER, RegInfo{.fullWidthReg = DBGIER, .size = Sizes::_2_BYTES}},
    {P, RegInfo{.fullWidthReg = P, .size = Sizes::_4_BYTES}},
    {PH, RegInfo{.fullWidthReg = P,
                 .offset = Sizes::_2_BYTES,
                 .size = Sizes::_2_BYTES}},
    {PL, RegInfo{.fullWidthReg = P, .size = Sizes::_2_BYTES}},
    {PC,
     RegInfo{
         .fullWidthReg = PC,
         .size = Sizes::_4_BYTES}},  // NOTE: this is actually a 22-bit register
    {RPC,
     RegInfo{
         .fullWidthReg = RPC,
         .size = Sizes::_4_BYTES}},  // NOTE: this is actually a 22-bit register
    {SP, RegInfo{.fullWidthReg = SP, .size = Sizes::_2_BYTES}},
    {ST0, RegInfo{.fullWidthReg = ST0, .size = Sizes::_2_BYTES}},
    {ST1, RegInfo{.fullWidthReg = ST1, .size = Sizes::_2_BYTES}},
    {XT, RegInfo{.fullWidthReg = XT, .size = Sizes::_4_BYTES}},
    {T,
     RegInfo{.fullWidthReg = XT,
             .offset = Sizes::_2_BYTES,
             .size = Sizes::_2_BYTES}},  // NOTE: as far as I can tell, T and TH
                                         // both refer to the high half of XT
    {TH, RegInfo{.fullWidthReg = XT,
                 .offset = Sizes::_2_BYTES,
                 .size = Sizes::_2_BYTES}},
    {TL, RegInfo{.fullWidthReg = XT, .size = Sizes::_2_BYTES}},

    // FPU registers
    {R0H, RegInfo{.fullWidthReg = R0H, .size = Sizes::_4_BYTES}},
    {R1H, RegInfo{.fullWidthReg = R1H, .size = Sizes::_4_BYTES}},
    {R2H, RegInfo{.fullWidthReg = R2H, .size = Sizes::_4_BYTES}},
    {R3H, RegInfo{.fullWidthReg = R3H, .size = Sizes::_4_BYTES}},
    {R4H, RegInfo{.fullWidthReg = R4H, .size = Sizes::_4_BYTES}},
    {R5H, RegInfo{.fullWidthReg = R5H, .size = Sizes::_4_BYTES}},
    {R6H, RegInfo{.fullWidthReg = R6H, .size = Sizes::_4_BYTES}},
    {R7H, RegInfo{.fullWidthReg = R7H, .size = Sizes::_4_BYTES}},
    {R0L, RegInfo{.fullWidthReg = R0L, .size = Sizes::_4_BYTES}},
    {R1L, RegInfo{.fullWidthReg = R1L, .size = Sizes::_4_BYTES}},
    {R2L, RegInfo{.fullWidthReg = R2L, .size = Sizes::_4_BYTES}},
    {R3L, RegInfo{.fullWidthReg = R3L, .size = Sizes::_4_BYTES}},
    {R4L, RegInfo{.fullWidthReg = R4L, .size = Sizes::_4_BYTES}},
    {R5L, RegInfo{.fullWidthReg = R5L, .size = Sizes::_4_BYTES}},
    {R6L, RegInfo{.fullWidthReg = R6L, .size = Sizes::_4_BYTES}},
    {R7L, RegInfo{.fullWidthReg = R7L, .size = Sizes::_4_BYTES}},
    {STF, RegInfo{.fullWidthReg = STF, .size = Sizes::_4_BYTES}},
    {RB, RegInfo{.fullWidthReg = RB, .size = Sizes::_4_BYTES}},
};

}  // namespace TIC28X::Registers

#endif  // TIC28X_REGISTERS_H_
