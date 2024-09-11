// gd32f30x EXMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "EXMC.hpp"

namespace exmc {

void EXMC::nor_sram_reset(Block_Number block) {
    write_register(*this, EXMC_Base_Regs::SNCTL_BASE, block, (block == Block_Number::BLOCK0) ? SNCTL_Block0_Reset : SNCTL_Block1_2_3_Reset);
    write_register(*this, EXMC_Base_Regs::SNTCFG_BASE, block, Common_Reset);
    write_register(*this, EXMC_Base_Regs::SNWTCFG_BASE, block, Common_Reset);
}

void EXMC::nor_sram_init() {
    write_bits(*this, EXMC_Base_Regs::SNCTL_BASE, nor_sram_config_.block, static_cast<uint32_t>(SNCTLX_Bits::NRMUX), nor_sram_config_.address_mux ? Set : Clear,
               static_cast<uint32_t>(SNCTLX_Bits::NRTP), static_cast<uint32_t>(nor_sram_config_.type), static_cast<uint32_t>(SNCTLX_Bits::NRW), static_cast<uint32_t>(nor_sram_config_.width),
               static_cast<uint32_t>(SNCTLX_Bits::NREN), Clear, static_cast<uint32_t>(SNCTLX_Bits::SBRSTEN), nor_sram_config_.burst ? Set : Clear,
               static_cast<uint32_t>(SNCTLX_Bits::NRWTPOL), static_cast<uint32_t>(nor_sram_config_.polarity), static_cast<uint32_t>(SNCTLX_Bits::WRAPEN), nor_sram_config_.wrap ? Set : Clear,
               static_cast<uint32_t>(SNCTLX_Bits::NRWTCFG), static_cast<uint32_t>(nor_sram_config_.nwait_active), static_cast<uint32_t>(SNCTLX_Bits::WREN), nor_sram_config_.memory_write ? Set : Clear,
               static_cast<uint32_t>(SNCTLX_Bits::NRWTEN), nor_sram_config_.nwait_signal ? Set : Clear, static_cast<uint32_t>(SNCTLX_Bits::EXMODEN), nor_sram_config_.extended_mode ? Set : Clear,
               static_cast<uint32_t>(SNCTLX_Bits::ASYNCWAIT), nor_sram_config_.async_wait ? Set : Clear, static_cast<uint32_t>(SNCTLX_Bits::SYNCWR), static_cast<uint32_t>(nor_sram_config_.mode));

    write_bits(*this, EXMC_Base_Regs::SNTCFG_BASE, nor_sram_config_.block, static_cast<uint32_t>(SNTCFGX_Bits::ASET), nor_sram_config_.rw_timing->async_ast - 1,
               static_cast<uint32_t>(SNTCFGX_Bits::AHLD), nor_sram_config_.rw_timing->async_aht - 1,
               static_cast<uint32_t>(SNTCFGX_Bits::DSET), nor_sram_config_.rw_timing->async_dst - 1,
               static_cast<uint32_t>(SNTCFGX_Bits::BUSLAT), nor_sram_config_.rw_timing->bus_latency - 1,
               static_cast<uint32_t>(SNTCFGX_Bits::CKDIV), static_cast<uint32_t>(nor_sram_config_.rw_timing->divider),
               static_cast<uint32_t>(SNTCFGX_Bits::DLAT), static_cast<uint32_t>(nor_sram_config_.rw_timing->sync_latency),
               static_cast<uint32_t>(SNTCFGX_Bits::ASYNCMOD), static_cast<uint32_t>(nor_sram_config_.rw_timing->async_access));

    write_bit(*this, EXMC_Base_Regs::SNCTL_BASE, nor_sram_config_.block, static_cast<uint32_t>(SNCTLX_Bits::NREN), (nor_sram_config_.type == Memory_Type::NOR) ? Set : Clear, true);

    if (nor_sram_config_.extended_mode == true) {
        write_bits(*this, EXMC_Base_Regs::SNWTCFG_BASE, nor_sram_config_.block, static_cast<uint32_t>(SNWTCFGX_Bits::WASET), nor_sram_config_.write_timing->async_ast - 1,
                   static_cast<uint32_t>(SNWTCFGX_Bits::WAHLD), nor_sram_config_.write_timing->async_aht - 1,
                   static_cast<uint32_t>(SNWTCFGX_Bits::WDSET), nor_sram_config_.write_timing->async_dst - 1,
                   static_cast<uint32_t>(SNWTCFGX_Bits::WBUSLAT), nor_sram_config_.write_timing->bus_latency - 1,
                   static_cast<uint32_t>(SNWTCFGX_Bits::WASET), static_cast<uint32_t>(nor_sram_config_.write_timing->async_access));
    } else {
        write_register(*this, EXMC_Base_Regs::SNWTCFG_BASE, nor_sram_config_.block, Common_Reset);
    }
}

void EXMC::set_nor_sram_enable(Block_Number block, bool enable) {
    write_bit(*this, EXMC_Base_Regs::SNCTL_BASE, block, static_cast<uint32_t>(SNCTLX_Bits::NRBKEN), enable ? Set : Clear, true);
}

void EXMC::nand_reset(NPC_Block block) {
    write_register(*this, EXMC_Base_Regs::NPCTL_BASE, block, NPCTL_Block1_2_Reset);
    write_register(*this, EXMC_Base_Regs::NPINTEN_BASE, block, NPINTEN_Block1_2_Reset);
    write_register(*this, EXMC_Base_Regs::NPCTCFG_BASE, block, Common_Block3_Reset);
    write_register(*this, EXMC_Base_Regs::NPATCFG_BASE, block, Common_Block3_Reset);
}

void EXMC::nand_init() {
    write_bits(*this, EXMC_Base_Regs::NPCTL_BASE, nand_config_.npc_block, static_cast<uint32_t>(NPCTLX_Bits::NDWTEN), nand_config_.wait ? Set : Clear,
               static_cast<uint32_t>(NPCTLX_Bits::NDTP), Set, static_cast<uint32_t>(NPCTLX_Bits::NDW), static_cast<uint32_t>(nand_config_.databus_width),
               static_cast<uint32_t>(NPCTLX_Bits::ECCEN), nand_config_.ecc ? Set : Clear, static_cast<uint32_t>(NPCTLX_Bits::ECCSZ), static_cast<uint32_t>(nand_config_.ecc_size),
               static_cast<uint32_t>(NPCTLX_Bits::CTR), static_cast<uint32_t>(nand_config_.ctr_latency), static_cast<uint32_t>(NPCTLX_Bits::ATR), static_cast<uint32_t>(nand_config_.atr_latency));

    write_bits(*this, EXMC_Base_Regs::NPCTCFG_BASE, nand_config_.npc_block, static_cast<uint32_t>(NPCTCFGX_Bits::COMSET), nand_config_.common_timing->st - 1,
               static_cast<uint32_t>(NPCTCFGX_Bits::COMWAIT), nand_config_.common_timing->wt - 1, static_cast<uint32_t>(NPCTCFGX_Bits::COMHLD), nand_config_.common_timing->ht,
               static_cast<uint32_t>(NPCTCFGX_Bits::COMHIZ), nand_config_.common_timing->dbhzt - 1);

    write_bits(*this, EXMC_Base_Regs::NPATCFG_BASE, nand_config_.npc_block, static_cast<uint32_t>(NPATCFGX_Bits::ATTSET), nand_config_.attribute_timing->st - 1,
               static_cast<uint32_t>(NPATCFGX_Bits::ATTWAIT), nand_config_.attribute_timing->wt - 1, static_cast<uint32_t>(NPATCFGX_Bits::ATTHLD), nand_config_.attribute_timing->ht,
               static_cast<uint32_t>(NPATCFGX_Bits::ATTHIZ), nand_config_.attribute_timing->dbhzt - 1);
}

void EXMC::set_nand_enable(NPC_Block npc_block, bool enable) {
    write_bit(*this, EXMC_Base_Regs::NPCTL_BASE, npc_block, static_cast<uint32_t>(NPCTLX_Bits::NDBKEN), enable ? Set : Clear, true);
}

void EXMC::pccard_reset() {
    write_register(*this, EXMC_Regs::NPCTL3, NPCTL_Block3_Reset, true);
    write_register(*this, EXMC_Regs::NPINTEN3, NPINTEN_Block3_Reset, true);
    write_register(*this, EXMC_Regs::NPCTCFG3, Common_Block3_Reset, true);
    write_register(*this, EXMC_Regs::NPATCFG3, Common_Block3_Reset, true);
    write_register(*this, EXMC_Regs::PIOTCFG3, Common_Block3_Reset, true);
}

void EXMC::pccard_init() {
    write_bits(*this, EXMC_Regs::NPCTL3, static_cast<uint32_t>(NPCTLX_Bits::NDWTEN), pccard_config_.wait ? Set : Clear,
              static_cast<uint32_t>(NPCTLX_Bits::NDW), static_cast<uint32_t>(Bus_Width::WIDTH_16BITS),
              static_cast<uint32_t>(NPCTLX_Bits::CTR), static_cast<uint32_t>(pccard_config_.ctr_latency),
              static_cast<uint32_t>(NPCTLX_Bits::ATR), static_cast<uint32_t>(pccard_config_.atr_latency));

    write_bits(*this, EXMC_Regs::NPCTCFG3, static_cast<uint32_t>(NPCTCFGX_Bits::COMSET), pccard_config_.common_timing->st - 1,
              static_cast<uint32_t>(NPCTCFGX_Bits::COMWAIT), pccard_config_.common_timing->wt - 1,
              static_cast<uint32_t>(NPCTCFGX_Bits::COMHLD), pccard_config_.common_timing->ht,
              static_cast<uint32_t>(NPCTCFGX_Bits::COMHIZ), pccard_config_.common_timing->dbhzt - 1);

    write_bits(*this, EXMC_Regs::NPATCFG3, static_cast<uint32_t>(NPATCFGX_Bits::ATTSET), pccard_config_.attribute_timing->st - 1,
              static_cast<uint32_t>(NPATCFGX_Bits::ATTWAIT), pccard_config_.attribute_timing->wt - 1,
              static_cast<uint32_t>(NPATCFGX_Bits::ATTHLD), pccard_config_.attribute_timing->ht,
              static_cast<uint32_t>(NPATCFGX_Bits::ATTHIZ), pccard_config_.attribute_timing->dbhzt - 1);

    write_bits(*this, EXMC_Regs::PIOTCFG3, static_cast<uint32_t>(PIOTCFG3_Bits::IOSET), pccard_config_.io_timing->st - 1,
              static_cast<uint32_t>(PIOTCFG3_Bits::IOWAIT), pccard_config_.io_timing->wt - 1,
              static_cast<uint32_t>(PIOTCFG3_Bits::IOHLD), pccard_config_.io_timing->ht,
              static_cast<uint32_t>(PIOTCFG3_Bits::IOHIZ), pccard_config_.io_timing->dbhzt - 1);
}

void EXMC::set_pccard_enable(bool enable) {
    write_bit(*this, EXMC_Regs::NPCTL3, static_cast<uint32_t>(NPCTLX_Bits::NDBKEN), enable ? Set : Clear, true);
}

void EXMC::set_nor_sram_page_size(Block_Number block, Page_Size size) {
    write_bit(*this, EXMC_Base_Regs::SNCTL_BASE, block, static_cast<uint32_t>(SNCTLX_Bits::CPS), static_cast<uint32_t>(size), true);
}

void EXMC::set_nand_ecc_enable(NPC_Block npc_block, bool enable) {
    write_bit(*this, EXMC_Base_Regs::NPCTL_BASE, npc_block, static_cast<uint32_t>(NPCTLX_Bits::ECCEN), enable ? Set : Clear, true);
}

uint32_t EXMC::get_ecc(NPC_Block npc_block) {
    return read_register<uint32_t>(*this, EXMC_Base_Regs::NECC_BASE, npc_block);
}

bool EXMC::get_flag(NPC_Block npc_block, Status_Flags flag) {
    return (read_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(flag), true) != Clear);
}

