// gd32f30x STARTUP clock initialization and startup functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BUZZER.hpp"

#include "RegRW.hpp"
#include "FMC.hpp"
#include "PMU.hpp"
#include "RCU.hpp"
#include "startup.hpp"

namespace startup {

class STARTUP {
public:
    STARTUP() {}

    virtual void startup_init();
    // Optional
    virtual void mfl_init() {};
    virtual void device_init() {};

    static constexpr uint32_t RCU_baseAddress = 0x40021000;

    inline volatile uint32_t *reg_address(rcu::RCU_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(RCU_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}
};

} // namespace startup

extern startup::STARTUP STARTUP_DEVICE;
