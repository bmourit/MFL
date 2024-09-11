// Template versions of read_register/write_register
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <limits>
#include "BitRW.hpp"

template<typename T, typename RegType, typename Instance>
inline T read_register(const Instance& instance, RegType reg, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    return *reinterpret_cast<volatile T *>(instance.reg_address(reg));
}

template<typename T, typename Instance>
inline T read_register(const Instance& instance, dma::DMA_Regs reg, dma::DMA_Channel channel) {
    return *reinterpret_cast<volatile T *>(instance.reg_address(reg, channel));
}

template<typename T, typename Instance>
inline T read_register(const Instance& instance, exmc::EXMC_Base_Regs base_reg, exmc::Block_Number block) {
    instance.ensure_clock_enabled();
    return *reinterpret_cast<volatile T *>(instance.reg_address(base_reg, block));
}

template<typename T, typename Instance>
inline T read_register(const Instance& instance, exmc::EXMC_Base_Regs base_reg, exmc::NPC_Block block) {
    instance.ensure_clock_enabled();
    return *reinterpret_cast<volatile T *>(instance.reg_address(base_reg, block));
}

template<typename T, typename RegType, typename Instance>
inline void write_register(const Instance& instance, RegType reg, T value, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    *reinterpret_cast<volatile T *>(instance.reg_address(reg)) = value;
}

template<typename T, typename Instance>
inline void write_register(const Instance& instance, dma::DMA_Regs reg, dma::DMA_Channel channel, T value) {
    *reinterpret_cast<volatile T *>(instance.reg_address(reg, channel)) = value;
}

template<typename T, typename Instance>
inline void write_register(const Instance& instance, exmc::EXMC_Base_Regs base_reg, exmc::Block_Number block, T value) {
    instance.ensure_clock_enabled();
    *reinterpret_cast<volatile T *>(instance.reg_address(base_reg, block)) = value;
}

template<typename T, typename Instance>
inline void write_register(const Instance& instance, exmc::EXMC_Base_Regs base_reg, exmc::NPC_Block block, T value) {
    instance.ensure_clock_enabled();
    *reinterpret_cast<volatile T *>(instance.reg_address(base_reg, block)) = value;
}
