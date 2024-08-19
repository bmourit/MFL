// gd32f30x GPIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "gpio_config.hpp"

#define MAX_GPIOS   7

namespace gpio {

class GPIO {
public:
    static GPIO& instance(GPIO_Base Base) {
        static GPIO instances[MAX_GPIOS] = {
            GPIO(GPIO_Base::GPIOA_BASE),
            GPIO(GPIO_Base::GPIOB_BASE),
            GPIO(GPIO_Base::GPIOC_BASE),
            GPIO(GPIO_Base::GPIOD_BASE),
            GPIO(GPIO_Base::GPIOE_BASE),
            GPIO(GPIO_Base::GPIOF_BASE),
            GPIO(GPIO_Base::GPIOG_BASE),
        };
        return instances[static_cast<int>(Base)];
    }

    // Initialize
    void init(Pin_Number pin, Pin_Mode mode, Output_Speed speed);
    // Reset
    void reset();
    // Clock
    void set_pclk_enable(bool enable);
    // Pin pull
    void set_pin_high(Pin_Bit_Mask pin);
    void set_pin_low(Pin_Bit_Mask pin);
    void set_pin_pull(Pin_Bit_Mask pin, bool high);
    // Port
    void set_port(uint16_t data);
    // Pin state
    bool get_pin_input_state(Pin_Bit_Mask pin);
    bool get_pin_output_state(Pin_Bit_Mask pin);
    // Port state
    uint16_t get_port_input_state();
    uint16_t get_port_output_state();
    // Lock
    void lock_pin(Pin_Number pin);

    // Register address
    inline volatile uint32_t *reg_address(GPIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    // Clock parameter storage
    GPIO_Clock_Config GPIO_pclk_info_;

private:
    explicit GPIO(GPIO_Base Base) : GPIO_pclk_info_(get_clock_config(Base)),
        base_address_(get_base_address(Base)) {}

    uint32_t base_address_;

    template<typename T>
    inline T read_register(GPIO_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(GPIO_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    uint32_t get_base_address(GPIO_Base Base) {
        return GPIO_baseAddress[static_cast<int>(Base)];
    }

    GPIO_Clock_Config get_clock_config(GPIO_Base Base) {
        return GPIO_pclk_index[static_cast<int>(Base)];
    }
};

} // namespace gpio

// Usage example:
// gpio::GPIO& gpioa = gpio::GPIO::instance(gpio::GPIO_Base::GPIOA_BASE);
