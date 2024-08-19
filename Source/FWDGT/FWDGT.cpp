// gd32f30x FWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "FWDGT.hpp"

namespace fwdgt {

void FWDGT::enable()
{
    write_register(FWDGT_Regs::CTL, 0x0000CCCC);
}

void FWDGT::write_enable()
{
    write_register(FWDGT_Regs::CTL, 0x00005555);
}

void FWDGT::write_disable()
{
    write_register(FWDGT_Regs::CTL, 0x00000000);
}

void FWDGT::set_write_enable(bool enable)
{
    write_register(FWDGT_Regs::CTL, enable ? 0x00005555 : 0x00000000);
}

bool FWDGT::set_prescaler(Prescaler_Values value)
{
    uint32_t timeout = 0x000FFFFF;
    uint32_t status = 0;

    // Enable write access
    write_register(FWDGT_Regs::CTL, 0x00005555);

    // Wait for PUD flag cleaar
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::PUD));
    } while ((status != 0) && (--timeout > 0));

    if (status != 0) {
        return true;
    }

    // Set the prescaler
    write_register(FWDGT_Regs::PSC, value);

    return false;
}

bool FWDGT::set_reload_prescaler(uint16_t reload, Prescaler_Values prescaler)
{
    uint32_t timeout = 0x000FFFFF;
    uint32_t status = 0;

    // Enable write access
    write_register(FWDGT_Regs::CTL, 0x00005555);

    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::PUD));
    } while ((status != 0U) && (--timeout > 0U));

    if (status != 0U) {
        return true;
    }

    // Initialize prescaler
    write_register(FWDGT_Regs::PSC, prescaler);

    timeout = 0x000FFFFF;
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RUD));
    } while ((status != 0) && (--timeout > 0));

    if (status != 0) {
        return true;
    }

    write_bit(*this, FWDGT_Regs::RLD, static_cast<uint32_t>(RLD_Bits::RLD), static_cast<uint32_t>(reload));

    // counter reload
    write_register(FWDGT_Regs::CTL, 0x0000AAAA);

    return false;
}

bool FWDGT::set_reload(uint16_t reload)
{
    uint32_t timeout = 0x000FFFFF;
    uint32_t status = 0;

    // Enable write access
    write_register(FWDGT_Regs::CTL, 0x00005555);

    // Wait for RUD flag to clear
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RUD));
    } while ((status != 0U) && (--timeout > 0));

    if (status != 0U) {
        return true;
    }

    write_bit(*this, FWDGT_Regs::RLD, static_cast<uint32_t>(RLD_Bits::RLD), static_cast<uint32_t>(reload));

    return false;
}

void FWDGT::reload_counter()
{
    write_register(FWDGT_Regs::CTL, 0x0000AAAA);
}

bool FWDGT::get_flag(State_Flags flag)
{
    return (read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(flag)) != 0) ? true : false;

}

} // namespace fwdgt

fwdgt::FWDGT FWDGT_DEVICE;
