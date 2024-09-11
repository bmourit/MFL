// gd32f30x AFIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "gpio_config.hpp"

namespace gpio {

class AFIO {
public:
    AFIO() {}

    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, true);
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, false);
    }
    void set_remap(Pin_Remap_Select remap);
    void set_exti_source(Source_Port port, Pin_Number pin);
    void set_output_event(Event_Port port, Pin_Number pin);
    void set_output_event_enable(bool enable);
    void set_compensation(bool enable);
    bool get_compensation();

    static constexpr uintptr_t AFIO_baseAddress = 0x40010000;

    inline volatile uint32_t *reg_address(AFIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(AFIO_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_AF, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace gpio

extern gpio::AFIO AFIO_DEVICE;
