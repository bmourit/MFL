// gd32f30x RCU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include <array>

#include "CONFIG.hpp"

namespace rcu {

///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class RCU_Regs {
    CTL = 0x00,
    CFG0 = 0x04,
    INTR = 0x08,
    APB2RST = 0x0C,
    APB1RST = 0x10,
    AHBEN = 0x14,
    APB2EN = 0x18,
    APB1EN = 0x1C,
    BDCTL = 0x20,
    RSTSCK = 0x24,
    CFG1 = 0x2C,
    DSV = 0x34,
    ADDCTL = 0xC0,
    ADDINTR = 0xCC,
    ADDAPB1RST = 0xE0,
    ADDAPB1EN = 0xE4,
};


///////////////////////////// STARTUP FUNCTIONALITY /////////////////////////////

static const unsigned char AHBPrescaler[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x06,
    0x07,
    0x08,
    0x09,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL_Bits : uint32_t {
    IRC8MEN = REG_BIT_DEF(0, 0),
    IRC8MSTB = REG_BIT_DEF(1, 1),
    IRC8MADJ = REG_BIT_DEF(3, 7),
    IRC8MCALIB = REG_BIT_DEF(8, 15),
    HXTALEN = REG_BIT_DEF(16, 16),
    HXTALSTB = REG_BIT_DEF(17, 17),
    HXTALBPS = REG_BIT_DEF(18, 18),
    CKMEN = REG_BIT_DEF(19, 19),
    PLLEN = REG_BIT_DEF(24, 24),
    PLLSTB = REG_BIT_DEF(25, 25),
};

enum class CFG0_Bits : uint32_t {
    SCS = REG_BIT_DEF(0, 1),
    SCSS = REG_BIT_DEF(2, 3),
    AHBPSC = REG_BIT_DEF(4, 7),
    APB1PSC = REG_BIT_DEF(8, 10),
    APB2PSC = REG_BIT_DEF(11, 13),
    ADCPSC = REG_BIT_DEF(14, 15),
    PLLSEL = REG_BIT_DEF(16, 16),
    PREDV0 = REG_BIT_DEF(17, 17),
    PLLMF = REG_BIT_DEF(18, 21),
    USBDPSC = REG_BIT_DEF(22, 23),
    CKOUT0SEL = REG_BIT_DEF(24, 26),
    PLLMF_4 = REG_BIT_DEF(27, 27),
    ADCPSC_2 = REG_BIT_DEF(28, 28),
    PLLMF_5 = REG_BIT_DEF(30, 30),
    USBDPSC_2 = REG_BIT_DEF(31, 31),
};

enum class INTR_Bits : uint32_t {
    IRC40KSTBIF = REG_BIT_DEF(0, 0),
    LXTALSTBIF = REG_BIT_DEF(1, 1),
    IRC8MSTBIF = REG_BIT_DEF(2, 2),
    HXTALSTBIF = REG_BIT_DEF(3, 3),
    PLLSTBIF = REG_BIT_DEF(4, 4),
    CKMIF = REG_BIT_DEF(7, 7),
    IRC40KSTBIE = REG_BIT_DEF(8, 8),
    LXTALSTBIE = REG_BIT_DEF(9, 9),
    IRC8MSTBIE = REG_BIT_DEF(10, 10),
    HXTALSTBIE = REG_BIT_DEF(11, 11),
    PLLSTBIE = REG_BIT_DEF(12, 12),
    IRC40KSTBIC = REG_BIT_DEF(16, 16),
    LXTALSTBIC = REG_BIT_DEF(17, 17),
    IRC8MSTBIC = REG_BIT_DEF(18, 18),
    HXTALSTBIC = REG_BIT_DEF(19, 19),
    PLLSTBIC = REG_BIT_DEF(20, 20),
    CKMIC = REG_BIT_DEF(23, 23),
    CLEAR_ALL = REG_BIT_DEF(16, 20),
};

enum class APB2RST_Bits : uint32_t {
    AFRST = REG_BIT_DEF(0, 0),
    PARST = REG_BIT_DEF(2, 2),
    PBRST = REG_BIT_DEF(3, 3),
    PCRST = REG_BIT_DEF(4, 4),
    PDRST = REG_BIT_DEF(5, 5),
    PERST = REG_BIT_DEF(6, 6),
    PFRST = REG_BIT_DEF(7, 7),
    PGRST = REG_BIT_DEF(8, 8),
    ADC0RST = REG_BIT_DEF(9, 9),
    ADC1RST = REG_BIT_DEF(10, 10),
    TIMER0RST = REG_BIT_DEF(11, 11),
    SPI0RST = REG_BIT_DEF(12, 12),
    TIMER7RST = REG_BIT_DEF(13, 13),
    USART0RST = REG_BIT_DEF(14, 14),
    ADC2RST = REG_BIT_DEF(15, 15),
};

enum class APB1RST_Bits : uint32_t {
    TIMER1RST = REG_BIT_DEF(0, 0),
    TIMER2RST = REG_BIT_DEF(1, 1),
    TIMER3RST = REG_BIT_DEF(2, 2),
    TIMER4RST = REG_BIT_DEF(3, 3),
    TIMER5RST = REG_BIT_DEF(4, 4),
    TIMER6RST = REG_BIT_DEF(5, 5),
    WWDGTRST = REG_BIT_DEF(11, 11),
    SPI1RST = REG_BIT_DEF(14, 14),
    SPI2RST = REG_BIT_DEF(15, 15),
    USART1RST = REG_BIT_DEF(17, 17),
    USART2RST = REG_BIT_DEF(18, 18),
    UART3RST = REG_BIT_DEF(19, 19),
    UART4RST = REG_BIT_DEF(20, 20),
    I2C0RST = REG_BIT_DEF(21, 21),
    I2C1RST = REG_BIT_DEF(22, 22),
    USBDRST = REG_BIT_DEF(23, 23),
    CAN0RST = REG_BIT_DEF(25, 25),
    BKPIRST = REG_BIT_DEF(27, 27),
    PMURST = REG_BIT_DEF(28, 28),
    DACRST = REG_BIT_DEF(29, 29),
};

enum class AHBEN_bit : uint32_t {
    DMA0EN = REG_BIT_DEF(0, 0),
    DMA1EN = REG_BIT_DEF(1, 1),
    SRAMSPEN = REG_BIT_DEF(2, 2),
    FMCSPEN = REG_BIT_DEF(4, 4),
    CRCEN = REG_BIT_DEF(6, 6),
    EXMCEN = REG_BIT_DEF(8, 8),
    SDIOEN = REG_BIT_DEF(10, 10),
};

enum class APB2EN_Bits : uint32_t {
    AFEN = REG_BIT_DEF(0, 0),
    PAEN = REG_BIT_DEF(2, 2),
    PBEN = REG_BIT_DEF(3, 3),
    PCEN = REG_BIT_DEF(4, 4),
    PDEN = REG_BIT_DEF(5, 5),
    PEEN = REG_BIT_DEF(6, 6),
    PFEN = REG_BIT_DEF(7, 7),
    PGEN = REG_BIT_DEF(8, 8),
    ADC0EN = REG_BIT_DEF(9, 9),
    ADC1EN = REG_BIT_DEF(10, 10),
    TIMER0EN = REG_BIT_DEF(11, 11),
    SPI0EN = REG_BIT_DEF(12, 12),
    TIMER7EN = REG_BIT_DEF(13, 13),
    USART0EN = REG_BIT_DEF(14, 14),
    ADC2EN = REG_BIT_DEF(15, 15),
};

enum class APB1EN_Bits : uint32_t {
    TIMER1EN = REG_BIT_DEF(0, 0),
    TIMER2EN = REG_BIT_DEF(1, 1),
    TIMER3EN = REG_BIT_DEF(2, 2),
    TIMER4EN = REG_BIT_DEF(3, 3),
    TIMER5EN = REG_BIT_DEF(4, 4),
    TIMER6EN = REG_BIT_DEF(5, 5),
    WWDGTEN = REG_BIT_DEF(11, 11),
    SPI1EN = REG_BIT_DEF(14, 14),
    SPI2EN = REG_BIT_DEF(15, 15),
    USART1EN = REG_BIT_DEF(17, 17),
    USART2EN = REG_BIT_DEF(18, 18),
    UART3EN = REG_BIT_DEF(19, 19),
    UART4EN = REG_BIT_DEF(20, 20),
    I2C0EN = REG_BIT_DEF(21, 21),
    I2C1EN = REG_BIT_DEF(22, 22),
    USBDEN = REG_BIT_DEF(23, 23),
    CAN0EN = REG_BIT_DEF(25, 25),
    BKPIEN = REG_BIT_DEF(27, 27),
    PMUEN = REG_BIT_DEF(28, 28),
    DACEN = REG_BIT_DEF(29, 29),
};

enum class BDCTL_Bits : uint32_t {
    LXTALEN = REG_BIT_DEF(0, 0),
    LXTALSTB = REG_BIT_DEF(1, 1),
    LXTALBPS = REG_BIT_DEF(2, 2),
    LXTALDRI = REG_BIT_DEF(3, 4),
    RTCSRC = REG_BIT_DEF(8, 9),
    RTCEN = REG_BIT_DEF(15, 15),
    BKPRST = REG_BIT_DEF(16, 16),
};

enum class RSTSCK_Bits : uint32_t {
    IRC40KEN = REG_BIT_DEF(0, 0),
    IRC40KSTB = REG_BIT_DEF(1, 1),
    RSTFC = REG_BIT_DEF(24, 24),
    EPRSTF = REG_BIT_DEF(26, 26),
    PORRSTF = REG_BIT_DEF(27, 27),
    SWRSTF = REG_BIT_DEF(28, 28),
    FWDGTRSTF = REG_BIT_DEF(29, 29),
    WWDGTRSTF = REG_BIT_DEF(30, 30),
    LPRSTF = REG_BIT_DEF(31, 31),
};

enum class DSV_Bits : uint32_t {
    DSLPVS = REG_BIT_DEF(0, 2),
};

enum class ADDCTL_Bits : uint32_t {
    CK48MSEL = REG_BIT_DEF(0, 0),
    IRC48MEN = REG_BIT_DEF(16, 16),
    IRC48MSTB = REG_BIT_DEF(17, 17),
    IRC48MCAL = REG_BIT_DEF(24, 31),
};

enum class ADDINTR_Bits : uint32_t {
    IRC48MSTBIF = REG_BIT_DEF(6, 6),
    IRC48MSTBIE = REG_BIT_DEF(14, 14),
    IRC48MSTBIC = REG_BIT_DEF(22, 22),
};

enum class CFG1_Bits : uint32_t {
    PREDV0 = REG_BIT_DEF(0, 3),
    PREDV1 = REG_BIT_DEF(4, 7),
    PLL1MF = REG_BIT_DEF(8, 11),
    PLL2MF = REG_BIT_DEF(12, 15),
    PREDV0SEL = REG_BIT_DEF(16, 16),
    I2S1SEL = REG_BIT_DEF(17, 17),
    I2S2SEL = REG_BIT_DEF(18, 18),
    ADCPSC_3 = REG_BIT_DEF(29, 29),
    PLLPRESEL = REG_BIT_DEF(30, 30),
    PLL2MF_4 =  REG_BIT_DEF(31, 31),
};


///////////////////////////// PCLKS /////////////////////////////

enum class RCU_PCLK {
    PCLK_DMA0,
    PCLK_DMA1,
    PCLK_CRC,
    PCLK_EXMC,
    PCLK_SDIO,
    PCLK_TIMER1,
    PCLK_TIMER2,
    PCLK_TIMER3,
    PCLK_TIMER4,
    PCLK_TIMER5,
    PCLK_TIMER6,
    PCLK_WWDGT,
    PCLK_SPI1,
    PCLK_SPI2,
    PCLK_USART1,
    PCLK_USART2,
    PCLK_UART3,
    PCLK_UART4,
    PCLK_I2C0,
    PCLK_I2C1,
    PCLK_USBD,
    PCLK_CAN0,
    PCLK_BKPI,
    PCLK_PMU,
    PCLK_DAC,
    PCLK_RTC,
    PCLK_CTC,
    PCLK_AF,
    PCLK_GPIOA,
    PCLK_GPIOB,
    PCLK_GPIOC,
    PCLK_GPIOD,
    PCLK_GPIOE,
    PCLK_GPIOF,
    PCLK_GPIOG,
    PCLK_ADC0,
    PCLK_ADC1,
    PCLK_TIMER0,
    PCLK_SPI0,
    PCLK_TIMER7,
    PCLK_USART0,
    PCLK_ADC2,
    PCLK_LAST
};

struct index_to_bits {
    RCU_Regs register_offset;
    uint32_t bit_info;
};

static const index_to_bits pclk_index[] = {
    {RCU_Regs::AHBEN, REG_BIT_DEF(0, 0)},
    {RCU_Regs::AHBEN, REG_BIT_DEF(1, 1)},
    {RCU_Regs::AHBEN, REG_BIT_DEF(6, 6)},
    {RCU_Regs::AHBEN, REG_BIT_DEF(8, 8)},
    {RCU_Regs::AHBEN, REG_BIT_DEF(10, 10)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(0, 0)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(1, 1)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(2, 2)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(3, 3)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(4, 4)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(5, 5)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(11, 11)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(14, 14)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(15, 15)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(17, 17)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(18, 18)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(19, 19)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(20, 20)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(21, 21)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(22, 22)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(23, 23)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(25, 25)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(27, 27)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(28, 28)},
    {RCU_Regs::APB1EN, REG_BIT_DEF(29, 29)},
    {RCU_Regs::BDCTL, REG_BIT_DEF(15, 15)},
    {RCU_Regs::ADDAPB1EN, REG_BIT_DEF(27, 27)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(0, 0)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(2, 2)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(3, 3)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(4, 4)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(5, 5)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(6, 6)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(7, 7)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(8, 8)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(9, 9)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(10, 10)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(11, 11)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(12, 12)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(13, 13)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(14, 14)},
    {RCU_Regs::APB2EN, REG_BIT_DEF(15, 15)},
};

///////////////////////////// PCLK RESET /////////////////////////////

enum class RCU_PCLK_Reset {
    PCLK_TIMER1RST = 0,
    PCLK_TIMER2RST,
    PCLK_TIMER3RST,
    PCLK_TIMER4RST,
    PCLK_TIMER5RST,
    PCLK_TIMER6RST,
    PCLK_WWDGTRST,
    PCLK_SPI1RST,
    PCLK_SPI2RST,
    PCLK_USART1RST,
    PCLK_USART2RST,
    PCLK_UART3RST,
    PCLK_UART4RST,
    PCLK_I2C0RST,
    PCLK_I2C1RST,
    PCLK_USBDRST,
    PCLK_CAN0RST,
    PCLK_BKPIRST,
    PCLK_PMURST,
    PCLK_DACRST,
    PCLK_CTCRST,
    PCLK_AFRST,
    PCLK_GPIOARST,
    PCLK_GPIOBRST,
    PCLK_GPIOCRST,
    PCLK_GPIODRST,
    PCLK_GPIOERST,
    PCLK_GPIOFRST,
    PCLK_GPIOGRST,
    PCLK_ADC0RST,
    PCLK_ADC1RST,
    PCLK_TIMER0RST,
    PCLK_SPI0RST,
    PCLK_TIMER7RST,
    PCLK_USART0RST,
    PCLK_ADC2RST,
    PCLK_RST_LAST,
};

static const index_to_bits pclk_reset_index[] = {
    {RCU_Regs::APB1RST, REG_BIT_DEF(0, 0)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(1, 1)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(2, 2)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(3, 3)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(4, 4)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(5, 5)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(11, 11)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(14, 14)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(15, 15)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(17, 17)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(18, 18)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(19, 19)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(20, 20)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(21, 21)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(22, 22)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(23, 23)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(25, 25)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(27, 27)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(28, 28)},
    {RCU_Regs::APB1RST, REG_BIT_DEF(29, 29)},
    {RCU_Regs::ADDAPB1RST, REG_BIT_DEF(27, 27)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(0, 0)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(2, 2)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(3, 3)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(4, 4)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(5, 5)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(6, 6)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(7, 7)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(8, 8)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(9, 9)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(10, 10)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(11, 11)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(12, 12)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(13, 13)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(14, 14)},
    {RCU_Regs::APB2RST, REG_BIT_DEF(15, 15)},
};


///////////////////////////// SLEEP /////////////////////////////

enum class RCU_PCLK_Sleep {
    PCLK_SLEEP_SRAM,
    PCLK_SLEEP_FMC,
    PCLK_SLEEP_LAST,
};

static const index_to_bits pclk_sleep_index[] = {
    {RCU_Regs::AHBEN, REG_BIT_DEF(2, 2)},
    {RCU_Regs::AHBEN, REG_BIT_DEF(4, 4)},
};


///////////////////////////// INTERRUPTS AND FLAGS /////////////////////////////

enum class RCU_Reset_Flags {
    FLAG_IRC8MSTB,
    FLAG_HXTALSTB,
    FLAG_PLLSTB,
    FLAG_LXTALSTB,
    FLAG_IRC40KSTB,
    FLAG_IRC48MSTB,
    FLAG_EPRST,
    FLAG_PORRST,
    FLAG_SWRST,
    FLAG_FWDGTRST,
    FLAG_WWDGTRST,
    FLAG_LPRST,
    FLAG_LAST,
};

static const index_to_bits reset_flag_index[] = {
    {RCU_Regs::CTL, REG_BIT_DEF(1, 1)},
    {RCU_Regs::CTL, REG_BIT_DEF(17, 17)},
    {RCU_Regs::CTL, REG_BIT_DEF(25, 25)},
    {RCU_Regs::BDCTL, REG_BIT_DEF(1, 1)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(1, 1)},
    {RCU_Regs::ADDCTL, REG_BIT_DEF(17, 17)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(26, 26)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(27, 27)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(28, 28)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(29, 29)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(30, 30)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(31, 31)},
};

enum class RCU_Interrupt_Flags {
    INTR_FLAG_IRC40KSTB,
    INTR_FLAG_LXTALSTB,
    INTR_FLAG_IRC8MSTB,
    INTR_FLAG_HXTALSTB,
    INTR_FLAG_PLLSTB,
    INTR_FLAG_CKM,
    INTR_FLAG_IRC48MSTB,
    INTR_FLAG_LAST,
};

static const index_to_bits interrupt_flag_index[] = {
    {RCU_Regs::INTR, REG_BIT_DEF(0, 0)},
    {RCU_Regs::INTR, REG_BIT_DEF(1, 1)},
    {RCU_Regs::INTR, REG_BIT_DEF(2, 2)},
    {RCU_Regs::INTR, REG_BIT_DEF(3, 3)},
    {RCU_Regs::INTR, REG_BIT_DEF(4, 4)},
    {RCU_Regs::INTR, REG_BIT_DEF(7, 7)},
    {RCU_Regs::ADDINTR, REG_BIT_DEF(6, 6)},
};

enum class RCU_Interrupt_Clear_Flags {
    INTR_FLAG_IRC40KSTB_CLR,
    INTR_FLAG_LXTALSTB_CLR,
    INTR_FLAG_IRC8MSTB_CLR,
    INTR_FLAG_HXTALSTB_CLR,
    INTR_FLAG_PLLSTB_CLR,
    INTR_FLAG_CKM_CLR,
    INTR_FLAG_IRC48MSTB_CLR,
    INTR_FLAG_CLR_LAST,
};

static const index_to_bits interrupt_clear_flag_index[] = {
    {RCU_Regs::INTR, REG_BIT_DEF(16, 16)},
    {RCU_Regs::INTR, REG_BIT_DEF(17, 17)},
    {RCU_Regs::INTR, REG_BIT_DEF(18, 18)},
    {RCU_Regs::INTR, REG_BIT_DEF(19, 19)},
    {RCU_Regs::INTR, REG_BIT_DEF(20, 20)},
    {RCU_Regs::INTR, REG_BIT_DEF(23, 23)},
    {RCU_Regs::ADDINTR, REG_BIT_DEF(22, 22)},
};

enum class RCU_Interrupt_Enable {
    INTR_IRC40KSTB,
    INTR_LXTALSTB,
    INTR_IRC8MSTB,
    INTR_HXTALSTB,
    INTR_PLLSTB,
    INTR_IRC48MSTB,
    INTR_ENABLE_LAST,
};

static const index_to_bits interrupt_enable_index[] = {
    {RCU_Regs::INTR, REG_BIT_DEF(8, 8)},
    {RCU_Regs::INTR, REG_BIT_DEF(9, 9)},
    {RCU_Regs::INTR, REG_BIT_DEF(10, 10)},
    {RCU_Regs::INTR, REG_BIT_DEF(11, 11)},
    {RCU_Regs::INTR, REG_BIT_DEF(12, 21)},
    {RCU_Regs::ADDINTR, REG_BIT_DEF(14, 14)},
};

///////////////////////////// OSCI /////////////////////////////

enum class OSCI_Select {
    HXTAL,
    LXTAL,
    IRC8M,
    IRC48M,
    IRC40K,
    PLL_CK,
    LAST,
};

static const index_to_bits osci_select_index[] = {
    {RCU_Regs::CTL, REG_BIT_DEF(16, 16)},
    {RCU_Regs::BDCTL, REG_BIT_DEF(0, 0)},
    {RCU_Regs::CTL, REG_BIT_DEF(0, 0)},
    {RCU_Regs::ADDCTL, REG_BIT_DEF(16, 16)},
    {RCU_Regs::RSTSCK, REG_BIT_DEF(0, 0)},
    {RCU_Regs::CTL, REG_BIT_DEF(24, 24)},
};


///////////////////////////// SYSTEM CLOCK /////////////////////////////

enum class Clock_Frequency {
    CK_SYS,
    CK_AHB,
    CK_APB1,
    CK_APB2,
};

// SCS and SCSS
enum class System_Clock_Source {
    SOURCE_IRC8M,
    SOURCE_HXTAL,
    SOURCE_PLL,
    SOURCE_INVALID,
    SOURCE_LAST,
};

struct System_Clock_Source_Mapping {
    uint32_t value;
    System_Clock_Source source;
};

static const System_Clock_Source_Mapping source_mapping[] = {
    {0, System_Clock_Source::SOURCE_IRC8M},
    {1, System_Clock_Source::SOURCE_HXTAL},
    {3, System_Clock_Source::SOURCE_PLL},
};

///////////////////////////// AHB?APB1/APB2 BUSES /////////////////////////////

enum class AHB_Prescaler : uint32_t {
    CKSYS_DIV1 = 0,
    CKSYS_DIV2 = 8,
    CKSYS_DIV4 = 9,
    CKSYS_DIV8 = 10,
    CKSYS_DIV16 = 11,
    CKSYS_DIV64 = 12,
    CKSYS_DIV128 = 13,
    CKSYS_DIV256 = 14,
    CKSYS_DIV512 = 15,
};

// APB1 and APB2
enum class APB_Prescaler : uint32_t {
    CKAHB_DIV1 = 0,
    CKAHB_DIV2 = 4,
    CKAHB_DIV4 = 5,
    CKAHB_DIV8 = 6,
    CKAHB_DIV16 = 7,
};


///////////////////////////// ADC /////////////////////////////

enum class ADC_Prescaler : uint32_t {
    CKAPB2_DIV2 = 0x0,
    CKAPB2_DIV4 = 0x1,
    CKAPB2_DIV6 = 0x2,
    CKAPB2_DIV8 = 0x3,
    CKAPB2_DIV12 = 0x5,
    CKAPB2_DIV16 = 0x7,
    CKAHB_DIV5 = 0x8,
    CKAHB_DIV6 = 0x9,
    CKAHB_DIV10 = 0xA,
    CKAHB_DIV20 = 0xB,
};


///////////////////////////// PLL /////////////////////////////

enum class PLL_Source : int32_t {
    PLLSRC_IRC8M_DIV2,
    PLLSRC_HXTAL_IRC48M,
    PLLSRC_INVALID,
    PLLSRC_LAST,
};

struct PLL_Source_Mapping {
    uint32_t value;
    PLL_Source source;
};

static const PLL_Source_Mapping pll_mapping[] = {
    {0, PLL_Source::PLLSRC_IRC8M_DIV2},
    {1, PLL_Source::PLLSRC_HXTAL_IRC48M},
};

enum class PLLMF_Select {
    PLL_MUL2,
    PLL_MUL3,
    PLL_MUL4,
    PLL_MUL5,
    PLL_MUL6,
    PLL_MUL7,
    PLL_MUL8,
    PLL_MUL9,
    PLL_MUL10,
    PLL_MUL11,
    PLL_MUL12,
    PLL_MUL13,
    PLL_MUL14,
    PLL_MUL15,
    PLL_MUL16,
    PLL_MUL17,
    PLL_MUL18,
    PLL_MUL19,
    PLL_MUL20,
    PLL_MUL21,
    PLL_MUL22,
    PLL_MUL23,
    PLL_MUL24,
    PLL_MUL25,
    PLL_MUL26,
    PLL_MUL27,
    PLL_MUL28,
    PLL_MUL29,
    PLL_MUL30,
    PLL_MUL31,
    PLL_MUL32
};

enum class PLL_Presel {
    PLLPRESRC_HXTAL,
    PLLPRESRC_IRC48M,
    PLLPRESRC_INVALID,
    PLLPRESRC_LAST,
};

struct PLL_Presel_Mapping {
    uint32_t value;
    PLL_Presel source;
};

static const PLL_Presel_Mapping pll_presel_mapping[] = {
    {1, PLL_Presel::PLLPRESRC_HXTAL},
    {1, PLL_Presel::PLLPRESRC_IRC48M},
};


///////////////////////////// USB /////////////////////////////

enum class USB_Prescaler {
    DIV1_5,
    DIV1,
    DIV2_5,
    DIV2,
    DIV3,
    DIV3_5,
    DIV4
};


///////////////////////////// CKOUT0 /////////////////////////////

enum class CKOUT0_Source {
    CKOUT0SRC_NONE = 0,
    CKOUT0SRC_CKSYS = 4,
    CKOUT0SRC_IRC8M = 5,
    CKOUT0SRC_HXTAL = 6,
    CKOUT0SRC_CKPLL_DIV2 = 7
};


///////////////////////////// LXTAL /////////////////////////////

enum class LXTAL_Drive {
    LXTAL_LOWDRI,
    LXTAL_MED_LOWDRI,
    LXTAL_MED_HIGHDRI,
    LXTAL_HIGHDRI
};


///////////////////////////// RTC /////////////////////////////

enum class RTC_Source {
    RTCSRC_NONE,
    RTCSRC_LXTAL,
    RTCSRC_IRC40K,
    RTCSRC_HXTAL_DIV_128
};


///////////////////////////// DEEPSLEEP /////////////////////////////

enum class DeepSleep_Voltage {
    DEEPSLEEP_V_0,
    DEEPSLEEP_V_1,
    DEEPSLEEP_V_2,
    DEEPSLEEP_V_3
};


///////////////////////////// CK48M /////////////////////////////

enum class CK48M_Source {
    CK48MSRC_CKPLL,
    CK48MSRC_IRC48M
};


///////////////////////////// CONSTANTS /////////////////////////////

static const unsigned int HXTAL_VALUE = 8000000;
static const unsigned int IRC8M_VALUE = 8000000;
static const unsigned int IRC48M_VALUE = 48000000;
static const unsigned int IRC40K_VALUE = 40000;
static const unsigned int LXTAL_VALUE = 32768;
static const unsigned int OSC_STARTUP_TIMEOUT = 65535;
static const unsigned int HXTAL_STARTUP_TIMEOUT = 65535;
static const unsigned int LXTAL_STARTUP_TIMEOUT = 65535;
static const unsigned int IRC8M_STARTUP_TIMEOUT = 1280;

} // namespace rcu
