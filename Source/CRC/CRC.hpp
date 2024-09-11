// gd32f30x CRC functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
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
    static constexpr uintptr_t CRC_baseAddress = 0x40023000;

    // Register address
    inline volatile uint32_t *reg_address(CRC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(CRC_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_CRC, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace crc

extern crc::CRC CRC_DEVICE;
