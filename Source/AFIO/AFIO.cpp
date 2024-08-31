// gd32f30x AFIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "AFIO.hpp"

namespace gpio {

AFIO::AFIO() {
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_AF, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, false);
}

void AFIO::reset() {
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_AFRST, false);
}

void AFIO::set_pclk_enable(bool enable) {
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_AF, enable);
}

void AFIO::set_remap(Pin_Remap_Select remap) {
    const auto &info = remap_index[static_cast<int>(remap)];
    write_bit(*this, info.register_offset, info.bit_info, static_cast<uint32_t>(info.type));
}

void AFIO::set_exti_source(Source_Port port, Pin_Number pin) {
    uint32_t source = (0xF << (4 * (static_cast<uint32_t>(pin) & 0x3)));

    if (pin < Pin_Number::PIN_4) {
        uint32_t extiss_reg = read_register<uint32_t>(AFIO_Regs::EXTISS0);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(AFIO_Regs::EXTISS0, extiss_reg);
    } else if (pin < Pin_Number::PIN_8) {
        uint32_t extiss_reg = read_register<uint32_t>(AFIO_Regs::EXTISS1);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(AFIO_Regs::EXTISS1, extiss_reg);
    } else if (pin < Pin_Number::PIN_12) {
        uint32_t extiss_reg = read_register<uint32_t>(AFIO_Regs::EXTISS2);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(AFIO_Regs::EXTISS2, extiss_reg);
    } else {
        uint32_t extiss_reg = read_register<uint32_t>(AFIO_Regs::EXTISS3);
        extiss_reg &= ~source;
        extiss_reg |= (static_cast<uint32_t>(port) << (4 * (static_cast<uint32_t>(pin) & 0x3)));
        write_register(AFIO_Regs::EXTISS3, extiss_reg);
    }
}

void AFIO::set_output_event(Event_Port port, Pin_Number pin) {
    uint32_t reg = read_register<uint32_t>(AFIO_Regs::EC);
    reg &= (~(0x1C0 | 0xF));
    reg |= (static_cast<uint32_t>(port) << 4);
    reg |= static_cast<uint32_t>(pin);
    write_register(AFIO_Regs::EC, reg);

}

void AFIO::output_event_enable(void) {
    write_bit(*this, AFIO_Regs::EC, static_cast<uint32_t>(EC_Bits::EOE), Set);
}

void AFIO::output_event_disable(void) {
    write_bit(*this, AFIO_Regs::EC, static_cast<uint32_t>(EC_Bits::EOE), Clear);
}

void AFIO::set_compensation(bool enable) {
    write_bit(*this, AFIO_Regs::CPSCTL, static_cast<uint32_t>(CPSCTL_Bits::CPS_EN), enable ? Set : Clear);
}

bool AFIO::get_compensation() {
    return (read_bit(*this, AFIO_Regs::CPSCTL, static_cast<uint32_t>(CPSCTL_Bits::CPS_RDY)) != 0);
}

} // namespace gpio

gpio::AFIO AFIO_DEVICE;
