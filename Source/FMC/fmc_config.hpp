// gd32f30x FMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include "CONFIG.hpp"

namespace fmc {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class FMC_Regs {
    WS = 0x00,
    KEY0 = 0x04,
    OBKEY = 0x08,
    STAT0 = 0x0C,
    CTL0 = 0x10,
    ADDR0 = 0x14,
    OBSTAT = 0x1C,
    WP = 0x20,
    KEY1 = 0x44,
    STAT1 = 0x4C,
    CTL1 = 0x50,
    ADDR1 = 0x54,
    WSEN = 0xFC,
    PID = 0x100
};

enum class OB_Regs {
    SPC = 0x00,
    USER = 0x02,
    DATA1 = 0x04,
    DATA2 = 0x06,
    WP0 = 0x08,
    WP1 = 0x0A,
    WP2 = 0x0C,
    WP3 = 0x0E
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class WS_Bits {
    WSCNT = REG_BIT_DEF(0, 2)
};

enum class STAT0_Bits {
    BUSY = REG_BIT_DEF(0, 0),
    PGERR = REG_BIT_DEF(2, 2),
    WPERR = REG_BIT_DEF(4, 4),
    ENDF = REG_BIT_DEF(5, 5),
};

enum class CTL0_Bits {
    PG = REG_BIT_DEF(0, 0),
    PER = REG_BIT_DEF(1, 1),
    MER = REG_BIT_DEF(2, 2),
    OBPG = REG_BIT_DEF(4, 4),
    OBER = REG_BIT_DEF(5, 5),
    START = REG_BIT_DEF(6, 6),
    LK = REG_BIT_DEF(7, 7),
    OBWEN = REG_BIT_DEF(9, 9),
    ERRIE = REG_BIT_DEF(10, 10),
    ENDIE = REG_BIT_DEF(12, 12),
};

enum class OBSTAT_Bits {
    OBERR = REG_BIT_DEF(0, 0),
    SPC = REG_BIT_DEF(1, 1),
    USER = REG_BIT_DEF(2, 9),
    DATA = REG_BIT_DEF(10, 25),
};

enum class STAT1_Bits {
    BUSY = REG_BIT_DEF(0, 0),
    PGERR = REG_BIT_DEF(2, 2),
    WPERR = REG_BIT_DEF(4, 4),
    ENDF = REG_BIT_DEF(5, 5),
};

enum class CTL1_Bits {
    PG = REG_BIT_DEF(0, 0),
    PER = REG_BIT_DEF(1, 1),
    MER = REG_BIT_DEF(2, 2),
    START = REG_BIT_DEF(6, 6),
    LK = REG_BIT_DEF(7, 7),
    ERRIE = REG_BIT_DEF(10, 10),
    ENDIE = REG_BIT_DEF(12, 12),
};

enum class WSEN_Bits {
    WSEN = REG_BIT_DEF(0, 0),
    BPEN = REG_BIT_DEF(1, 0)
};

enum class SPC_Bits {
    SPC = REG_BIT_DEF(0, 7),
    SPC_N = REG_BIT_DEF(8, 15)
};

enum class USER_Bits {
    USER = REG_BIT_DEF(16, 23),
    USER_N = REG_BIT_DEF(24, 31)
};

enum class WPX_Bits {
    WP0 = REG_BIT_DEF(0, 7),
    WP1 = REG_BIT_DEF(8, 15),
    WP2 = REG_BIT_DEF(16, 23),
    WP3 = REG_BIT_DEF(24, 31)
};


///////////////////////////// ENUMS /////////////////////////////

enum class Interrupt_Types {
    INTR_BANK0_END,		// CTL0 ENDIE
    INTR_BANK0_ERR,		// CTL0 ERRIE
    INTR_BANK1_END,		// CTL1 ENDIE
    INTR_BANK1_ERR		// CTL1 ERRIE
};

enum class Flag_Types {
    FLAG_BANK0_BUSY,	// STAT0 BUSY
    FLAG_BANK0_PGERR, 	// STAT0 PGERR
    FLAG_BANK0_WPERR,	// STAT0 WPERR
    FLAG_BANK0_END,		// STAT0 ENDF
    FLAG_OBERR,			// OBSTAT OBERR
    FLAG_BANK1_BUSY,	// STAT1 BUSY
    FLAG_BANK1_PGERR,	// STAT1 PGERR
    FLAG_BANK1_WPERR,	// STAT1 WPERR
    FLAG_BANK1_END    	// STAT1 ENDF
};

enum class Interrupt_Flags {
    INTR_FLAG_BANK0_PGERR,  // FMC_STAT0 PGERR, CTL0 ERRIE
    INTR_FLAG_BANK0_WPERR,  // FMC_STAT0 WPERR, CTL0 ERRIE
    INTR_FLAG_BANK0_END,    // FMC_STAT0 ENDF, CTL0 ENDIE
    INTR_FLAG_BANK1_PGERR,  // FMC_STAT1 PGERR, CTL1 ERRIE
    INTR_FLAG_BANK1_WPERR,  // FMC_STAT1 WPERR, CTL1 ERRIE
    INTR_FLAG_BANK1_END,    // FMC_STAT1 ENDF, CTL1 ENDIE
};

enum class Wait_State {
    WS_WSCNT_0,
    WS_WSCNT_1,
    WS_WSCNT_2
};

enum class OB_Watchdog_Type {
    FWDGT_HW = 0x00,
    FWDGT_SW = 0x01
};

enum class OB_Deep_Sleep {
    DEEPSLEEP_RESET = 0x00,
    DEEPSLEEP_NO_RESET = 0x02,
};

enum class OB_Standby {
    STANDBY_NO_RESET = 0x04,
    STABDBY_RESET = 0x00
};

enum class OB_Boot_Bank {
    BOOT_BANK1 = 0x00,
    BOOT_BANK0 = 0x08
};

enum class OB_User_Mask {
    USER_MASK = 0xF0
};

enum class OB_Security_Type {
    OB_NSPC = 0xA5,		// No protection
    OB_USPC = 0xBB		// Under protection
};

enum class WP_Sector {
    OB_WP_0 = (1 << 0),
    OB_WP_1 = (1 << 1),
    OB_WP_2 = (1 << 2),
    OB_WP_3 = (1 << 3),
    OB_WP_4 = (1 << 4),
    OB_WP_5 = (1 << 5),
    OB_WP_6 = (1 << 6),
    OB_WP_7 = (1 << 7),
    OB_WP_8 = (1 << 8),
    OB_WP_9 = (1 << 9),
    OB_WP_10 = (1 << 10),
    OB_WP_11 = (1 << 11),
    OB_WP_12 = (1 << 12),
    OB_WP_13 = (1 << 13),
    OB_WP_14 = (1 << 14),
    OB_WP_15 = (1 << 15),
    OB_WP_16 = (1 << 16),
    OB_WP_17 = (1 << 17),
    OB_WP_18 = (1 << 18),
    OB_WP_19 = (1 << 19),
    OB_WP_20 = (1 << 20),
    OB_WP_21 = (1 << 21),
    OB_WP_22 = (1 << 22),
    OB_WP_23 = (1 << 23),
    OB_WP_24 = (1 << 24),
    OB_WP_25 = (1 << 25),
    OB_WP_26 = (1 << 26),
    OB_WP_27 = (1 << 27),
    OB_WP_28 = (1 << 28),
    OB_WP_29 = (1 << 29),
    OB_WP_30 = (1 << 30),
    OB_WP_31 = (1 << 31),
    //OB_WP_ALL = 0xFFFFFFFF,
};

enum class FMC_State {
    READY,
    BUSY,
    PG_ERROR,
    WP_ERROR,
    TIMEOUT,
};


///////////////////////////// CONSTANTS /////////////////////////////

// Keys
static const unsigned int UNLOCK_KEY0 = 0x45670123;
static const unsigned int UNLOCK_KEY1 = 0xCDEF89AB;

// Timeout
static const unsigned int FMC_TIMEOUT_COUNT = 0x0FFF0000;

// Bank
static const unsigned int BANK0_END_ADDRESS = 0x0807FFFF;
static const unsigned int BANK0_SIZE = 0x00000200;
static const volatile uint16_t *const MAX_SIZE = reinterpret_cast<const volatile uint16_t*>(0x1FFFF7E0U);

} // namespace fmc
