// gd32f30x ARMDBG debug in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "DBG.hpp"

namespace armdbg {

void ARMDBG::reset()
{
    write_register(DBG_Regs::CTL0, Clear);
}

uint32_t ARMDBG::get_debug_id()
{
    return read_register<uint32_t>(DBG_Regs::ID);
}

void ARMDBG::set_debug_with_trace_enable(bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRACE_IOEN), enable ? Set : Clear);
}

void ARMDBG::set_peripheral_debug_enable(Debug_Peripheral peripheral, bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(peripheral), enable ? Set : Clear);
}

void ARMDBG::set_debug_during_low_power_enable(Low_Power_Debug type, bool enable)
{
    write_bit(*this, DBG_Regs::CTL0, static_cast<uint32_t>(type), enable ? Set : Clear);
}

} // namespace armdbg

armdbg::ARMDBG ARMDBG_DEVICE;
