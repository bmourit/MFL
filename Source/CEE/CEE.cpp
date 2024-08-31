// gd32f30x CEE register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "CEE.hpp"

namespace cee {

void CEE::enhanced_mode_enable()
{
    write_bit(*this, CEE_Regs::CEE, static_cast<uint32_t>(CEE_Bits::CEE_EN), Set);
}

void CEE::enhanced_mode_disable()
{
    write_bit(*this, CEE_Regs::CEE, static_cast<uint32_t>(CEE_Bits::CEE_EN), Clear);
}

void CEE::set_enhanced_mode_enable(bool enable)
{
    write_bit(*this, CEE_Regs::CEE, static_cast<uint32_t>(CEE_Bits::CEE_EN), enable ? Set : Clear);
}

} // namespace cee

cee::CEE CEE_DEVICE;
