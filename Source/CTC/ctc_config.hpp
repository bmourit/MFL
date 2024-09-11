// gd32f30x CTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"
#include "rcu_config.hpp"

namespace ctc {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class CTC_Regs {
    CTL0 = 0x00,
    CTL1 = 0x04,
    STAT = 0x08,
    INTC = 0x0C,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL0_Bits {
    CKOKIE = REG_BIT_DEF(0, 0),
    CKWARNIE = REG_BIT_DEF(1, 1),
    ERRIE = REG_BIT_DEF(2, 2),
    EREFIE = REG_BIT_DEF(3, 3),
    CNTEN = REG_BIT_DEF(5, 5),
    AUTOTRIM = REG_BIT_DEF(6, 6),
    SWREFPUL = REG_BIT_DEF(7, 7),
    TRIMVALUE = REG_BIT_DEF(8, 13),
};

enum class CTL1_Bits {
    RLVALUE = REG_BIT_DEF(0, 15),
    CKLIM  = REG_BIT_DEF(16, 23),
    REFPSC = REG_BIT_DEF(24, 26),
    REFSEL = REG_BIT_DEF(28, 29),
    REFPOL = REG_BIT_DEF(31, 31),
};

enum class STAT_Bits {
    CKOKIF = REG_BIT_DEF(0, 0),
    CKWARNIF = REG_BIT_DEF(1, 1),
    ERRIF = REG_BIT_DEF(2, 2),
    EREFIF = REG_BIT_DEF(3, 3),
    CKERR = REG_BIT_DEF(8, 8),
    REFMISS = REG_BIT_DEF(9, 9),
    TRIMERR = REG_BIT_DEF(10, 10),
    REFDIR = REG_BIT_DEF(15, 15),
    REFCAP = REG_BIT_DEF(16, 31),
};

enum class INTC_Bits {
    CKOKIC = REG_BIT_DEF(0, 0),
    CKWARNIC = REG_BIT_DEF(1, 1),
    ERRIC = REG_BIT_DEF(2, 2),
    EREFIC = REG_BIT_DEF(3, 3),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Reference_Polarity {
    RISING,
    FALLING,
};

enum class Reference_Select {
    GPIO,
    LXTAL,
};

enum class Reference_Prescaler {
    OFF,
    DIV2,
    DIV4,
    DIV8,
    DIB16,
    DIV32,
    DIV64,
    DIV128,
};

enum class Status_Flags {
    FLAG_CKOK = REG_BIT_DEF(0, 0),
    FLAG_CKWARN = REG_BIT_DEF(1, 1),
    FLAG_ERR = REG_BIT_DEF(2, 2),
    FLAG_EREF = REG_BIT_DEF(3, 3),
    FLAG_CKERR = REG_BIT_DEF(8, 8),
    FLAG_REFMISS = REG_BIT_DEF(9, 9),
    FLAG_TRIMERR = REG_BIT_DEF(10, 10),
};

enum class Clear_Flags {
    INTC_CKOKIC = REG_BIT_DEF(0, 0),
    INTC_CKWARNIC = REG_BIT_DEF(1, 1),
    INTC_ERRIC = REG_BIT_DEF(2, 2),
    INTC_EREFIC = REG_BIT_DEF(3, 3),
};

enum class Interrupt_Flags {
    INTR_FLAG_CKOK = REG_BIT_DEF(0, 0),
    INTR_FLAG_CKWARN = REG_BIT_DEF(1, 1),
    INTR_FLAG_ERR = REG_BIT_DEF(2, 2),
    INTR_FLAG_EREF = REG_BIT_DEF(3, 3),
    INTR_FLAG_CKERR = REG_BIT_DEF(8, 8),
    INTR_FLAG_REFMISS = REG_BIT_DEF(9, 9),
    INTR_FLAG_TRIMERR = REG_BIT_DEF(10, 10),
    INTR_ERRIC_FLAG_MASK = REG_BIT_DEF(8, 10),
};

enum class Interrupt_Type {
    INTR_CKOK = REG_BIT_DEF(0, 0),
    INTR_CKWARN = REG_BIT_DEF(1, 1),
    INTR_ERR = REG_BIT_DEF(2, 2),
    INTR_EREF = REG_BIT_DEF(3, 3)
};

} // namespace ctc
