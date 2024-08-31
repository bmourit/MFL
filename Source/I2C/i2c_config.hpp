// gd32f30x I2C peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace i2c {


///////////////////////////// BASE ADDRESS /////////////////////////////

enum class I2C_Base {
    I2C0_BASE,
    I2C1_BASE,
};

static constexpr unsigned int I2C_baseAddress[] = {
    0x40005400, // I2C0
    0x40005800, // I2C1
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class I2C_Regs {
    CTL0 = 0x00,
    CTL1 = 0x04,
    SADDR0 = 0x08,
    SADDR1 = 0x0C,
    DATA = 0x10,
    STAT0 = 0x14,
    STAT1 = 0x18,
    CKCFG = 0x1C,
    RT = 0x20,
    FMPCFG = 0x90,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL0_Bits {
    I2CEN = REG_BIT_DEF(0, 0),
    SMBEN = REG_BIT_DEF(1, 1),
    SMBSEL = REG_BIT_DEF(2, 2),
    ARPEN = REG_BIT_DEF(3, 5),
    PECEN = REG_BIT_DEF(6, 6),
    GCEN = REG_BIT_DEF(7, 7),
    SS = REG_BIT_DEF(8, 8),
    START = REG_BIT_DEF(9, 9),
    STOP = REG_BIT_DEF(10, 10),
    ACKEN = REG_BIT_DEF(11, 11),
    POAP = REG_BIT_DEF(12, 12),
    PECTRANS = REG_BIT_DEF(13, 13),
    SALT = REG_BIT_DEF(14, 14),
    SRESET = REG_BIT_DEF(15, 15),
};

enum class CTL1_Bits {
    I2CCLK = REG_BIT_DEF(0, 6),
    ERRIE = REG_BIT_DEF(8, 8),
    EVIE = REG_BIT_DEF(9, 9),
    BUFIE = REG_BIT_DEF(10, 10),
    DMAON = REG_BIT_DEF(11, 11),
    DMALST = REG_BIT_DEF(12, 12),
};

enum class SADDR0_Bits {
    ADDRESS0 = REG_BIT_DEF(0, 0),
    ADDRESS = REG_BIT_DEF(1, 7),
    ADDRESS_H = REG_BIT_DEF(8, 9),
    ADDRESS_MASK = REG_BIT_DEF(0, 9),
    ADDFORMAT = REG_BIT_DEF(15, 16),
};

enum class SADDR1_Bits {
    DUADEN = REG_BIT_DEF(0, 0),
    ADDRESS2 = REG_BIT_DEF(1, 7),
};

enum class DATA_Bits {
    TRB = REG_BIT_DEF(0, 7),
};

enum class STAT0_Bits {
    SBSEND = REG_BIT_DEF(0, 0),
    ADDSEND = REG_BIT_DEF(1, 1),
    BTC = REG_BIT_DEF(2, 2),
    ADD10SEND = REG_BIT_DEF(3, 3),
    STPDET = REG_BIT_DEF(4, 4),
    RBNE = REG_BIT_DEF(6, 6),
    TBE = REG_BIT_DEF(7, 7),
    BERR = REG_BIT_DEF(8, 8),
    LOSTARB = REG_BIT_DEF(9, 9),
    AERR = REG_BIT_DEF(10, 10),
    OUERR = REG_BIT_DEF(11, 11),
    PECERR = REG_BIT_DEF(12, 12),
    SMBTO = REG_BIT_DEF(14, 14),
    SMBALT = REG_BIT_DEF(15, 15),
};

enum class STAT1_Bits {
    MASTER = REG_BIT_DEF(0, 0),
    I2CBSY = REG_BIT_DEF(1, 1),
    TR = REG_BIT_DEF(2, 2),
    RXGC = REG_BIT_DEF(4, 4),
    DEFSMB = REG_BIT_DEF(5, 5),
    HSTSMB = REG_BIT_DEF(6, 6),
    DUMODF = REG_BIT_DEF(7, 7),
    PECV = REG_BIT_DEF(8, 15),
};

enum class CKCFG_Bits {
    CLKC = REG_BIT_DEF(0, 11),
    DTCY = REG_BIT_DEF(14, 14),
    FAST = REG_BIT_DEF(15, 15),
};

enum class RT_Bits {
    RISETIME = REG_BIT_DEF(0, 6),
};

enum class FMPCFG_Bits {
    FMPEN = REG_BIT_DEF(0, 0),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Status_Flags {
    FLAG_SBSEND,
    FLAG_ADDSEND,
    FLAG_BTC,
    FLAG_ADD10SEND,
    FLAG_STPDET,
    FLAG_RBNE,
    FLAG_TBE,
    FLAG_BERR,
    FLAG_LOSTARB,
    FLAG_AERR,
    FLAG_OUERR,
    FLAG_PECERR,
    FLAG_SMBTO,
    FLAG_SMBALT,
    FLAG_MASTER,
    FLAG_I2CBSY,
    FLAG_TRS,
    FLAG_RXGC,
    FLAG_DEFSMB,
    FLAG_HSTSMB,
    FLAG_DUMOD,
};

struct index_to_bits {
    I2C_Regs register_offset;
    uint32_t bit_info;
};

static const index_to_bits status_flag_index[] = {
    {I2C_Regs::STAT0, REG_BIT_DEF(0, 0)},
    {I2C_Regs::STAT0, REG_BIT_DEF(1, 1)},
    {I2C_Regs::STAT0, REG_BIT_DEF(2, 2)},
    {I2C_Regs::STAT0, REG_BIT_DEF(3, 3)},
    {I2C_Regs::STAT0, REG_BIT_DEF(4, 4)},
    {I2C_Regs::STAT0, REG_BIT_DEF(6, 6)},
    {I2C_Regs::STAT0, REG_BIT_DEF(7, 7)},
    {I2C_Regs::STAT0, REG_BIT_DEF(8, 8)},
    {I2C_Regs::STAT0, REG_BIT_DEF(9, 9)},
    {I2C_Regs::STAT0, REG_BIT_DEF(10, 10)},
    {I2C_Regs::STAT0, REG_BIT_DEF(11, 11)},
    {I2C_Regs::STAT0, REG_BIT_DEF(12, 12)},
    {I2C_Regs::STAT0, REG_BIT_DEF(14, 14)},
    {I2C_Regs::STAT0, REG_BIT_DEF(15, 15)},
    {I2C_Regs::STAT1, REG_BIT_DEF(0, 0)},
    {I2C_Regs::STAT1, REG_BIT_DEF(1, 1)},
    {I2C_Regs::STAT1, REG_BIT_DEF(2, 2)},
    {I2C_Regs::STAT1, REG_BIT_DEF(4, 4)},
    {I2C_Regs::STAT1, REG_BIT_DEF(5, 5)},
    {I2C_Regs::STAT1, REG_BIT_DEF(6, 6)},
    {I2C_Regs::STAT1, REG_BIT_DEF(7, 7)},
};

enum class Clear_Flags {
    FLAG_ADDSEND,
    FLAG_BERR,
    FLAG_LOSTARB,
    FLAG_AERR,
    FLAG_OUERR,
    FLAG_PECERR,
    FLAG_SMBTO,
    FLAG_SMBALT,
};

static const index_to_bits clear_flag_index[] = {
    {I2C_Regs::STAT0, REG_BIT_DEF(1, 1)},
    {I2C_Regs::STAT0, REG_BIT_DEF(8, 8)},
    {I2C_Regs::STAT0, REG_BIT_DEF(9, 9)},
    {I2C_Regs::STAT0, REG_BIT_DEF(10, 10)},
    {I2C_Regs::STAT0, REG_BIT_DEF(11, 11)},
    {I2C_Regs::STAT0, REG_BIT_DEF(12, 12)},
    {I2C_Regs::STAT0, REG_BIT_DEF(14, 14)},
    {I2C_Regs::STAT0, REG_BIT_DEF(15, 15)},
};

enum class Interrupt_Flags {
    INTR_FLAG_SBSEND,
    INTR_FLAG_ADDSEND,
    INTR_FLAG_BTC,
    INTR_FLAG_ADD10SEND,
    INTR_FLAG_STPDET,
    INTR_FLAG_RBNE,
    INTR_FLAG_TBE,
    INTR_FLAG_BERR,
    INTR_FLAG_LOSTARB,
    INTR_FLAG_AERR,
    INTR_FLAG_OUERR,
    INTR_FLAG_PECERR,
    INTR_FLAG_SMBTO,
    INTR_FLAG_SMBALT,
};

struct double_index_to_bits {
    I2C_Regs register0_offset;
    uint32_t bit_info0;
    I2C_Regs register1_offset;
    uint32_t bit_info1;
};

static const double_index_to_bits interrupt_flag_index[] = {
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(0, 0)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(1, 1)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(2, 2)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(3, 3)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(4, 4)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(6, 6)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9), I2C_Regs::STAT0, REG_BIT_DEF(7, 7)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(8, 8)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(9, 9)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(10, 10)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(11, 11)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(12, 12)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(14, 14)},
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8), I2C_Regs::STAT0, REG_BIT_DEF(15, 15)},
};