void EXMC::clear_flag(NPC_Block npc_block, Status_Flags flag) {
    write_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(flag), Clear, true);
}

bool EXMC::get_interrupt_flag(NPC_Block npc_block, Interrupt_Flags flag) {
    uint32_t intr_status = 0;
    uint32_t flag_status = read_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(flag), true);

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_RISING:
        intr_status = read_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(Interrupt_Type::INTR_RISING_EN), true);
        break;
    case Interrupt_Flags::INTR_FLAG_LEVEL:
        intr_status = read_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(Interrupt_Type::INTR_LEVEL_EN), true);
        break;
    case Interrupt_Flags::INTR_FLAG_FALLING:
        intr_status = read_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(Interrupt_Type::INTR_FALLING_EN), true);
        break;
    default:
        break;
    }

    return ((flag_status) && (intr_status));
}

void EXMC::clear_interrupt_flag(NPC_Block npc_block, Interrupt_Flags flag) {
    write_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(flag), Clear, true);
}

void EXMC::set_interrupt_enable(NPC_Block npc_block, Interrupt_Type type, bool enable) {
    write_bit(*this, EXMC_Base_Regs::NPINTEN_BASE, npc_block, static_cast<uint32_t>(type), enable ? Set : Clear, true);
}

} // namespace exmc

exmc::EXMC EXMC_DEVICE;
