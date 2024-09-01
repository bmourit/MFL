// gd32f30x EXTI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
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
    // Flags
    bool get_flag(EXTI_Line line);
    void clear_flag(EXTI_Line line);
    // Interrupt flags
    bool get_interrupt_flag(EXTI_Line line);
    void clear_interrupt_flag(EXTI_Line line);
    // Events
    void event_enable(EXTI_Line line);                  // DEPRECATED - use set_event_enable
    void event_disable(EXTI_Line line);                 // DEPRECATED - use set_event_enable
    void set_event_enable(EXTI_Line line, bool enable);
    // Software interrupts
    void software_interrupt_enable(EXTI_Line line);     // DEPRECATED - use set_software_interrrupt_enable
    void software_interrupt_disable(EXTI_Line line);    // DEPRECATED - use set_software_interrrupt_enable
    void set_software_interrupt_enable(EXTI_Line line, bool enable);
    // Interrupts
    void interrupt_enable(EXTI_Line line);              // DEPRECATED - use set_interrupt_enable
    void interrupt_disable(EXTI_Line line);             // DEPRECATED - use set_interrupt_enable
    void set_interrupt_enable(EXTI_Line line, bool enable);

    static constexpr uint32_t EXTI_baseAddress = 0x40010400;

    inline volatile uint32_t *reg_address(EXTI_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(EXTI_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(EXTI_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(EXTI_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;

    }
};

} // namespace exti

extern exti::EXTI EXTI_DEVICE;