enum class Interrupt_Type {
    INTR_ERR,
    INTR_EV,
    INTR_BUF,
};

static const index_to_bits interrupt_type_index[] = {
    {I2C_Regs::CTL1, REG_BIT_DEF(8, 8)},
    {I2C_Regs::CTL1, REG_BIT_DEF(9, 9)},
    {I2C_Regs::CTL1, REG_BIT_DEF(10, 10)},
};

enum class Bus_Mode {
    I2C,
    SMBUS,
};

enum class Bus_Type {
    DEVICE,
    HOST,
};

enum class Transfer_Direction {
    RECEIVE = 1,
    TRANSMIT = 2,
};

enum class ACK_Select {
    CURRENT,
    NEXT,
};

enum class Stretch_Low {
    SCLSTRETCH_ENABLE,
    SCLSTRETCH_DISABLE,
};

enum class Duty_Cycle {
    DTCY_2,
    DTCY_16_9,
};

enum class Address_Format {
    FORMAT_7BITS,
    FORMAT_10BITS,
};

enum class I2C_Error_Type {
    OK = 0,
    INVALID_I2C,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
    INVALID_CLOCK_FREQUENCY,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct I2C_Clock_Config {
    rcu::RCU_PCLK clock_reg;
    rcu::RCU_PCLK_Reset reset_reg;
};

static const I2C_Clock_Config I2C_pclk_index[] = {
    {rcu::RCU_PCLK::PCLK_I2C0, rcu::RCU_PCLK_Reset::PCLK_I2C0RST},
    {rcu::RCU_PCLK::PCLK_I2C1, rcu::RCU_PCLK_Reset::PCLK_I2C1RST},
};

struct I2C_PinOps {
    gpio::GPIO_Base gpio_port;
    gpio::Pin_Number pin;
    gpio::Pin_Mode mode;
    gpio::Output_Speed speed;
};

struct I2C_Config {
    I2C_PinOps sda_pin;
    I2C_PinOps scl_pin;
};


///////////////////////////// CONSTANTS /////////////////////////////

constexpr uint32_t MaximumClockSpeed = 60;
constexpr uint32_t MinimumClockSpeed = 2;
constexpr uint32_t AddressMask = 0x000003FF;
constexpr uint32_t Address2Mask = 0x000000FE;

} // namespace i2c
