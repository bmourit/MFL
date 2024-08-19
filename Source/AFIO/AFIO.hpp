// gd32f30x AFIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "gpio_config.hpp"

namespace gpio {

class AFIO {
public:
    AFIO() {}

    void reset();
    void set_pclk_enable(bool enable);
    void set_remap(Pin_Remap_Select remap);
    void set_exti_source(Source_Port port, Pin_Number pin);
    void set_output_event(Event_Port port, Pin_Number pin);
    void output_event_enable();
    void output_event_disable();
    void set_compensation(Bit_State state);
    bool get_compensation();

    static constexpr uint32_t AFIO_baseAddress = 0x40010000;

    inline volatile uint32_t *reg_address(AFIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(AFIO_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(AFIO_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(AFIO_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace gpio

extern gpio::AFIO AFIO_DEVICE;
