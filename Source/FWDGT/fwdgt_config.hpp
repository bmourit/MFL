// gd32f30x FWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include "CONFIG.hpp"

namespace fwdgt {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class FWDGT_Regs {
    CTL = 0x00,
    PSC = 0x04,
    RLD = 0x08,
    STAT = 0x0C,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL_Bits {
    CMD = REG_BIT_DEF(0, 15),
};

enum class PSC_Bits {
    PSC = REG_BIT_DEF(0, 2),
};

enum class RLD_Bits {
    RLD = REG_BIT_DEF(0, 11),
};

enum class STAT_Bits {
    PUD = REG_BIT_DEF(0, 0),
    RUD = REG_BIT_DEF(1, 1),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Prescaler_Values {
    DIV4,
    DIV8,
    DIV16,
    DIV32,
    DIV64,
    DIV128,
    DIV256,
};

enum class State_Flags {
    FLAG_PUD,
    FLAG_RUD,
};

} // namespace fwdgt


///////////////////////////// REFERENCE /////////////////////////////

//value to enable write access is 0x00005555
//value to disable write access is 0x00000000
//value for reload key is 0x0000AAAA
//value to enable key is 0x0000CCCC
