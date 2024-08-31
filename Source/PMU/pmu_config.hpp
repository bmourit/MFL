// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace pmu {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class PMU_Regs {
    CTL = 0x00,
    CS = 0x04
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL_Bits {
    LDOLP = REG_BIT_DEF(0, 0),
    STBMOD = REG_BIT_DEF(1, 1),
    WURST = REG_BIT_DEF(2, 2),
    STBRST = REG_BIT_DEF(3, 3),
    LVDEN = REG_BIT_DEF(4, 4),
    LVDT = REG_BIT_DEF(5, 7),
    BKPWEN = REG_BIT_DEF(8, 8),
    LDLP = REG_BIT_DEF(10, 10),
    LDNP = REG_BIT_DEF(11, 11),
    LDOVS = REG_BIT_DEF(14, 15),
    HDEN = REG_BIT_DEF(16, 16),
    HDS = REG_BIT_DEF(17, 17),
    LDEN = REG_BIT_DEF(18, 19)
};

enum class CS_Bits {
    WUF = REG_BIT_DEF(0, 0),
    STBF = REG_BIT_DEF(1, 1),
    LVDF = REG_BIT_DEF(2, 2),
    WUPEN = REG_BIT_DEF(8, 8),
    LDOVSRF = REG_BIT_DEF(14, 14),
    HDRF = REG_BIT_DEF(16, 16),
    HDSRF = REG_BIT_DEF(17, 17),
    LDRF = REG_BIT_DEF(18, 19),
};


///////////////////////////// ENUMS /////////////////////////////

enum class LVD_Threshold {
    LVDT_2_1,   // 2.1V
    LVDT_2_3,   // 2.3V
    LVDT_2_4,   // 2.4V
    LVDT_2_6,   // 2.6V
    LVDT_2_7,   // 2.7V
    LVDT_2_8,   // 2.8V
    LVDT_3_0,   // 3.0V
    LVDT_3_1,   // 3.1V
};

enum class Power_Driver {
    NORMAL_DRIVER,
    LOW_DRIVER,
};

enum class Output_Voltage {
    LDO_VOLTAGE_LOW = 1,
    LDO_VOLTAGE_MID = 2,
    LDO_VOLTAGE_HIGH = 3,
};

enum class Low_Driver {
    LOW_DRIVER_DEEP_SLEEP_DISABLE = 0,
    LOW_DRIVER_DEEP_SLEEP_ENABLE = 3,
};

// Alias for CS_Bits
enum class Status_Flags {
    WAKEUP_FLAG = REG_BIT_DEF(0, 0),
    STANDBY_FLAG = REG_BIT_DEF(1, 1),
    LVD_FLAG = REG_BIT_DEF(2, 2),
    FDOVSR_FLAG = REG_BIT_DEF(14, 14),
    HDR_FLAG = REG_BIT_DEF(16, 16),
    HDSR_FLAG = REG_BIT_DEF(17, 17),
    LDR_FLAG = REG_BIT_DEF(18, 19),
};

enum class Clear_Flags {
    RESET_WAKEUP_FLAG,
    RESET_STANDBY_FLAG
};

enum class PMU_Commands {
    WFI_CMD,
    WFE_CMD
};

} // namespace pmu
