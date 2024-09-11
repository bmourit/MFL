// gd32f30x EXTI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "exti_config.hpp"

namespace exti {

class EXTI {
public:
    EXTI() {}

    // Initialize
    void init(EXTI_Line line, EXTI_Mode mode, EXTI_Trigger trigger);
    // Reset
    void reset();
    // Events
    void set_event_enable(EXTI_Line line, bool enable);
    // Interrupts and flags
    bool get_flag(EXTI_Line line);
    void clear_flag(EXTI_Line line);
    bool get_interrupt_flag(EXTI_Line line);
    void clear_interrupt_flag(EXTI_Line line);
    void set_software_interrupt_enable(EXTI_Line line, bool enable);
    void set_interrupt_enable(EXTI_Line line, bool enable);

    static constexpr uintptr_t EXTI_baseAddress = 0x40010400;

    inline volatile uint32_t *reg_address(EXTI_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(EXTI_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}
};

} // namespace exti

extern exti::EXTI EXTI_DEVICE;
