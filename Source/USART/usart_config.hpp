// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"
#include "RCU.hpp"

namespace usart {


///////////////////////////// BASE ADDRESS /////////////////////////////

enum class USART_Base {
    USART0_BASE,
    USART1_BASE,
    USART2_BASE,
    UART3_BASE,
    UART4_BASE,
};

static constexpr uint32_t USART_baseAddress[] = {
    0x40013800,	// USART0
    0x40004400, // USART1
    0x40004800, // USART2
    0x40004C00, // UART3
    0x40005000, // UART4
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class USART_Regs {
    STAT0 = 0x00,
    DATA = 0x04,
    BAUD = 0x08,
    CTL0 = 0x0C,
    CTL1 = 0x10,
    CTL2 = 0x14,
    GP = 0x18,
    CTL3 = 0x80,
    RT = 0x84,
    STAT1 = 0x88,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class STAT0_Bits {
    PERR = REG_BIT_DEF(0, 0),
    FERR = REG_BIT_DEF(1, 1),
    NERR = REG_BIT_DEF(2, 2),
    ORERR = REG_BIT_DEF(3, 3),
    IDLEF = REG_BIT_DEF(4, 4),
    RBNE = REG_BIT_DEF(5, 5),
    TC = REG_BIT_DEF(6, 6),
    TBE = REG_BIT_DEF(7, 7),
    LBDF = REG_BIT_DEF(8, 8),
    CTSF = REG_BIT_DEF(9, 9),
};

enum class DATA_Bits {
    DATA = REG_BIT_DEF(0, 8),
};

enum class BAUD_Bits {
    FRADIV = REG_BIT_DEF(0, 3),
    INTDIV = REG_BIT_DEF(4, 15),
    BAUDRATE = REG_BIT_DEF(0, 15),	// simplifies writing baudrate
};

enum class CTL0_Bits {
    SBKCMD = REG_BIT_DEF(0, 0),
    RWU = REG_BIT_DEF(1, 1),
    REN = REG_BIT_DEF(2, 2),
    TEN = REG_BIT_DEF(3, 3),
    IDLEIE = REG_BIT_DEF(4, 4),
    RBNEIE = REG_BIT_DEF(5, 5),
    TCIE = REG_BIT_DEF(6, 6),
    TBEIE = REG_BIT_DEF(7, 7),
    PERRIE = REG_BIT_DEF(8, 8),
    PM = REG_BIT_DEF(9, 9),
    PCEN = REG_BIT_DEF(10, 10),
    PMEN = REG_BIT_DEF(9, 10),	// convenience
    WM = REG_BIT_DEF(11, 11),
    WL = REG_BIT_DEF(12, 12),
    UEN = REG_BIT_DEF(13, 13),
};

enum class CTL1_Bits {
    ADDR = REG_BIT_DEF(0, 3),
    LBLEN = REG_BIT_DEF(5, 5),
    LBDIE = REG_BIT_DEF(6, 6),
    CLEN = REG_BIT_DEF(8, 8),
    CPH = REG_BIT_DEF(9, 9),
    CPL = REG_BIT_DEF(10, 10),
    CKEN = REG_BIT_DEF(11, 11),
    STB = REG_BIT_DEF(12, 13),
    LMEN = REG_BIT_DEF(14, 14),
};

enum class CTL2_Bits {
    ERRIE = REG_BIT_DEF(0, 0),
    IREN = REG_BIT_DEF(1, 1),
    IRLP = REG_BIT_DEF(2, 2),
    HDEN = REG_BIT_DEF(3, 3),
    NKEN = REG_BIT_DEF(4, 4),
    SCEN = REG_BIT_DEF(5, 5),
    DENR = REG_BIT_DEF(6, 6),
    DENT = REG_BIT_DEF(7, 7),
    RTSEN = REG_BIT_DEF(8, 8),
    CTSEN = REG_BIT_DEF(9, 9),
    CTSIE = REG_BIT_DEF(10, 10),
};

enum class GP_Bits {
    PSC = REG_BIT_DEF(0, 7),
    GUAT = REG_BIT_DEF(8, 15),
};

enum class CTL3_Bits {
    RTEN = REG_BIT_DEF(0, 0),
    SCRTNUM = REG_BIT_DEF(1, 3),
    RTIE = REG_BIT_DEF(4, 4),
    EBIE = REG_BIT_DEF(5, 5),
    RINV = REG_BIT_DEF(8, 8),
    TINV = REG_BIT_DEF(9, 9),
    DINV = REG_BIT_DEF(10, 10),
    MSBF = REG_BIT_DEF(11, 11),
};

enum class RT_Bits {
    RT = REG_BIT_DEF(0, 23),
    BL = REG_BIT_DEF(24, 31),
};

enum class STAT1_Bits {
    RTF = REG_BIT_DEF(11, 11),
    EBF = REG_BIT_DEF(12, 12),
    BSY = REG_BIT_DEF(18, 18),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Direction_Mode {
    RX_MODE,
    TX_MODE,
    RXTX_MODE,
    RXTX_OFF,
};

enum class Parity_Mode {
    PM_NONE,
    PM_EVEN,
    PM_ODD,
};

enum class Wakeup_Mode {
    WM_IDLE,
    WM_ADDR,
};

enum class Word_Length {
    WL_8BITS,
    WL_9BITS,
};

enum class Break_Length {
    LENGTH_10B,
    LENGTH_11B,
};

enum class Pulse_Length {
    EXT_PULSE_DISABLE,
    EXT_PULSE_ENABLE,
};

enum class Clock_Phase {
    FIRST_CLOCK,
    SECOND_CLOCK,
};

enum class Clock_Polarity {
    POLARITY_LOW,
    POLARITY_HIGH,
};

enum class Stop_Bits {
    STB_1BIT,
    STB_0_5BIT,
    STB_2BIT,
    STB_1_5BIT,
};

enum class MSBF_Mode {
    MSBF_LSB,
    MSBF_MSB,
};

enum class Inversion_Method {
    DATA,
    TRANSMISSION,
    RECEPTION,
};

enum class IrDA_Power {
    NORMAL,
    LOW,
};

enum class Status_Flags {
    FLAG_PERR,	// STAT0, PERR
    FLAG_FERR,	// STAT0, FERR
    FLAG_NERR,	// STAT0, NERR
    FLAG_ORERR,	// STAT0, ORERR
    FLAG_IDLEF,	// STAT0, IDLEF
    FLAG_RBNE,	// STAT0, RBNE
    FLAG_TC,	// STAT0, TC
    FLAG_TBE,	// STAT0, TBE
    FLAG_LBDF,	// STAT0, LBDF
    FLAG_CTSF,	// STAT0, CTSF
    FLAG_RTF,	// STAT1, RTF
    FLAG_EBF,	// STAT1, EBF
    FLAG_BSY,	// STAT1, BSY
};

enum class Interrupt_Flags {
    INTR_FLAG_CTL0_PERR,    // STAT0, PERR CTL0, PERRIE
    INTR_FLAG_CTL0_TBE,		// STAT0, TBE CTL0, TBEIE
    INTR_FLAG_CTL0_TC,		// STAT0, TC CTL0, TCIE
    INTR_FLAG_CTL0_RBNE,    // STAT0, RBNE CTL0, RBNEIE
    INTR_FLAG_CTL0_ORERR,	// STAT0, ORERR CTL0, RBNEIE
    INTR_FLAG_CTL0_IDLEF,	// STAT0, IDLEF CTL0, IDLEIE
    INTR_FLAG_CTL1_LBDF,    // STAT0, LBDF CTL1, LBDIE
    INTR_FLAG_CTL2_CTSF,    // STAT0, CTSF CTL2, CTSIE
    INTR_FLAG_CTL2_ORERR,	// STAT0, ORERR CTL2, ERRIE
    INTR_FLAG_CTL2_NERR,    // STAT0, FERR CTL2, ERRIE
    INTR_FLAG_CTL2_FERR,    // STAT0, NERR CTL2, ERRIE
    INTR_FLAG_CTL3_EBF,		// STAT1, EBF CTL3, EBIE
    INTR_FLAG_CTL3_RTF,		// STAT1, RTF CTL3, RTIE

};

enum class Interrupt_Type {
    INTR_PERRIE,	// CTL0
    INTR_TBEIE,		// CTL0
    INTR_TCIE,		// CTL0
    INTR_RBNEIE,	// CTL0
    INTR_IDLEIE,	// CTL0
    INTR_LBDIE,		// CTL1
    INTR_CTSIE,		// CTL2
    INTR_ERRIE,		// CTL2
    INTR_EBIE,		// CTL3
    INTR_RTIE,		// CTL3
};

enum class Inversion_Type {
    DINV_ENABLE,
    DINV_DISABLE,
    TXPIN_ENABLE,
    TXPIN_DISABLE,
    RXPIN_ENABLE,
    RXPIN_DISABLE,
};

enum class USART_DMA_Config {
    DMA_NONE,
    DMA_TX,
    DMA_RX,
    DMA_DUAL,
};

enum class USART_Error_Type {
    OK = 0,
    INVALID_USART,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
    USART_INSTANCE_FAILED,
    PARITY_ERROR,
    NOISE_ERROR,
    FRAME_ERROR,
    DMA_TRANSFER_ERRROR,
    TIMEOUT,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct USART_Clock_Config {
    rcu::RCU_PCLK clock_reg;
    rcu::RCU_PCLK_Reset reset_reg;
};

static const USART_Clock_Config USART_pclk_index[] = {
    {rcu::RCU_PCLK::PCLK_USART0, rcu::RCU_PCLK_Reset::PCLK_USART0RST},
    {rcu::RCU_PCLK::PCLK_USART1, rcu::RCU_PCLK_Reset::PCLK_USART1RST},
    {rcu::RCU_PCLK::PCLK_USART2, rcu::RCU_PCLK_Reset::PCLK_USART2RST},
    {rcu::RCU_PCLK::PCLK_UART3, rcu::RCU_PCLK_Reset::PCLK_UART3RST},
    {rcu::RCU_PCLK::PCLK_UART4, rcu::RCU_PCLK_Reset::PCLK_UART4RST},
};

struct USART_sc_params {
    uint8_t guat;
    uint8_t sc_auto_retry;
    uint8_t sc_block_length;
};

struct USART_sync_params {
    uint32_t clen;
    uint32_t cph;
    uint32_t cpl;
};

struct USART_irda_params {
    uint8_t prescaler;
    uint32_t power_level;

};

struct USART_Pin_Config {
    gpio::GPIO_Base gpio_port;
    gpio::Pin_Number pin;
    gpio::Pin_Mode mode;
    gpio::Output_Speed speed;
};

struct USART_Config {
    USART_Pin_Config rx_pin_config; // Pass the expected config for rx gpio pin setup
    USART_Pin_Config tx_pin_config; // Pass the expected config for tx gpio pin setup
    USART_DMA_Config dma_pin_ops;   // DMA pin usage flag: off, RX pin, TX pin, or both pins
    uint32_t baudrate;              // Must never be 0
    Parity_Mode parity;
    Word_Length word_length;
    Stop_Bits stop_bits;
    MSBF_Mode msbf;
    Direction_Mode direction;
};

} // namespace usart
