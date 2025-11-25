// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_CONDITIONS_H
#define TIC28X_CONDITIONS_H
#include <cstdint>
#include <string>
#include <unordered_map>

namespace TIC28X::Conditions {
constexpr uint8_t NEQ = 0x0;
constexpr uint8_t EQ = 0x1;
constexpr uint8_t GT = 0x2;
constexpr uint8_t GEQ = 0x3;
constexpr uint8_t LT = 0x4;
constexpr uint8_t LEQ = 0x5;
constexpr uint8_t HI = 0x6;
constexpr uint8_t HIS_C = 0x7;
constexpr uint8_t LO_NC = 0x8;
constexpr uint8_t LOS = 0x9;
constexpr uint8_t NOV = 0xA;
constexpr uint8_t OV = 0xB;
constexpr uint8_t NTC = 0xC;
constexpr uint8_t TC = 0xD;
constexpr uint8_t NBIO = 0xE;
constexpr uint8_t UNC = 0xF;

inline std::unordered_map<uint8_t, std::string> NAMES = {
    {NEQ, "neq"},      {EQ, "eq"},   {GT, "gt"},     {GEQ, "geq"},
    {LT, "lt"},        {LEQ, "leq"}, {HI, "hi"},     {HIS_C, "his, c"},
    {LO_NC, "lo, nc"}, {LOS, "los"}, {NOV, "nov"},   {OV, "ov"},
    {NTC, "ntc"},      {TC, "tc"},   {NBIO, "nbio"}, {UNC, "unc"},
};

}  // namespace TIC28X::Conditions

#endif  // TIC28X_CONDITIONS_H
