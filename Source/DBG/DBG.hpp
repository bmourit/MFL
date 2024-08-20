// gd32f30x DBG debug in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include "BitRW.hpp"
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
    void debug_with_trace_enable();
    void debug_with_trace_disable();
    void set_debug_with_trace_enable(bool enable);
    // Peripheral
    void peripheral_debug_enable(Debug_Peripheral peripheral);
    void peripheral_debug_disable(Debug_Peripheral peripheral);
    void set_peripheral_debug_enable(Debug_Peripheral peripheral, bool enable);
    // Low power
    void debug_during_low_power_enable(Low_Power_Debug type);
    void debug_during_low_power_disable(Low_Power_Debug type);
    void set_debug_during_low_power_enable(Low_Power_Debug type, bool enable);

    // Base address
    static constexpr unsigned int DBG_baseAddress = 0xE0042000;

    // Offset access
    inline volatile uint32_t *reg_address(DBG_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(DBG_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(DBG_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(DBG_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace armdbg

extern armdbg::ARMDBG ARMDBG_DEVICE;
