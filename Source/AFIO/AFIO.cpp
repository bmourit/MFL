// gd32f30x AFIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "AFIO.hpp"

// Initialize the static member
bool gpio::AFIO::is_clock_enabled = false;

namespace gpio {

void AFIO::set_remap(Pin_Remap_Select remap) {
    const auto &info = remap_index[static_cast<int>(remap)];
    write_bit(*this, info.register_offset, info.bit_info, static_cast<uint32_t>(info.type), true);
}

void AFIO::set_exti_source(Source_Port port, Pin_Number pin) {
    uint32_t source = (0xF << (4 * (static_cast<uint32_t>(pin) & 0x3)));

    if (pin < Pin_Number::PIN_4) {
        uint32_t extiss_reg = read_register<uint32_t>(*this, AFIO_Regs::EXTISS0, true);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(*this, AFIO_Regs::EXTISS0, extiss_reg, true);
    } else if (pin < Pin_Number::PIN_8) {
        uint32_t extiss_reg = read_register<uint32_t>(*this, AFIO_Regs::EXTISS1, true);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(*this, AFIO_Regs::EXTISS1, extiss_reg, true);
    } else if (pin < Pin_Number::PIN_12) {
        uint32_t extiss_reg = read_register<uint32_t>(*this, AFIO_Regs::EXTISS2, true);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(*this, AFIO_Regs::EXTISS2, extiss_reg, true);
    } else {
        uint32_t extiss_reg = read_register<uint32_t>(*this, AFIO_Regs::EXTISS3, true);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(*this, AFIO_Regs::EXTISS3, extiss_reg, true);
    }
}

void AFIO::set_output_event(Event_Port port, Pin_Number pin) {
    uint32_t reg = read_register<uint32_t>(*this, AFIO_Regs::EC, true);
    reg &= (~(0x1C0 | 0xF));
    reg |= (static_cast<uint32_t>(port) << 4);
    reg |= static_cast<uint32_t>(pin);
    write_register<uint32_t>(*this, AFIO_Regs::EC, reg, true);

}

void AFIO::set_output_event_enable(bool enable) {
    write_bit(*this, AFIO_Regs::EC, static_cast<uint32_t>(EC_Bits::EOE), enable ? Set : Clear, true);
}

void AFIO::set_compensation(bool enable) {
    write_bit(*this, AFIO_Regs::CPSCTL, static_cast<uint32_t>(CPSCTL_Bits::CPS_EN), enable ? Set : Clear, true);
}

bool AFIO::get_compensation() {
    return (read_bit(*this, AFIO_Regs::CPSCTL, static_cast<uint32_t>(CPSCTL_Bits::CPS_RDY), true) != 0);
}

} // namespace gpio

gpio::AFIO AFIO_DEVICE;
