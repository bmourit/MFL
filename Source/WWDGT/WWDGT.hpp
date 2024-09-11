// gd32f30x WWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "wwdgt_config.hpp"

namespace wwdgt {

class WWDGT {
public:
    explicit WWDGT() {}

    void reset();
    void enable();
    void update_counter(uint16_t value);
    void setup(uint16_t value, uint16_t window, Prescaler_Values prescaler);
    bool get_flag();
    void clear_flag();
    void set_interrupt_enable(bool enable);

    static constexpr uintptr_t WWDGT_baseAddress = 0x40002C00;

    inline volatile uint32_t *reg_address(WWDGT_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(WWDGT_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_WWDGT, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace wwdgt

extern wwdgt::WWDGT WWDGT_DEVICE;
