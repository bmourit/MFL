// gd32f30x WWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "wwdgt_config.hpp"

namespace wwdgt {

class WWDGT {
public:
    explicit WWDGT() {}

    void init();
    void reset();
    void enable();
    void update_counter(uint16_t value);
    void setup(uint16_t value, uint16_t window, Prescaler_Values prescaler);
    bool get_flag();
    void clear_flag();
    void interrupt_enable();
    void interrupt_disable();

    static constexpr uint32_t WWDGT_baseAddress = 0x40002C00;

    inline volatile uint32_t *reg_address(WWDGT_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(reinterpret_cast<uintptr_t>(WWDGT_baseAddress) + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(WWDGT_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(WWDGT_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace wwdgt

extern wwdgt::WWDGT WWDGT_DEVICE;
