// Template versions of read_bit/write_bit
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <limits>
#include "dma_config.hpp"

template <typename RegType, typename Instance>
inline uint32_t read_bit(const Instance& instance, RegType reg, uint32_t bits)
{
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval >>= bitno;
    regval &= ((1 << width) - 1);

    return regval;
}

template <typename RegType, typename Instance>
inline uint16_t read_bit16(const Instance& instance, RegType reg, uint32_t bits)
{
    uint32_t value = read_bit(instance, reg, bits);

    if (value > std::numeric_limits<unsigned short>::max()) {
        // TODO: Better handling here
        return 0;
    }

    return static_cast<uint16_t>(value);
}

template <typename RegType, typename Instance>
inline uint8_t read_bit8(const Instance& instance, RegType reg, uint32_t bits)
{
    uint32_t value = read_bit(instance, reg, bits);

    if (value > std::numeric_limits<unsigned char>::max()) {
        // TODO: Better handling here
        return 0;
    }

    return static_cast<uint8_t>(value);
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
inline void write_bit(const Instance& instance, RegType reg, uint32_t bits, uint32_t value)
{
    uint32_t regval = *instance.reg_address(reg);

    const uint32_t width = bits & 0xff;
    const uint32_t bitno = bits >> 16;

    regval &= ~(((1 << width) - 1) << bitno);
    regval |= value << bitno;

    *instance.reg_address(reg) = regval;
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
