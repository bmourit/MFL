// gd32f30x ARMDBG debug in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "DBG.hpp"

namespace armdbg {

void ARMDBG::reset()
{
    write_register(DBG_Regs::CTL0, 0x00000000);
}

uint32_t ARMDBG::get_debug_id()
{
    return read_register<uint32_t>(DBG_Regs::ID);
}

void ARMDBG::debug_with_trace_enable()
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRACE_IOEN), 1);
}

void ARMDBG::debug_with_trace_disable()
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRACE_IOEN), 0);
}

void ARMDBG::set_debug_with_trace_enable(bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRACE_IOEN), enable ? 1 : 0);
}

void ARMDBG::peripheral_debug_enable(Debug_Peripheral peripheral)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(peripheral), 1);
}

void ARMDBG::peripheral_debug_disable(Debug_Peripheral peripheral)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(peripheral), 0);
}

void ARMDBG::set_peripheral_debug_enable(Debug_Peripheral peripheral, bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(peripheral), enable ? 1 : 0);
}

void ARMDBG::debug_during_low_power_enable(Low_Power_Debug type)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(type), 1);
}

void ARMDBG::debug_during_low_power_disable(Low_Power_Debug type)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(type), 0);
}

void ARMDBG::set_debug_during_low_power_enable(Low_Power_Debug type, bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(type), enable ? 1 : 0);
}

} // namespace armdbg

armdbg::ARMDBG ARMDBG_DEVICE;
