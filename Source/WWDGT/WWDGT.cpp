// gd32f30x WWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "WWDGT.hpp"

namespace wwdgt {

void WWDGT::init()
{
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_WWDGT, true);
}

void WWDGT::reset()
{
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_WWDGTRST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_WWDGTRST, false);
}

void WWDGT::enable()
{
    write_bit(*this, WWDGT_Regs::CTL, static_cast<uint32_t>(CTL_Bits::WDGTEN), Set);
}

void WWDGT::update_counter(uint16_t value)
{
    write_bit(*this, WWDGT_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CNT), static_cast<uint32_t>(value));
}

void WWDGT::setup(uint16_t value, uint16_t window, Prescaler_Values prescaler)
{
    write_bit(*this, WWDGT_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CNT), static_cast<uint32_t>(value));
    write_bit(*this, WWDGT_Regs::CFG, static_cast<uint32_t>(CFG_Bits::WIN), static_cast<uint32_t>(window));
    write_bit(*this, WWDGT_Regs::CFG, static_cast<uint32_t>(CFG_Bits::PSC), static_cast<uint32_t>(prescaler));
}

bool WWDGT::get_flag()
{
    return (read_bit(*this, WWDGT_Regs::STAT, static_cast<uint32_t>(STAT_Bits::EWIF) != Clear));
}

void WWDGT::clear_flag()
{
    write_register(WWDGT_Regs::STAT, Clear);
}

void WWDGT::interrupt_enable()
{
    write_bit(*this, WWDGT_Regs::CFG, static_cast<uint32_t>(CFG_Bits::EWIE), Set);
}

void WWDGT::interrupt_disable()
{
    write_bit(*this, WWDGT_Regs::CFG, static_cast<uint32_t>(CFG_Bits::EWIE), Clear);
}

} // namespace wwdgt

// Instantiate class of global device
wwdgt::WWDGT WWDGT_DEVICE;
