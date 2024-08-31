// gd32f30x CEE register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"
#include "BitRW.hpp"

namespace cee {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class CEE_Regs {
    CEE = 0x00,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CEE_Bits {
    CEE_EN = REG_BIT_DEF(7, 7),
};

} // namespace cee
