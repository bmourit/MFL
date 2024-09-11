// gd32f30x GPIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "GPIO.hpp"

// Initialize the static member
bool gpio::GPIO::is_clock_enabled = false;

namespace gpio {

constexpr uint32_t LockValue = 0x00010000;

void GPIO::init_pin(Pin_Number pin, Pin_Mode mode, Output_Speed speed)
{
    // Determine the correct register and bit position for the pin configuration
    GPIO_Regs reg = (pin < Pin_Number::PIN_8) ? GPIO_Regs::CTL0 : GPIO_Regs::CTL1;
    uint32_t shift = (static_cast<uint32_t>(pin) % 8) * 4;

    // Read the current register value
    uint32_t ctl_value = read_register<uint32_t>(*this, reg);

    // Clear the relevant bits for the pin
    ctl_value &= ~(0xF << shift);

    // Set the mode bits (MDx[0:1]) and configuration bits (CTLx[2:3])
    uint32_t config_bits = 0;
    uint32_t mode_bits = 0;

    switch (mode) {
    case Pin_Mode::ANALOG:
        config_bits = 0;    // Analog mode
        mode_bits = 0;      // Input mode (Analog)
        break;
    case Pin_Mode::INPUT_FLOATING:
        config_bits = 1;    // Floating input
        mode_bits = 0;      // Input mode
        break;
    case Pin_Mode::INPUT_PULLUP:
        write_register<uint32_t>(*this, GPIO_Regs::BOP, 1 << static_cast<uint32_t>(pin));
        config_bits = 2;    // Input with pull-up/pull-down
        mode_bits = 0;      // Input mode
        break;
    case Pin_Mode::INPUT_PULLDOWN:
        write_register<uint32_t>(*this, GPIO_Regs::BC, 1 << static_cast<uint32_t>(pin));
        config_bits = 2;    // Input with pull-up/pull-down
        mode_bits = 0;      // Input mode
        break;
    case Pin_Mode::OUTPUT_PUSHPULL:
        config_bits = 0;    // GPIO output with push-pull
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 3;  // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), Set);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        break;
    case Pin_Mode::OUTPUT_OPENDRAIN:
        config_bits = 1;    // GPIO output with open-drain
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 3;  // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), Set);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        break;
    case Pin_Mode::ALT_PUSHPULL:
        config_bits = 2;    // AFIO output with push-pull
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 3;  // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), Set);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        break;
    case Pin_Mode::ALT_OPENDRAIN:
        config_bits = 3;    // AFIO output with open-drain
        if (speed == Output_Speed::SPEED_MAX) {
            mode_bits = 3;  // Output mode max speed
            write_bit(*this, GPIO_Regs::SPD, 1 << static_cast<uint32_t>(pin), Set);
        } else {
            mode_bits = static_cast<uint32_t>(speed);   // Output mode
        }
        break;
    }

    // Combine mode and configuration bits
    ctl_value |= (config_bits << (shift + 2)) | (mode_bits << shift);

    // Write the updated value back to the register
    write_register<uint32_t>(*this, reg, ctl_value);
}

void GPIO::set_pin_high(Pin_Bit_Mask pin)
{
    write_bit(*this, GPIO_Regs::BOP, static_cast<uint32_t>(pin), Set);
}

void GPIO::set_pin_low(Pin_Bit_Mask pin)
{
    write_bit(*this, GPIO_Regs::BC, static_cast<uint32_t>(pin), Set);
}

void GPIO::set_pin_pull(Pin_Bit_Mask pin, bool high)
{
    write_bit(*this, high ? GPIO_Regs::BOP : GPIO_Regs::BC, static_cast<uint32_t>(pin), Set);
}

void GPIO::write_pin(Pin_Bit_Mask pin, bool set)
{
    write_bit(*this, set ? GPIO_Regs::BOP : GPIO_Regs::BC, static_cast<uint32_t>(pin), Set);
}

bool GPIO::read_pin(Pin_Bit_Mask pin)
{
    return (read_bit(*this, GPIO_Regs::ISTAT, static_cast<uint32_t>(pin)) != Clear);
}

void GPIO::toggle_pin(Pin_Bit_Mask pin)
{
    write_bit(*this, GPIO_Regs::OCTL, static_cast<uint32_t>(pin), read_bit(*this, GPIO_Regs::OCTL, static_cast<uint32_t>(pin)) ? Clear : Set);
}

void GPIO::set_port(uint16_t data)
{
    write_register<uint32_t>(*this, GPIO_Regs::OCTL, static_cast<uint32_t>(data));
}

bool GPIO::get_pin_input_state(Pin_Bit_Mask pin)
{
    return (read_bit(*this, GPIO_Regs::ISTAT, static_cast<uint32_t>(pin)) != Clear);
}

bool GPIO::get_pin_output_state(Pin_Bit_Mask pin)
{
    return (read_bit(*this, GPIO_Regs::OCTL, static_cast<uint32_t>(pin)) != Clear);
}

uint16_t GPIO::get_port_input_state()
{
    return static_cast<uint16_t>(read_register<uint32_t>(*this, GPIO_Regs::ISTAT));
}

uint16_t GPIO::get_port_output_state()
{
    return static_cast<uint16_t>(read_register<uint32_t>(*this, GPIO_Regs::OCTL));
}

void GPIO::lock_pin(Pin_Number pin)
{
    uint32_t lock = LockValue;
    lock |= (1 << static_cast<uint32_t>(pin));

    // lock the pin by write/read in this order
    // write lock -> write pin -> write lock -> read 1x -> read 2x
    write_register(*this, GPIO_Regs::LOCK, lock);
    write_register(*this, GPIO_Regs::LOCK, (1 << static_cast<uint32_t>(pin)));
    write_register(*this, GPIO_Regs::LOCK, lock);
    lock = read_register<uint32_t>(*this, GPIO_Regs::LOCK);
    lock = read_register<uint32_t>(*this, GPIO_Regs::LOCK);
}

} // namespace gpio
