// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_FLAGS_H
#define TIC28X_FLAGS_H

namespace TIC28X::Flags {
// ST0 Flags
constexpr uint8_t SXM = 0;
constexpr uint8_t OVM = 1;
constexpr uint8_t TC = 2;
constexpr uint8_t C = 3;
constexpr uint8_t Z = 4;
constexpr uint8_t N = 5;
constexpr uint8_t V = 6;
constexpr uint8_t PM = 7;
constexpr uint8_t OVC = 8;

// ST1 Flags
constexpr uint8_t INTM = 9;
constexpr uint8_t DBGM = 10;
constexpr uint8_t PAGE0 = 11;
constexpr uint8_t VMAP = 12;
constexpr uint8_t SPA = 13;
constexpr uint8_t LOOP = 14;
constexpr uint8_t EALLOW = 15;
constexpr uint8_t IDLESTAT = 16;
constexpr uint8_t AMODE = 17;
constexpr uint8_t OBJMODE = 18;
constexpr uint8_t M0M1MAP = 19;
constexpr uint8_t XF = 20;
constexpr uint8_t ARP = 21;

// VSTATUS Flags (VCU Status Register)
// These are tracked as separate flags for Binary Ninja analysis
constexpr uint8_t VSTATUS_OVFR = 22;  // VCU Overflow flag: Real (bit 12)
constexpr uint8_t VSTATUS_OVRI = 23;  // VCU Overflow flag: Imaginary (bit 13)
constexpr uint8_t VSTATUS_SAT = 24;   // VCU Saturation mode enable (bit 10)
constexpr uint8_t VSTATUS_RND = 25;   // VCU Rounding mode (bit 11)

// VSTATUS bit positions within the 32-bit VSTATUS register
// Reference: TI SPRUHS1C Section 5.3.2 - VCU Status Register (VSTATUS)
namespace VStatusBits {
// Bit positions
constexpr uint32_t SHIFTR_BIT = 0;  // Right shift amount (bits 4-0)
constexpr uint32_t SHIFTL_BIT = 5;  // Left shift amount (bits 9-5)
constexpr uint32_t SAT_BIT = 10;    // Saturation mode at bit 10
constexpr uint32_t RND_BIT = 11;    // Rounding mode at bit 11
constexpr uint32_t OVFR_BIT = 12;   // Overflow flag: Real at bit 12
constexpr uint32_t OVRI_BIT = 13;   // Overflow flag: Imaginary at bit 13
constexpr uint32_t CPACK_BIT = 14;  // Complex packing order at bit 14
constexpr uint32_t OPACK_BIT = 15;  // Viterbi traceback packing order at bit 15

// Masks
constexpr uint32_t SHIFTR_MASK = 0x1Fu;             // bits 4-0
constexpr uint32_t SHIFTL_MASK = (0x1Fu << 5);      // bits 9-5
constexpr uint32_t SAT_MASK = (1u << SAT_BIT);      // bit 10
constexpr uint32_t RND_MASK = (1u << RND_BIT);      // bit 11
constexpr uint32_t OVFR_MASK = (1u << OVFR_BIT);    // bit 12
constexpr uint32_t OVRI_MASK = (1u << OVRI_BIT);    // bit 13
constexpr uint32_t CPACK_MASK = (1u << CPACK_BIT);  // bit 14
constexpr uint32_t OPACK_MASK = (1u << OPACK_BIT);  // bit 15
}  // namespace VStatusBits

inline std::unordered_map<uint32_t, std::string> NAMES = {
    {SXM, "sxm"},
    {OVM, "ovm"},
    {TC, "tc"},
    {C, "c"},
    {Z, "z"},
    {N, "n"},
    {V, "v"},
    {PM, "pm"},
    {OVC, "ovc"},
    {INTM, "intm"},
    {DBGM, "dbgm"},
    {PAGE0, "page0"},
    {VMAP, "vmap"},
    {SPA, "spa"},
    {LOOP, "loop"},
    {EALLOW, "eallow"},
    {IDLESTAT, "idlestat"},
    {AMODE, "amode"},
    {OBJMODE, "objmode"},
    {M0M1MAP, "m0m1map"},
    {XF, "xf"},
    {ARP, "arp"},
    {VSTATUS_OVFR, "vstatus_ovfr"},
    {VSTATUS_OVRI, "vstatus_ovri"},
    {VSTATUS_SAT, "vstatus_sat"},
    {VSTATUS_RND, "vstatus_rnd"},
};

}  // namespace TIC28X::Flags

#endif  // TIC28X_FLAGS_H