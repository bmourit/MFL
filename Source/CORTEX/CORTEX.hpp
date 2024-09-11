// gd32f30x CORTEX peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "cortex_config.hpp"

namespace cortex {

class CORTEX {
public:
    CORTEX() {}

    void set_low_power_mode_enable(Low_Power_Mode mode, bool enable);
    // SysTick
    void set_systick_source(Systick_Source source);
    // Vector table
    void set_nvic_vector_table(Vector_Table base, uint32_t offset);
    // Priority
    void set_nvic_priority_group(Priority_Group group);
    // Interrupts
    void nvic_irq_enable(uint8_t irq, uint8_t preemption_priority, uint8_t sub_priority);
    void nvic_irq_disable(uint8_t irq);
};

} // namespace cortex

extern cortex::CORTEX CORTEX_DEVICE;
