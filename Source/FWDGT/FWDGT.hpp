// gd32f30x FWDGT peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "fwdgt_config.hpp"

namespace fwdgt {

class FWDGT {
public:
    FWDGT() {}

    void enable();
    void write_enable();
    void write_disable();
    void set_write_enable(bool enable);
    bool set_prescaler(Prescaler_Values value);
    bool set_reload_prescaler(uint16_t reload, Prescaler_Values value);
    bool set_reload(uint16_t reload);
    void reload_counter();
    bool get_flag(State_Flags flag);

    static constexpr uint32_t FWDGT_baseAddress = 0x40003000;

    inline volatile uint32_t *reg_address(FWDGT_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(FWDGT_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(FWDGT_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(FWDGT_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace fwdgt

extern fwdgt::FWDGT FWDGT_DEVICE;
