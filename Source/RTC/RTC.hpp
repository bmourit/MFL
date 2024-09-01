// gd32f30x RTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
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
    void interrupt_enable(Interrupt_Type type);
    void interrupt_disable(Interrupt_Type type);

    static constexpr uint32_t RTC_baseAddress = 0x40002800;

    inline volatile uint32_t *reg_address(RTC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(RTC_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(RTC_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(RTC_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace rtc

extern rtc::RTC RTC_DEVICE;
