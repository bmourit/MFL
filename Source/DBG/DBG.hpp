// gd32f30x DBG debug in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "dbg_config.hpp"

namespace armdbg {

class ARMDBG {
public:
    ARMDBG() {}

    // Reset
    void reset();
    // Debug ID
    uint32_t get_debug_id();
    // Trace
    void set_debug_with_trace_enable(bool enable);
    // Peripheral
    void set_peripheral_debug_enable(Debug_Peripheral peripheral, bool enable);
    // Low power
    void set_debug_during_low_power_enable(Low_Power_Debug type, bool enable);

    // Base address
    static constexpr uintptr_t DBG_baseAddress = 0xE0042000;

    // Offset access
    inline volatile uint32_t *reg_address(DBG_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(DBG_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}
};

} // namespace armdbg

extern armdbg::ARMDBG ARMDBG_DEVICE;
