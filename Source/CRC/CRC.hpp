// gd32f30x CRC functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "crc_config.hpp"

namespace crc {

class CRC {
public:
    CRC() {}

    // Reset
    void reset();
    // Data
    void reset_data();
    uint32_t get_data();
    // Free data
    uint8_t get_free_data();
    void set_free_data(uint8_t data);
    // Calculate
    uint32_t calculate_data(uint32_t data);
    uint32_t calculate_mulitple_data(const uint32_t *array, uint32_t size);

    // Base address
    static constexpr uint32_t CRC_baseAddress = 0x40023000;

    // Register address
    inline volatile uint32_t *reg_address(CRC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(reinterpret_cast<uintptr_t>(CRC_baseAddress) + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(CRC_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(CRC_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace crc
