// Template versions of read_bit/write_bit
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <limits>
#include "dma_config.hpp"
#include "exmc_config.hpp"

constexpr uint32_t Clear = 0;
constexpr uint32_t Set = 1;

template <typename RegType, typename Instance>
inline uint32_t read_bit(const Instance& instance, RegType reg, uint32_t bits, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval >>= bitno;
    regval &= ((1 << width) - 1);

    return regval;
}

template <typename RegType, typename Block, typename Instance>
inline uint32_t read_bit(const Instance& instance, RegType reg, Block block, uint32_t bits, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t regval = *instance.reg_address(reg, block);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval >>= bitno;
    regval &= ((1 << width) - 1);

    return regval;
}

template <typename RegType, typename Instance>
inline uint16_t read_bit16(const Instance& instance, RegType reg, uint32_t bits, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t value = read_bit(instance, reg, bits);

    if (value > std::numeric_limits<unsigned short>::max()) {
        // TODO: Better handling here
        return 0;
    }

    return static_cast<uint16_t>(value);
}

template <typename RegType, typename Instance>
inline uint8_t read_bit8(const Instance& instance, RegType reg, uint32_t bits, bool check_clock = false) {
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t value = read_bit(instance, reg, bits);

    if (value > std::numeric_limits<unsigned char>::max()) {
        // TODO: Better handling here
        return 0;
    }

    return static_cast<uint8_t>(value);
}

template <typename RegType, typename Instance>
inline uint32_t read_bit_with_channel_offset(const Instance& instance, RegType reg, uint32_t bits, dma::DMA_Channel channel)
{
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval >>= bitno;
    regval &= ((1 << width) -1) << (static_cast<uint32_t>(channel) * 4);

    return regval;
}

template <typename RegType, typename Instance>
inline uint32_t read_bit_channel(const Instance& instance, RegType reg, dma::DMA_Channel channel, uint32_t bits)
{
    uint32_t regval = *instance.reg_address(reg, channel);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval >>= bitno;
    regval &= ((1 << width) - 1);

    return regval;
}

template <typename RegType, typename Instance>
inline void write_bit(const Instance& instance, RegType reg, uint32_t bits, uint32_t value, bool check_clock = false)
{
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg) = regval;
}

template <typename RegType, typename Instance, typename Block>
inline void write_bit(const Instance& instance, RegType reg, Block block, uint32_t bits, uint32_t value, bool check_clock = false)
{
    if (check_clock) {
        instance.ensure_clock_enabled();
    }
    uint32_t regval = *instance.reg_address(reg, block);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg, block) = regval;
}

template <typename RegType, typename Instance>
inline void write_bit_extra(const Instance& instance, RegType reg, uint32_t bits, uint32_t value, uint32_t extra_offset)
{
    uint32_t regval = *instance.reg_address(reg, extra_offset);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg, extra_offset) = regval;
}

template <typename RegType, typename Instance>
inline void write_bit_channel(const Instance& instance, RegType reg, dma::DMA_Channel channel, uint32_t bits, uint32_t value)
{
    uint32_t regval = *instance.reg_address(reg, channel);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg, channel) = regval;
}

template <typename RegType, typename Instance>
inline void write_bit_with_channel_offset(const Instance& instance, RegType reg, uint32_t bits, dma::DMA_Channel channel, uint32_t value)
{
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << (bitno << (static_cast<uint32_t>(channel) * 4)));
    regval |= (value << (bitno << (static_cast<uint32_t>(channel) * 4)));

    *instance.reg_address(reg) = regval;
}

template <typename RegType, typename Instance>
inline void write_bits(const Instance& instance, RegType reg) {
    // Base case: No-op if no fields are passed
}

template <typename RegType, typename Instance, typename... Args>
inline void write_bits(const Instance& instance, RegType reg, uint32_t bits, uint32_t value, Args... args) {
    instance.ensure_clock_enabled();

    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= (value << bitno);

    *instance.reg_address(reg) = regval;

    // Recursive call to handle the remaining arguments
    if constexpr (sizeof...(args) > 0) {
        write_bits(instance, reg, args...);
    }
}

template <typename Block, typename Instance, typename... Args>
inline void write_bits(const Instance& instance, exmc::EXMC_Base_Regs reg, Block block, uint32_t bits, uint32_t value, Args... args) {
    instance.ensure_clock_enabled();

    uint32_t regval = *instance.reg_address(reg, block);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= (value << bitno);

    *instance.reg_address(reg, block) = regval;

    // Recursive call to handle the remaining arguments
    if constexpr (sizeof...(args) > 0) {
        write_bits(instance, reg, block, args...);
    }
}

template <typename RegType, typename Instance, typename... Args>
inline void write_bits_channel(const Instance& instance, RegType reg, dma::DMA_Channel channel, uint32_t bits, uint32_t value, Args... args) {
    uint32_t regval = *instance.reg_address(reg, channel);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg, channel) = regval;

    // Recursive call to handle the remaining arguments
    if constexpr (sizeof...(args) > 0) {
        write_bits_channel(instance, reg, channel, args...);
    }
}
