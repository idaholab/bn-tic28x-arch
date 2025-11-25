// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_UTIL_H_
#define TIC28X_UTIL_H_
#include <cstddef>
#include <cstdint>

#include "binaryninjacore.h"

uint32_t DataToOpcode(const uint8_t* data, size_t len);
uint64_t CreateLowerBitMask(unsigned int n);

#endif
