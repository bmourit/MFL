// gd32f30x GPIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>
#include <stdexcept>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "gpio_config.hpp"

namespace gpio {

class GPIO {
public:
    static Result<GPIO, GPIO_Error_Type> get_instance(GPIO_Base Base) {
        switch (Base) {
        case GPIO_Base::GPIOA_BASE:
            return get_enum_instance<GPIO_Base, GPIO, GPIO_Error_Type>(
                       Base, GPIO_Base::GPIOA_BASE, get_instance_for_base<GPIO_Base::GPIOA_BASE>()
                   );
        case GPIO_Base::GPIOB_BASE:
            return get_enum_instance<GPIO_Base, GPIO, GPIO_Error_Type>(
                       Base, GPIO_Base::GPIOB_BASE, get_instance_for_base<GPIO_Base::GPIOB_BASE>()
                   );
        case GPIO_Base::GPIOC_BASE:
            return get_enum_instance<GPIO_Base, GPIO, GPIO_Error_Type>(
                       Base, GPIO_Base::GPIOC_BASE, get_instance_for_base<GPIO_Base::GPIOC_BASE>()
                   );
        case GPIO_Base::GPIOD_BASE:
            return get_enum_instance<GPIO_Base, GPIO, GPIO_Error_Type>(
                       Base, GPIO_Base::GPIOD_BASE, get_instance_for_base<GPIO_Base::GPIOD_BASE>()
                   );
        default:
            return RETURN_ERROR(GPIO, GPIO_Error_Type::INVALID_PORT);
        }
    }

    // Initialize
    void init_pin(Pin_Number pin, Pin_Mode mode, Output_Speed speed);
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

private:
    GPIO(GPIO_Base Base) : GPIO_pclk_info_(GPIO_pclk_index[static_cast<int>(Base)]),
        base_address_(GPIO_baseAddress[static_cast<int>(Base)]),
        base_index_(Base) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(GPIO_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(GPIO_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(GPIO_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    GPIO_Clock_Config GPIO_pclk_info_;
    uint32_t base_address_;
    GPIO_Base base_index_;
    static bool is_clock_enabled;

    template <GPIO_Base Base>
    static GPIO& get_instance_for_base() {
        static GPIO instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(GPIO_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(GPIO_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace gpio
