// gd32f30x GPIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "GPIO.hpp"

// Initialize the static member
bool gpio::GPIO::is_clock_enabled = false;

namespace gpio {

void GPIO::init_pin(Pin_Number pin, Pin_Mode mode, Output_Speed speed)
{
    // Determine the correct register and bit position for the pin configuration
    GPIO_Regs reg = (pin < Pin_Number::PIN_8) ? GPIO_Regs::CTL0 : GPIO_Regs::CTL1;
    uint32_t shift = (static_cast<uint32_t>(pin) % 8) * 4;

    // Read the current register value
    uint32_t ctl_value = read_register<uint32_t>(reg);

    // Clear the relevant bits for the pin
    ctl_value &= ~(0xF << shift);

    // Set the mode bits (MDx and configuration bits (CTLx)
    uint32_t mode_bits = 0;
    uint32_t config_bits = 0;

    switch (mode) {
    case Pin_Mode::ANALOG:
        mode_bits = 0x00;   // Input mode (Analog)
        config_bits = 0x00; // Analog mode
        break;
    case Pin_Mode::INPUT_FLOATING:
        mode_bits = 0x00;   // Input mode (Floating)
        config_bits = 0x01; // Floating input
        break;
    case Pin_Mode::INPUT_PULLUP:
        write_bit(*this, GPIO_Regs::BOP, 1 << static_cast<uint32_t>(pin), 1);
        mode_bits = 0x00;   // Input mode (Pull-up/Pull-down)
        config_bits = 0x02; // Input with pull-up/pull-down
        break;
    case Pin_Mode::INPUT_PULLDOWN:
        write_bit(*this, GPIO_Regs::BC, 1 << static_cast<uint32_t>(pin), 1);
        mode_bits = 0x00;   // Input mode (Pull-up/Pull-down)
        config_bits = 0x02; // Input with pull-up/pull-down
        break;
    case Pin_Mode::OUTPUT_PUSHPULL:
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 0x03;                           // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), 1);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        config_bits = 0x00;                             // GPIO output with push-pull
        break;
    case Pin_Mode::OUTPUT_OPENDRAIN:
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 0x03;                           // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), 1);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        config_bits = 0x01;                             // GPIO output with open-drain
        break;
    case Pin_Mode::ALT_PUSHPULL:
        mode_bits = 0x01;   // AF mode (same as Input)
        config_bits = 0x02; // AF output with push-pull
        break;
    case Pin_Mode::ALT_OPENDRAIN:
        mode_bits = 0x01;   // AF mode (same as Input)
        config_bits = 0x03; // AFIO output with open-drain
        break;
    }

    // Combine mode and configuration bits
    ctl_value |= (mode_bits << (shift + 2)) | (config_bits << shift);

    // Write the updated value back to the register
    write_register(reg, ctl_value);
}

void GPIO::set_pin_high(Pin_Bit_Mask pin)
{
    write_bit(*this, GPIO_Regs::BOP, static_cast<uint32_t>(pin), 1);
}

void GPIO::set_pin_low(Pin_Bit_Mask pin)
{
    write_bit(*this, GPIO_Regs::BC, static_cast<uint32_t>(pin), 1);
}

void GPIO::set_pin_pull(Pin_Bit_Mask pin, bool high)
{
    if (high != false) {
        write_bit(*this, GPIO_Regs::BOP, static_cast<uint32_t>(pin), 1);
    } else {
        write_bit(*this, GPIO_Regs::BC, static_cast<uint32_t>(pin), 1);
    }
}

void GPIO::set_port(uint16_t data)
{
    write_register(GPIO_Regs::OCTL, static_cast<uint32_t>(data));
}

bool GPIO::get_pin_input_state(Pin_Bit_Mask pin)
{
    return (read_bit(*this, GPIO_Regs::ISTAT, static_cast<uint32_t>(pin)) ? true : false);
}

bool GPIO::get_pin_output_state(Pin_Bit_Mask pin)
{
    return (read_bit(*this, GPIO_Regs::OCTL, static_cast<uint32_t>(pin)) ? true : false);
}

uint16_t GPIO::get_port_input_state()
{
    return read_register<uint32_t>(GPIO_Regs::ISTAT);
}

uint16_t GPIO::get_port_output_state()
{
    return read_register<uint32_t>(GPIO_Regs::OCTL);
}

void GPIO::lock_pin(Pin_Number pin)
{
    uint32_t lock = 0x00010000U;
    lock |= (1 << static_cast<uint32_t>(pin));

    // lock the pin by write/read in this order
    // write lock -> write pin -> write lock -> read 1x -> read 2x
    write_register(GPIO_Regs::LOCK, lock);
    write_register(GPIO_Regs::LOCK, (1 << static_cast<uint32_t>(pin)));
    write_register(GPIO_Regs::LOCK, lock);
    lock = read_register<uint32_t>(GPIO_Regs::LOCK);
    lock = read_register<uint32_t>(GPIO_Regs::LOCK);
}

} // namespace gpio
