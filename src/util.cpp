// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "util.h"

#include "sizes.h"

// Take in an array of bytes and create an integer opcode so we can extract data
uint32_t DataToOpcode(const uint8_t* data, const size_t len) {
  if (len == TIC28X::Sizes::_2_BYTES) {
    return data[1] << 8 | data[0];
  }
  if (len == TIC28X::Sizes::_4_BYTES) {
    return data[1] << 24 | data[0] << 16 | data[3] << 8 | data[2];
  }
  return 0;
}

// Returns a bit mask with the lower n bits set to 1
uint64_t CreateLowerBitMask(unsigned int n) {
  if (n <= 0 || n > 63) {
    return 0;
  }
  return (1u << n) - 1;
}