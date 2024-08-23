// gd32f30x DMA peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include "CONFIG.hpp"
#include "rcu_config.hpp"

namespace dma {


///////////////////////////// BASE ADDRESS /////////////////////////////

enum class DMA_Base {
    DMA0_BASE,
    DMA1_BASE,
};

static constexpr unsigned int DMA_baseAddress[] = {
    0x40020000,
    0x40020400,
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class DMA_Regs {
    INTF = 0x00,
    INTC = 0x04,
    // Channel offsets
    CHXCTL = 0x08,
    CHXCNT = 0x0C,
    CHXPADDR = 0x10,
    CHXMADDR = 0x14,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class INTF_Bits {
    GIF = REG_BIT_DEF(0, 0),
    FTFIF = REG_BIT_DEF(1, 1),
    HTFIF = REG_BIT_DEF(2, 2),
    ERRIF = REG_BIT_DEF(3, 3),
};

enum class INTC_Bits {
    GIFC = REG_BIT_DEF(0, 0),
    FTFIFC = REG_BIT_DEF(1, 1),
    HTFIFC = REG_BIT_DEF(2, 2),
    ERRIFC = REG_BIT_DEF(3, 3),
};

enum class CHXCTL_Bits {
    CHEN = REG_BIT_DEF(0, 0),
    FTFIE = REG_BIT_DEF(1, 1),
    HTFIE = REG_BIT_DEF(2, 2),
    ERRIE = REG_BIT_DEF(3, 3),
    DIR = REG_BIT_DEF(4, 4),
    CMEN = REG_BIT_DEF(5, 5),
    PNAGA = REG_BIT_DEF(6, 6),
    MNAGA = REG_BIT_DEF(7, 7),
    PWIDTH = REG_BIT_DEF(8, 9),
    MWIDTH = REG_BIT_DEF(10, 11),
    PRIO = REG_BIT_DEF(12, 13),
    M2M = REG_BIT_DEF(14, 14),
};


///////////////////////////// ENUMS /////////////////////////////

enum class DMA_Channel {
    CHANNEL0 = 0,
    CHANNEL1,
    CHANNEL2,
    CHANNEL3,
    CHANNEL4,
    CHANNEL5,
    CHANNEL6,
};

enum class Interrupt_Flags {
    INTR_FLAG_GIF = REG_BIT_DEF(0, 0),
    INTR_FLAG_FTFIF = REG_BIT_DEF(1, 1),
    INTR_FLAG_HTFIF = REG_BIT_DEF(2, 2),
    INTR_FLAG_ERRIF = REG_BIT_DEF(3, 3),
};

enum class State_Flags {
    FLAG_GIF = REG_BIT_DEF(0, 0),
    FLAG_FTFIF = REG_BIT_DEF(1, 1),
    FLAG_HTFIF = REG_BIT_DEF(2, 2),
    FLAG_ERRIF = REG_BIT_DEF(3, 3),
};

enum class Interrupt_Type {
    INTR_FTFIE = REG_BIT_DEF(1, 1),
    INTR_HTFIE = REG_BIT_DEF(2, 2),
    INTR_ERRIE = REG_BIT_DEF(3, 3),
};

enum class Transfer_Direction {
    P2M,
    M2P,
};

enum class Increase_Mode {
    INCREASE_DISABLE,
    INCREASE_ENABLE,
};

enum class Bit_Width {
    WIDTH_8BIT,
    WIDTH_16BIT,
    WIDTH_32BIT,
};

enum class Channel_Priority {
    LOW_PRIORITY,
    MEDIUM_PRIORITY,
    HIGH_PRIORITY,
    ULTRA_HIGH_PRIORITY,
};

enum class M2M_Mode {
    M2M_MODE_DISABLE,
    M2M_MODE_ENABLE,
};

enum class Data_Type {
    MEMORY_ADDRESS,
    PERIPHERAL_ADDRESS,
};

enum class DMA_Error_Type {
    OK = 0,
    INVALID_DMA,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct DMA_Clock_Config {
    rcu::RCU_PCLK clock_reg;
};

static const DMA_Clock_Config DMA_pclk_index[] = {
    {rcu::RCU_PCLK::PCLK_DMA0},
    {rcu::RCU_PCLK::PCLK_DMA1},
};

struct DMA_Config {
    uint32_t peripheral_address = 0;
    Bit_Width peripheral_bit_width = Bit_Width::WIDTH_8BIT;
    uint32_t memory_address = 0;
    Bit_Width memory_bit_width = Bit_Width::WIDTH_8BIT;
    uint32_t count = 0U;
    Increase_Mode peripheral_increase = Increase_Mode::INCREASE_DISABLE;
    Increase_Mode memory_increase = Increase_Mode::INCREASE_DISABLE;
    Channel_Priority channel_priority = Channel_Priority::LOW_PRIORITY;
    Transfer_Direction direction = Transfer_Direction::P2M;
};

} // namespace dma
