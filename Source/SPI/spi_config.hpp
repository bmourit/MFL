// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace spi {


///////////////////////////// BASE ADDRESS /////////////////////////////

enum class SPI_Base {
    SPI0_BASE,
    SPI1_BASE,
    SPI2_BASE,
};

static constexpr unsigned int SPI_baseAddress[] = {
    0x40013000, // SPI0
    0x40003800, // SPI1
    0x40003C00, // SPI2
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class SPI_Regs {
    CTL0 = 0x00,
    CTL1 = 0x04,
    STAT = 0x08,
    DATA = 0x0C,
    CRCPOLY = 0x10,
    RCRC = 0x14,
    TCRC = 0x18,
    I2SCTL = 0x1C,
    I2SPSC = 0x20,
    QCTL = 0x80,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL0_Bits {
    CKPH = REG_BIT_DEF(0, 0),
    CKPL = REG_BIT_DEF(1, 1),
    MSTMOD = REG_BIT_DEF(2, 2),
    PSC = REG_BIT_DEF(3, 5),
    SPIEN = REG_BIT_DEF(6, 6),
    LF = REG_BIT_DEF(7, 7),
    SWNSS = REG_BIT_DEF(8, 8),
    SWNSSEN = REG_BIT_DEF(9, 9),
    RO = REG_BIT_DEF(10, 10),
    FF16 = REG_BIT_DEF(11, 11),
    CRCNT = REG_BIT_DEF(12, 12),
    CRCEN = REG_BIT_DEF(13, 13),
    BDOEN = REG_BIT_DEF(14, 14),
    BDEN = REG_BIT_DEF(15, 15),
};

enum class CTL1_Bits {
    DMAREN = REG_BIT_DEF(0, 0),
    DMATEN = REG_BIT_DEF(1, 1),
    NSSDRV = REG_BIT_DEF(2, 2),
    NSSP = REG_BIT_DEF(3, 3),
    TMOD = REG_BIT_DEF(4, 4),
    ERRIE = REG_BIT_DEF(5, 5),
    RBNEIE = REG_BIT_DEF(6, 6),
    TBEIE = REG_BIT_DEF(7, 7),
};

enum class STAT_Bits {
    RBNE = REG_BIT_DEF(0, 0),
    TBE = REG_BIT_DEF(1, 1),
    I2SCH = REG_BIT_DEF(2, 2),
    TXURERR = REG_BIT_DEF(3, 3),
    CRCERR = REG_BIT_DEF(4, 4),
    CONFERR = REG_BIT_DEF(5, 5),
    RXORERR = REG_BIT_DEF(6, 6),
    TRANS = REG_BIT_DEF(7, 7),
    FERR = REG_BIT_DEF(8, 8),
};

enum class SPI_16Bit {
    MASK_16BIT = REG_BIT_DEF(0, 15),
};

enum class I2SCTL_Bits {
    CHLEN = REG_BIT_DEF(0, 0),
    DTLEN = REG_BIT_DEF(1, 2),
    CKPL = REG_BIT_DEF(3, 3),
    I2SSTD = REG_BIT_DEF(4, 5),
    PCMSMOD = REG_BIT_DEF(7, 7),
    I2SOPMOD = REG_BIT_DEF(8, 9),
    I2SEN = REG_BIT_DEF(10, 10),
    I2SSEL = REG_BIT_DEF(11, 11),
};

enum class I2SPSC_Bits {
    DIV = REG_BIT_DEF(0, 7),
    OF = REG_BIT_DEF(8, 8),
    MCKOEN = REG_BIT_DEF(9, 9),
};

enum class QCTL_Bits {
    QMOD = REG_BIT_DEF(0, 0),
    QRD = REG_BIT_DEF(1, 1),
    IO23_DRV = REG_BIT_DEF(2, 2),
};

// Alias
enum class Status_Flags {
    FLAG_RBNE = REG_BIT_DEF(0, 0),
    FLAG_TBE = REG_BIT_DEF(1, 1),
    FLAG_I2SCH = REG_BIT_DEF(2, 2),
    FLAG_TXURERR = REG_BIT_DEF(3, 3),
    FLAG_CRCERR = REG_BIT_DEF(4, 4),
    FLAG_CONFERR = REG_BIT_DEF(5, 5),
    FLAG_RXORERR = REG_BIT_DEF(6, 6),
    FLAG_TRANS = REG_BIT_DEF(7, 7),
    FLAG_FERR = REG_BIT_DEF(8, 8),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Mode_Select {
    MASTER,
    SLAVE,
};

enum class Direction_Mode {
    BIDIRECTIONAL_RECEIVE,
    BIDIRECTIONAL_TRANSMIT,
};

enum class Operational_Mode {
    MFD_MODE,	// Master Full-Duplex
    MTU_MODE,	// Master Transmission Unidirection
    MRU_MODE,	// Master Reception Unidirectional
    MTB_MODE,	// Master Transmission Bidirectional
    MRB_MODE,	// Master Reception Bidirectional
    SFD_MODE,	// Slave Full-Duplex
    STU_MODE,	// Slave Transmission Unidirection
    SRU_MODE,	// Slave Reception Unidirectional
    STB_MODE,	// Slave Transmission Bidirectional
    SRB_MODE,	// SLAVE Reception Bidirectional
};

enum class Frame_Format {
    FF_8BIT,
    FF_16BIT,
};

enum class NSS_Type {
    HARDWARE_NSS,
    SOFTWARE_NSS,
};

enum class Endian_Type {
    MSBF,
    LSBF,
};

enum class Clock_Polarity {
    PULL_LOW,
    PULL_HIGH,
};

enum class Clock_Phase {
    PHASE_FIRST_EDGE,
    PHASE_SECOND_EDGE,
};

enum class PCLK_Divider {
    PCLK_2,
    PCLK_4,
    PCLK_8,
    PCLK_16,
    PCLK_32,
    PCLK_64,
    PCLK_128,
    PCLK_256,
};

enum class DMA_Direction {
    DMA_TX,
    DMA_RX,
};

enum class CRC_Direction {
    CRC_TX,
    CRC_RX,
};

enum class Interrupt_Type {
    INTR_TBE,
    INTR_RBNE,
    INTR_ERR,
};

enum class Interrupt_Flags {
    INTR_FLAG_TBE,
    INTR_FLAG_RBNE,
    INTR_FLAG_RXORERR,
    INTR_FLAG_CONFERR,
    INTR_FLAG_CRCERR,
    INTR_FLAG_TXURERR,
    INTR_FLAG_FER,
};

enum class SPI_Error_Type {
    OK = 0,
    INVALID_SPI,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct SPI_Clock_Config {
    rcu::RCU_PCLK clock_reg;
    rcu::RCU_PCLK_Reset reset_reg;
};

static const SPI_Clock_Config SPI_pclk_index[] = {
    {rcu::RCU_PCLK::PCLK_SPI0, rcu::RCU_PCLK_Reset::PCLK_SPI0RST},
    {rcu::RCU_PCLK::PCLK_SPI1, rcu::RCU_PCLK_Reset::PCLK_SPI1RST},
    {rcu::RCU_PCLK::PCLK_SPI2, rcu::RCU_PCLK_Reset::PCLK_SPI2RST},
};

struct SPI_Pin_Config {
    gpio::GPIO_Base gpio_port;
    gpio::Pin_Number pin;
    gpio::Pin_Mode mode;
    gpio::Output_Speed speed;
};

struct SPI_Config {
    bool use_ssel_pin;
    SPI_Pin_Config mosi_pin;
    SPI_Pin_Config miso_pin;
    SPI_Pin_Config sclk_pin;
    SPI_Pin_Config ssel_pin;
    Operational_Mode operational_mode;
    Frame_Format frame_format;
    NSS_Type nss_type;
    PCLK_Divider pclk_divider;
    Endian_Type msbf;
    Clock_Polarity polarity_pull;
    Clock_Phase clock_phase;
};

} // namespace spi
