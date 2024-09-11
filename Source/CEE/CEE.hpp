// gd32f30x CEE register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "cee_config.hpp"

namespace cee {

class CEE {
public:
    CEE() {}

    void enhanced_mode_enable();
    void enhanced_mode_disable();
    void set_enhanced_mode_enable(bool enable);

    static constexpr uintptr_t CEE_baseAddress = 0x4002103C;

    // Register access needed for read_bit/write_bit functionality
    inline volatile uint32_t *reg_address(CEE_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(CEE_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}
};

} // namespace exti

extern cee::CEE CEE_DEVICE;
