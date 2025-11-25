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
};

}  // namespace TIC28X::Flags

#endif  // TIC28X_FLAGS_H