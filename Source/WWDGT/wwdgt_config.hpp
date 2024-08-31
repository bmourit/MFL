// gd32f30x WWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace wwdgt {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class WWDGT_Regs {
    CTL = 0x00,
    CFG = 0x04,
    STAT = 0x08,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL_Bits {
    CNT = REG_BIT_DEF(0, 6),
    WDGTEN = REG_BIT_DEF(7, 7),
};

enum class CFG_Bits {
    WIN = REG_BIT_DEF(0, 6),
    PSC = REG_BIT_DEF(7, 8),
    EWIE = REG_BIT_DEF(9, 9),
};

enum class STAT_Bits {
    EWIF = REG_BIT_DEF(0, 0),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Prescaler_Values {
    PSC_DIV1,
    PSC_DIV2,
    PSC_DIV4,
    PSC_DIV8,
};

} // namespace fwdgt
