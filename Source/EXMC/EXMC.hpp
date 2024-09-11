// gd32f30x EXMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "exmc_config.hpp"

namespace exmc {

class EXMC {
public:
    EXMC() {}

    // NOR and SRAM
    void nor_sram_reset(Block_Number block);
    void nor_sram_init();
    void nor_sram_configure(NOR_SRAM_Config config) {
        nor_sram_config_ = config;
        nor_sram_init();
    }
    void set_nor_sram_enable(Block_Number block, bool enable);
    // NAND
    void nand_reset(NPC_Block npc_block);
    void nand_init();
    void nand_configure(NAND_Config config) {
        nand_config_ = config;
        nand_init();
    }
    void set_nand_enable(NPC_Block npc_block, bool enable);
    // PC card
    void pccard_reset(void);
    void pccard_init();
    void pccard_configure(PCCARD_Config config) {
        pccard_config_ = config;
        pccard_init();
    }
    void set_pccard_enable(bool enable);
    // ECC
    void set_nor_sram_page_size(Block_Number block, Page_Size size);
    void set_nand_ecc_enable(NPC_Block npc_block, bool enable);
    uint32_t get_ecc(NPC_Block npc_block);
    // Interrupts and flags
    bool get_flag(NPC_Block npc_block, Status_Flags flag);
    void clear_flag(NPC_Block npc_block, Status_Flags flag);
    bool get_interrupt_flag(NPC_Block npc_block, Interrupt_Flags flag);
    void clear_interrupt_flag(NPC_Block npc_block, Interrupt_Flags flag);
    void set_interrupt_enable(NPC_Block npc_block, Interrupt_Type type, bool enable);

    static constexpr uintptr_t EXMC_baseAddress = 0xA0000000;

    inline volatile uint32_t* reg_address(EXMC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(EXMC_baseAddress + static_cast<uint32_t>(reg));
    }

    inline volatile uint32_t* reg_address(EXMC_Base_Regs base_reg, Block_Number block) const {
        return reinterpret_cast<volatile uint32_t *>(
                   EXMC_baseAddress + static_cast<uint32_t>(base_reg) + (static_cast<uint32_t>(block) * NSRAM_Block_Offset));
    }

    inline volatile uint32_t* reg_address(EXMC_Base_Regs base_reg, NPC_Block block) const {
        return reinterpret_cast<volatile uint32_t *>(
                   EXMC_baseAddress + static_cast<uint32_t>(base_reg) + (static_cast<uint32_t>(block) * NPC_Block_Offset));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_EXMC, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;

    NOR_SRAM_Timing rw_timing_default = {
        .async_access = Async_Mode::MODE_A,
        .sync_latency = Data_Latency::LATENCY_17_CLK,
        .divider = Sync_Divider::DIV16,
        .bus_latency = 0xF,
        .async_dst = 0xFF,
        .async_aht = 0xF,
        .async_ast = 0xF,
    };

    NOR_SRAM_Timing write_timing_default = {
        .async_access = Async_Mode::MODE_A,
        .sync_latency = Data_Latency::LATENCY_2_CLK,
        .divider = Sync_Divider::NONE,
        .bus_latency = 0xF,
        .async_dst = 0xFF,
        .async_aht = 0xF,
        .async_ast = 0xF,
    };

    NOR_SRAM_Config nor_sram_default_config = {
        .block = Block_Number::BLOCK0,
        .mode = Write_Mode::ASYNC,
        .async_wait = false,
        .extended_mode = false,
        .nwait_signal = true,
        .memory_write = true,
        .nwait_active = NWAIT_Active::BEFORE,
        .wrap = false,
        .polarity = Signal_Polarity::LOW,
        .burst = false,
        .width = Bus_Width::WIDTH_8BITS,
        .type = Memory_Type::SRAM,
        .address_mux = true,
        .rw_timing = &rw_timing_default,
        .write_timing = &write_timing_default,
    };

    NPC_Timing common_timing_default = {
        .dbhzt = 0xFC,
        .ht = 0xFC,
        .wt = 0xFC,
        .st = 0xFC,
    };

    NAND_Config nand_default_config = {
        .npc_block = NPC_Block::NAND_BLOCK1,
        .ecc_size = ECC_Size::BYTES_256,
        .ctr_latency = HCLK_Delay::DELAY_1_HCLK,
        .atr_latency = HCLK_Delay::DELAY_1_HCLK,
        .ecc = false,
        .databus_width = Bus_Width::WIDTH_8BITS,
        .wait = false,
        .common_timing = &common_timing_default,
        .attribute_timing = &common_timing_default,
    };

    NPC_Timing io_timing_default = {
        .dbhzt = 0xFC,
        .ht = 0xFC,
        .wt = 0xFC,
        .st = 0xFC,
    };

    PCCARD_Config pccard_default_config = {
        .ctr_latency = HCLK_Delay::DELAY_1_HCLK,
        .atr_latency = HCLK_Delay::DELAY_1_HCLK,
        .wait = false,
        .common_timing = &common_timing_default,
        .attribute_timing = &common_timing_default,
        .io_timing = &io_timing_default,

    };

    NOR_SRAM_Config nor_sram_config_ = nor_sram_default_config;
    NAND_Config nand_config_ = nand_default_config;
    PCCARD_Config pccard_config_ = pccard_default_config;
};

} // namespace exmc

extern exmc::EXMC EXMC_DEVICE;
