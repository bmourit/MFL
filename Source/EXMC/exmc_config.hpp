// gd32f30x EXMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace exmc {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class EXMC_Regs {
    SNCTL0 = 0x00,
    SNTCFG0 = 0x04,
    SNCTL1 = 0x08,
    SNTCFG1 = 0x0C,
    SNCTL2 = 0x10,
    SNTCFG2 = 0x14,
    SNCTL3 = 0x18,
    SNTCFG3 = 0x1C,
    NPCTL1 = 0x60,
    NPINTEN1 = 0x64,
    NPCTCFG1 = 0x68,
    NPATCFG1 = 0x6C,
    NECC1 = 0x74,
    NPCTL2 = 0x80,
    NPINTEN2 = 0x84,
    NPCTCFG2 = 0x88,
    NPATCFG2 = 0x8C,
    NECC2 = 0x94,
    NPCTL3 = 0xA0,
    NPINTEN3 = 0xA4,
    NPCTCFG3 = 0xA8,
    NPATCFG3 = 0xAC,
    PIOTCFG3 = 0xB0,
    SNWTCFG0 = 0x104,
    SNWTCFG1 = 0x10C,
    SNWTCFG2 = 0x114,
    SNWTCFG3 = 0x11C,
};

enum class EXMC_Base_Regs {
    SNCTL_BASE = 0x00,
    SNTCFG_BASE = 0x04,
    NPCTL_BASE = 0x60,
    NPINTEN_BASE = 0x64,
    NPCTCFG_BASE = 0x68,
    NPATCFG_BASE = 0x6C,
    NECC_BASE = 0x74,
    SNWTCFG_BASE = 0x104,
};

enum class SNCTLX_Bits {
    NRBKEN = REG_BIT_DEF(0, 0),
    NRMUX = REG_BIT_DEF(1, 1),
    NRTP = REG_BIT_DEF(2, 3),
    NRW = REG_BIT_DEF(4, 5),
    NREN = REG_BIT_DEF(6, 6),
    SBRSTEN = REG_BIT_DEF(8, 8),
    NRWTPOL = REG_BIT_DEF(9, 9),
    WRAPEN = REG_BIT_DEF(10, 10),
    NRWTCFG = REG_BIT_DEF(11, 11),
    WREN = REG_BIT_DEF(12, 12),
    NRWTEN = REG_BIT_DEF(13, 13),
    EXMODEN = REG_BIT_DEF(14, 14),
    ASYNCWAIT = REG_BIT_DEF(15, 15),
    CPS = REG_BIT_DEF(16, 18),
    SYNCWR = REG_BIT_DEF(19, 19),
};

enum class SNTCFGX_Bits {
    ASET = REG_BIT_DEF(0, 3),
    AHLD = REG_BIT_DEF(4, 7),
    DSET = REG_BIT_DEF(8, 15),
    BUSLAT = REG_BIT_DEF(16, 19),
    CKDIV = REG_BIT_DEF(20, 23),
    DLAT = REG_BIT_DEF(24, 27),
    ASYNCMOD = REG_BIT_DEF(28, 29),
};

enum class SNWTCFGX_Bits {
    WASET = REG_BIT_DEF(0, 3),
    WAHLD = REG_BIT_DEF(4, 7),
    WDSET = REG_BIT_DEF(8, 15),
    WBUSLAT = REG_BIT_DEF(16, 19),
    WASYNCMOD = REG_BIT_DEF(28, 29)
};

enum class NPCTLX_Bits {
    NDWTEN = REG_BIT_DEF(1, 1),
    NDBKEN = REG_BIT_DEF(2, 2),
    NDTP = REG_BIT_DEF(3, 3),
    NDW = REG_BIT_DEF(4, 5),
    ECCEN = REG_BIT_DEF(6, 6),
    CTR = REG_BIT_DEF(9, 12),
    ATR = REG_BIT_DEF(13, 16),
    ECCSZ = REG_BIT_DEF(17, 19),
};

enum class NPINTENX_Bits {
    INTRS = REG_BIT_DEF(0, 0),
    INTHS = REG_BIT_DEF(1, 1),
    INTFS = REG_BIT_DEF(2, 2),
    INTREN = REG_BIT_DEF(3, 3),
    INTHEN = REG_BIT_DEF(4, 4),
    INTFEN = REG_BIT_DEF(5, 5),
    FFEPT = REG_BIT_DEF(6, 6),
};

enum class NPCTCFGX_Bits {
    COMSET = REG_BIT_DEF(0, 7),
    COMWAIT = REG_BIT_DEF(8, 15),
    COMHLD = REG_BIT_DEF(16, 23),
    COMHIZ = REG_BIT_DEF(24, 31),
};

enum class NPATCFGX_Bits {
    ATTSET = REG_BIT_DEF(0, 7),
    ATTWAIT = REG_BIT_DEF(8, 15),
    ATTHLD = REG_BIT_DEF(16, 23),
    ATTHIZ = REG_BIT_DEF(24, 31),
};

enum class PIOTCFG3_Bits {
    IOSET = REG_BIT_DEF(0, 7),
    IOWAIT = REG_BIT_DEF(8, 15),
    IOHLD = REG_BIT_DEF(16, 23),
    IOHIZ = REG_BIT_DEF(24, 31),
};

