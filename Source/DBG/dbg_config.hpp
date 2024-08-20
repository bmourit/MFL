// gd32f30x DBG debug in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace armdbg {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class DBG_Regs {
    ID = 0x00,
    CTL0 = 0x04,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL0_Bits {
    SLP_HOLD = REG_BIT_DEF(0, 0),
    DSLP_HOLD = REG_BIT_DEF(1, 1),
    STB_HOLD = REG_BIT_DEF(2, 2),
    TRACE_IOEN = REG_BIT_DEF(5, 5),
    FWDGT_HOLD = REG_BIT_DEF(8, 8),
    WWDGT_HOLD = REG_BIT_DEF(9, 9),
    TIMER0_HOLD = REG_BIT_DEF(10, 10),
    TIMER1_HOLD = REG_BIT_DEF(11, 11),
    TIMER2_HOLD = REG_BIT_DEF(12, 12),
    TIMER3_HOLD = REG_BIT_DEF(13, 13),
    CAN0_HOLD = REG_BIT_DEF(14, 14),
    I2C0_HOLD = REG_BIT_DEF(15, 15),
    I2C1_HOLD = REG_BIT_DEF(16, 16),
    TIMER7_HOLD = REG_BIT_DEF(17, 17),
    TIMER4_HOLD = REG_BIT_DEF(18, 18),
    TIMER5_HOLD = REG_BIT_DEF(19, 19),
    TIMER6_HOLD = REG_BIT_DEF(20, 20),
};


///////////////////////////// ALIASES /////////////////////////////

enum class Low_Power_Debug {
    DEBUG_SLEEP = REG_BIT_DEF(0, 0),
    DEBUG_DEEPSLEEP = REG_BIT_DEF(1, 1),
    DEBUG_STANDBY = REG_BIT_DEF(2, 2),
};

enum class Debug_Peripheral {
    FWDGT = REG_BIT_DEF(8, 8),
    WWDGT = REG_BIT_DEF(9, 9),
    TIMER0 = REG_BIT_DEF(10, 10),
    TIMER1 = REG_BIT_DEF(11, 11),
    TIMER2 = REG_BIT_DEF(12, 12),
    TIMER3 = REG_BIT_DEF(13, 13),
    CAN0 = REG_BIT_DEF(14, 14),
    I2C0 = REG_BIT_DEF(15, 15),
    I2C1 = REG_BIT_DEF(16, 16),
    TIMER7 = REG_BIT_DEF(17, 17),
    TIMER4 = REG_BIT_DEF(18, 18),
    TIMER5 = REG_BIT_DEF(19, 19),
    TIMER6 = REG_BIT_DEF(20, 20),
};

} // namespace dbg
