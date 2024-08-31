// gd32f30x FWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "FWDGT.hpp"

namespace fwdgt {

constexpr uint32_t EnableValue = 0x0000CCCC;
constexpr uint32_t WriteEnable = 0x00005555;
constexpr uint32_t TimeoutValue = 0x000FFFFF;
constexpr uint32_t ReloadValue = 0x0000AAAA;

void FWDGT::enable()
{
    write_register(FWDGT_Regs::CTL, EnableValue);
}

void FWDGT::write_enable()
{
    write_register(FWDGT_Regs::CTL, WriteEnable);
}

void FWDGT::write_disable()
{
    write_register(FWDGT_Regs::CTL, Clear);
}

void FWDGT::set_write_enable(bool enable)
{
    write_register(FWDGT_Regs::CTL, enable ? WriteEnable : Clear);
}

bool FWDGT::set_prescaler(Prescaler_Values value)
{
    uint32_t timeout = TimeoutValue;
    uint32_t status = Clear;

    // Enable write access
    write_register(FWDGT_Regs::CTL, WriteEnable);

    // Wait for PUD flag cleaar
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::PUD));
    } while ((status != Clear) && (--timeout > Clear));

    if (status != Clear) {
        return true;
    }

    // Set the prescaler
    write_register(FWDGT_Regs::PSC, value);

    return false;
}

bool FWDGT::set_reload_prescaler(uint16_t reload, Prescaler_Values prescaler)
{
    uint32_t timeout = TimeoutValue;
    uint32_t status = Clear;

    // Enable write access
    write_register(FWDGT_Regs::CTL, WriteEnable);

    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::PUD));
    } while ((status != Clear) && (--timeout > Clear));

    if (status != Clear) {
        return true;
    }

    // Initialize prescaler
    write_register(FWDGT_Regs::PSC, prescaler);

    timeout = TimeoutValue;
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RUD));
    } while ((status != Clear) && (--timeout > Clear));

    if (status != Clear) {
        return true;
    }

    write_bit(*this, FWDGT_Regs::RLD, static_cast<uint32_t>(RLD_Bits::RLD), static_cast<uint32_t>(reload));

    // counter reload
    write_register(FWDGT_Regs::CTL, ReloadValue);

    return false;
}

bool FWDGT::set_reload(uint16_t reload)
{
    uint32_t timeout = TimeoutValue;
    uint32_t status = Clear;

    // Enable write access
    write_register(FWDGT_Regs::CTL, WriteEnable);

    // Wait for RUD flag to clear
    do {
        status = read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RUD));
    } while ((status != Clear) && (--timeout > Clear));

    if (status != Clear) {
        return true;
    }

    write_bit(*this, FWDGT_Regs::RLD, static_cast<uint32_t>(RLD_Bits::RLD), static_cast<uint32_t>(reload));

    return false;
}

void FWDGT::reload_counter()
{
    write_register(FWDGT_Regs::CTL, ReloadValue);
}

bool FWDGT::get_flag(Status_Flags flag)
{
    return (read_bit(*this, FWDGT_Regs::STAT, static_cast<uint32_t>(flag)) != Clear);

}

} // namespace fwdgt

fwdgt::FWDGT FWDGT_DEVICE;
