// gd32f30x RTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "rtc_config.hpp"

namespace rtc {

class RTC {
public:
    RTC() {}

    void set_prescaler(uint32_t prescaler);
    void set_alarm(uint32_t alarm);
    uint32_t get_divider();
    void start_configuration();
    void stop_configuration();
    void lwoff_wait();
    void sync_register_wait();
    uint32_t get_counter();
    void set_counter(uint32_t counter);
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    static constexpr uintptr_t RTC_baseAddress = 0x40002800;

    inline volatile uint32_t *reg_address(RTC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(RTC_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_RTC, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace rtc

extern rtc::RTC RTC_DEVICE;
