// gd32f30x CRC functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace crc {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class CRC_Regs {
    DATA = 0x00,
    FDATA = 0x04,
    CTL = 0x08,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class FDATA_Bits {
    FDATA = REG_BIT_DEF(0, 7),
};

enum class CTL_Bits {
    RST = REG_BIT_DEF(0, 0),
};

} // namespace crc
