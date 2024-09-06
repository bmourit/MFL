// gd32f30x DAC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace dac {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class DAC_Regs {
    CTL = 0x00,
    SWT = 0x04,
    DAC0_R12DH = 0x08,
    DAC0_L12DH = 0x0C,
    DAC0_R8DH  = 0x10,
    DAC1_R12DH = 0x14,
    DAC1_L12DH = 0x18,
    DAC1_R8DH  = 0x1C,
    DACC_R12DH = 0x20,
    DACC_L12DH = 0x24,
    DACC_R8DH  = 0x28,
    DAC0_DO = 0x2C,
    DAC1_DO = 0x30,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL_Bits {
    DEN0 = REG_BIT_DEF(0, 0),
    DBOFF0 = REG_BIT_DEF(1, 1),
    DTEN0 = REG_BIT_DEF(2, 2),
    DTSEL0 = REG_BIT_DEF(3, 5),
    DWM0 = REG_BIT_DEF(6, 7),
    DWBW0 = REG_BIT_DEF(8, 11),
    DDMAEN0 = REG_BIT_DEF(12, 12),
    DEN1 = REG_BIT_DEF(16, 16),
    DBOFF1 = REG_BIT_DEF(17, 17),
    DTEN1 = REG_BIT_DEF(18, 18),
    DTSEL1 = REG_BIT_DEF(19, 21),
    DWM1 = REG_BIT_DEF(22, 23),
    DWBW1 = REG_BIT_DEF(24, 27),
    DDMAEN1 = REG_BIT_DEF(28, 28),
};

enum class SWT_Bits {
    SWTR0 = REG_BIT_DEF(0, 0),
    SWTR1 = REG_BIT_DEF(1, 1),
};

enum class DAC0_R12DH_Bits {
    DAC0_DH = REG_BIT_DEF(0, 11),
};

enum class DAC0_L12DH_Bits {
    DAC0_DH = REG_BIT_DEF(4, 15),
};

enum class DAC0_R8DH_Bits {
    DAC0_DH = REG_BIT_DEF(0, 7),
};

enum class DAC1_R12DH_Bits {
    DAC1_DH = REG_BIT_DEF(0, 11),
};

enum class DAC1_L12DH_Bits {
    DAC1_DH = REG_BIT_DEF(4, 15),
};

enum class DAC1_R8DH_Bits {
    DAC1_DH = REG_BIT_DEF(0, 7),
};

enum class DACC_R12DH_Bits {
    DAC0_DH = REG_BIT_DEF(0, 11),
    DAC1_DH = REG_BIT_DEF(16, 27),
};

enum class DACC_L12DH_Bits {
    DAC0_DH = REG_BIT_DEF(4, 15),
    DAC1_DH = REG_BIT_DEF(20, 31),
};

enum class DACC_R8DH_Bits {
    DAC0_DH = REG_BIT_DEF(0, 7),
    DAC1_DH = REG_BIT_DEF(8, 15),
};

enum class DAC0_DO_Bits {
    DAC0_DO = REG_BIT_DEF(0, 11),
};

enum class DAC1_DO_Bits {
    DAC1_DO = REG_BIT_DEF(0, 11),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Internal_Device {
    DAC0,
    DAC1,
};

enum class Trigger_Source {
    TIMER5_TRIGGER,
    TIMER7_TRIGGER,
    TIMER6_TRIGGER,
    TIMER4_TRIGGER,
    TIMER1_TRIGGER,
    TIMER3_TRIGGER,
    EXTI_9_TRIGGER,
    SOFTWARE_TRIGGER,
};

enum class Wave_Type {
    DISABLE,
    LFSR,
    TRIANGLE,
};

enum class Bit_Width {
    WIDTH_1,
    WIDTH_2,
    WIDTH_3,
    WIDTH_4,
    WIDTH_5,
    WIDTH_6,
    WIDTH_7,
    WIDTH_8,
    WIDTH_9,
    WIDTH_10,
    WIDTH_11,
    WIDTH_12,
};

enum class LFSR_Noise {
    LFSR_BIT0,
    LFSR_BIT1_0,
    LFSR_BIT2_0,
    LFSR_BIT3_0,
    LFSR_BIT4_0,
    LFSR_BIT5_0,
    LFSR_BIT6_0,
    LFSR_BIT7_0,
    LFSR_BIT8_0,
    LFSR_BIT9_0,
    LFSR_BIT10_0,
    LFSR_BIT11_0,
};

enum class Data_Align {
    RIGHT_12B,
    LEFT_12B,
    RIGHT_8B,
};

enum class Triangle_Amplitude {
    AMP_1,
    AMP_3,
    AMP_7,
    AMP_15,
    AMP_31,
    AMP_63,
    AMP_127,
    AMP_255,
    AMP_511,
    AMP_1023,
    AMP_2047,
    AMP_4095,
};

} // namespace dac
