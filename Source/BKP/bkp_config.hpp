// gd32f30x BKP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"
#include "rcu_config.hpp"

namespace bkp {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class BKP_Regs {
    DATA0 = 0x04,
    DATA1 = 0x08,
    DATA2 = 0x0C,
    DATA3 = 0x10,
    DATA4 = 0x14,
    DATA5 = 0x18,
    DATA6 = 0x1C,
    DATA7 = 0x20,
    DATA8 = 0x24,
    DATA9 = 0x28,
    OCTL = 0x2C,
    TPCTL = 0x30,
    TPCS = 0x34,
    DATA10 = 0x40,
    DATA11 = 0x44,
    DATA12 = 0x48,
    DATA13 = 0x4C,
    DATA14 = 0x50,
    DATA15 = 0x54,
    DATA16 = 0x58,
    DATA17 = 0x5C,
    DATA18 = 0x60,
    DATA19 = 0x64,
    DATA20 = 0x68,
    DATA21 = 0x6C,
    DATA22 = 0x70,
    DATA23 = 0x74,
    DATA24 = 0x78,
    DATA25 = 0x7C,
    DATA26 = 0x80,
    DATA27 = 0x84,
    DATA28 = 0x88,
    DATA29 = 0x8C,
    DATA30 = 0x90,
    DATA31 = 0x94,
    DATA32 = 0x98,
    DATA33 = 0x9C,
    DATA34 = 0xA0,
    DATA35 = 0xA4,
    DATA36 = 0xA8,
    DATA37 = 0xAC,
    DATA38 = 0xB0,
    DATA39 = 0xB4,
    DATA40 = 0xB8,
    DATA41 = 0xBC,
};

enum class DATA_Bits {
    DATAX = REG_BIT_DEF(0, 15),
};

enum class OCTL_Bits {
    RCCV = REG_BIT_DEF(0, 6),
    COEN = REG_BIT_DEF(7, 7),
    ASOEN = REG_BIT_DEF(8, 8),
    ROSEL = REG_BIT_DEF(9, 9),
    CCOSEL = REG_BIT_DEF(14, 14),
    CALDIR = REG_BIT_DEF(15, 15),
};

enum class TPCTL_Bits {
    TPEN = REG_BIT_DEF(0, 0),
    TPAL = REG_BIT_DEF(1, 1),
};

enum class TPCS_Bits {
    TER = REG_BIT_DEF(0, 0),
    TIR = REG_BIT_DEF(1, 1),
    TPIE = REG_BIT_DEF(2, 2),
    TEF = REG_BIT_DEF(8, 8),
    TIF = REG_BIT_DEF(9, 9),
};

enum class Output_Pulse {
    ALARM_PULSE,
    SECOND_PULSE,
};

enum class Clock_Divider {
    DIV_64,
    DIV_1,
};

enum class Calibration_Type {
    SLOW_DOWN,
    SPEED_UP,
};

enum class Tamper_Level {
    ACTIVE_HIGH,
    ACTIVE_LOW = 2,
};

enum class Status_Flags {
    TAMPER_FLAG = REG_BIT_DEF(8, 8),
};

enum class Clear_Flags {
    TAMPER_FLAG_CLEAR = REG_BIT_DEF(0, 0),
    TAMPER_INTR_FLAG_CLEAR = REG_BIT_DEF(1, 1),
};

enum class Interrupt_Flags {
    TAMPER_INTR_FLAG = REG_BIT_DEF(9, 9),
};

enum class Backup_Data {
    DATA_0 = 1,
    DATA_1,
    DATA_2,
    DATA_3,
    DATA_4,
    DATA_5,
    DATA_6,
    DATA_7,
    DATA_8,
    DATA_9,
    DATA_10,
    DATA_11,
    DATA_12,
    DATA_13,
    DATA_14,
    DATA_15,
    DATA_16,
    DATA_17,
    DATA_18,
    DATA_19,
    DATA_20,
    DATA_21,
    DATA_22,
    DATA_23,
    DATA_24,
    DATA_25,
    DATA_26,
    DATA_27,
    DATA_28,
    DATA_29,
    DATA_30,
    DATA_31,
    DATA_32,
    DATA_33,
    DATA_34,
    DATA_35,
    DATA_36,
    DATA_37,
    DATA_38,
    DATA_39,
    DATA_40,
    DATA_41,
};


///////////////////////////// CONSTANTS /////////////////////////////

constexpr uint32_t OUTPUT_SECOND_PULSE = 0x200;
constexpr uint32_t CLOCK_DIV_1 = 0x4000;
constexpr uint32_t CLOCK_SPEED_UP = 0x8000;

} // namespace backup
