// gd32f30x RTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace rtc {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class RTC_Regs {
    INTEN = 0x00,
    CTL = 0x04,
    PSCH = 0x08,
    PSCL = 0x0C,
    DIVH = 0x10,
    DIVL = 0x14,
    CNTH = 0x18,
    CNTL = 0x1C,
    ALRMH = 0x20,
    ALRML = 0x24,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class INTEN_Bits {
    SCIE = REG_BIT_DEF(0, 0),
    ALRMIE = REG_BIT_DEF(1, 1),
    OVIE = REG_BIT_DEF(2, 2),
};

enum class Interrupt_Type {
    INTR_SCIE = REG_BIT_DEF(0, 0),
    INTR_ALRMIE = REG_BIT_DEF(1, 1),
    INTR_OVIE = REG_BIT_DEF(2, 2),
};

enum class CTL_Bits {
    SCIF = REG_BIT_DEF(0, 0),
    ALRMIF = REG_BIT_DEF(1, 1),
    OVIF = REG_BIT_DEF(2, 2),
    RSYNF = REG_BIT_DEF(3, 3),
    CMF = REG_BIT_DEF(4, 4),
    LWOFF = REG_BIT_DEF(5, 5),
};

enum class Status_Flags {
    FLAG_SCIF = REG_BIT_DEF(0, 0),
    FLAG_ALRMIF = REG_BIT_DEF(1, 1),
    FLAG_OVIF = REG_BIT_DEF(2, 2),
    FLAG_RSYNF = REG_BIT_DEF(3, 3),
    FLAG_LWOFF = REG_BIT_DEF(5, 5),
};

enum class PSCH_Bits {
    HIGH_PSC = REG_BIT_DEF(0, 3),
};

enum class PSCL_Bits {
    LOW_PSC = REG_BIT_DEF(0, 15),
};

enum class DIVH_Bits {
    HIGH_DIV = REG_BIT_DEF(0, 3),
};

enum class DIVL_Bits {
    LOW_DIV = REG_BIT_DEF(0, 15),
};

enum class CNTH_Bits {
    HIGH_CNT = REG_BIT_DEF(0, 15),
};

enum class CNTL_Bits {
    LOW_CNT = REG_BIT_DEF(0, 15),
};

enum class ALRMH_Bits {
    HIGH_ALRM = REG_BIT_DEF(0, 15),
};

enum class ALRML_Bits {
    LOW_ALRM = REG_BIT_DEF(0, 15),
};

} // namespace rtc