enum class NECCX_Bits {
    ECC = REG_BIT_DEF(0, 31),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Page_Size {
    AUTO_SPLIT,
    BYTES_128,
    BYTES_256,
    BYTES_512,
    BYTES_1024,
};

enum class Bus_Width {
    WIDTH_8BITS,
    WIDTH_16BITS,
};

enum class Memory_Type {
    SRAM,
    PSRAM,
    NOR,
};

enum class Async_Mode {
    MODE_A,
    MODE_B,
    MODE_C,
    MODE_D,
};

enum class Data_Latency {
    LATENCY_2_CLK,
    LATENCY_3_CLK,
    LATENCY_4_CLK,
    LATENCY_5_CLK,
    LATENCY_6_CLK,
    LATENCY_7_CLK,
    LATENCY_8_CLK,
    LATENCY_9_CLK,
    LATENCY_10_CLK,
    LATENCY_11_CLK,
    LATENCY_12_CLK,
    LATENCY_13_CLK,
    LATENCY_14_CLK,
    LATENCY_15_CLK,
    LATENCY_16_CLK,
    LATENCY_17_CLK,
};

enum class Sync_Divider {
    NONE,
    DIV2,
    DIV3,
    DIV4,
    DIV5,
    DIV6,
    DIV7,
    DIV8,
    DIV9,
    DIV10,
    DIV11,
    DIV12,
    DIV13,
    DIV14,
    DIV15,
    DIV16,
};

enum class ECC_Size {
    BYTES_256,
    BYTES_512,
    BYTES_1024,
    BYTES_2048,
    BYTES_4096,
    BYTES_8192,
};

enum class HCLK_Delay {
    DELAY_1_HCLK,
    DELAY_2_HCLK,
    DELAY_3_HCLK,
    DELAY_4_HCLK,
    DELAY_5_HCLK,
    DELAY_6_HCLK,
    DELAY_7_HCLK,
    DELAY_8_HCLK,
    DELAY_9_HCLK,
    DELAY_10_HCLK,
    DELAY_11_HCLK,
    DELAY_12_HCLK,
    DELAY_13_HCLK,
    DELAY_14_HCLK,
    DELAY_15_HCLK,
    DELAY_16_HCLK,
};

enum class Block_Number {
    BLOCK0,
    BLOCK1,
    BLOCK2,
    BLOCK3,
};

enum class Write_Mode {
    ASYNC,
    SYNC,
};

enum class NWAIT_Active {
    BEFORE,
    DURING,
};

enum Signal_Polarity {
    LOW,
    HIGH,
};

enum class NPC_Block {
    NAND_BLOCK1,
    NAND_BLOCK2,
    PCCARD_BLOCK3,
};

enum class Status_Flags {
    FLAG_RISING = REG_BIT_DEF(0, 0),
    FLAG_LEVEL = REG_BIT_DEF(1, 1),
    FLAG_FALLING = REG_BIT_DEF(2, 2),
    FLAG_FIFOE = REG_BIT_DEF(6, 6),
};

enum class Interrupt_Flags {
    INTR_FLAG_RISING = REG_BIT_DEF(0, 0),
    INTR_FLAG_LEVEL = REG_BIT_DEF(1, 1),
    INTR_FLAG_FALLING = REG_BIT_DEF(2, 2),
};

enum class Interrupt_Type {
    INTR_RISING_EN = REG_BIT_DEF(3, 3),
    INTR_LEVEL_EN = REG_BIT_DEF(4, 4),
    INTR_FALLING_EN = REG_BIT_DEF(5, 5),
};


///////////////////////////// STRUCTURES /////////////////////////////

struct NOR_SRAM_Timing {
    Async_Mode async_access;
    Data_Latency sync_latency;
    Sync_Divider divider;
    uint32_t bus_latency;
    uint32_t async_dst;     // Data setup time
    uint32_t async_aht;     // Address hold time
    uint32_t async_ast;     // Address setup time
};

struct NOR_SRAM_Config {
    Block_Number block;
    Write_Mode mode;
    bool async_wait;
    bool extended_mode;
    bool nwait_signal;
    bool memory_write;
    NWAIT_Active nwait_active;
    bool wrap;
    Signal_Polarity polarity;
    bool burst;
    Bus_Width width;
    Memory_Type type;
    bool address_mux;
    NOR_SRAM_Timing* rw_timing;
    NOR_SRAM_Timing* write_timing;
};

struct NPC_Timing {
    uint32_t dbhzt; // Data bus Hi z time
    uint32_t ht;    // Hold time
    uint32_t wt;    // Wait time
    uint32_t st;    // Setup time
};

struct NAND_Config {
    NPC_Block npc_block;
    ECC_Size ecc_size;
    HCLK_Delay ctr_latency;
    HCLK_Delay atr_latency;
    bool ecc;
    Bus_Width databus_width;
    bool wait;
    NPC_Timing* common_timing;
    NPC_Timing* attribute_timing;
};

struct PCCARD_Config {
    HCLK_Delay ctr_latency;
    HCLK_Delay atr_latency;
    bool wait;
    NPC_Timing* common_timing;
    NPC_Timing* attribute_timing;
    NPC_Timing* io_timing;
};


///////////////////////////// CONSTANTS /////////////////////////////

constexpr uint32_t NSRAM_Block_Offset = 0x08;
constexpr uint32_t NPC_Block_Offset = 0x20;

constexpr uint32_t SNCTL_Block0_Reset = 0x000030DB;
constexpr uint32_t SNCTL_Block1_2_3_Reset = 0x000030D2;
constexpr uint32_t Common_Reset = 0x0FFFFFFF;

constexpr uint32_t NPCTL_Block1_2_Reset = 0x00000018;
constexpr uint32_t NPINTEN_Block1_2_Reset = 0x00000042;

constexpr uint32_t NPCTL_Block3_Reset = 0x00000018;
constexpr uint32_t NPINTEN_Block3_Reset = 0x00000043;
constexpr uint32_t Common_Block3_Reset = 0xFCFCFCFC;


} // namespace exmc
