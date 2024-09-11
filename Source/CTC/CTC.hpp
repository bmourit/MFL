// gd32f30x CTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "ctc_config.hpp"

namespace ctc {

class CTC {
public:
    CTC() {}

    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_CTCRST, true);
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_CTCRST, false);
    }
    void set_trim_counter_enable(bool enable);
    void set_irc48m_trim(uint8_t value);
    void generate_reference_source_pulse();
    void set_hardware_auto_trim_enable(bool enable);
    void set_reference_source_polarity(Reference_Polarity polarity);
    void set_refenece_source_signal(Reference_Select reference);
    void set_reference_source_prescaler(Reference_Prescaler prescaler);
    void set_clock_trim_limit(uint8_t limit);
    void set_trim_counter_reload(uint16_t reload);
    uint16_t get_trim_counter_capture();
    bool get_trim_counter_direction();
    uint16_t get_trim_counter_reload();
    uint8_t get_trim_irc48m();
    bool get_flag(Status_Flags flag);
    void clear_flag(Clear_Flags flag);
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Clear_Flags flag);
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    static constexpr uintptr_t CTC_baseAddress = 0x4000C800;

    inline volatile uint32_t *reg_address(CTC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(CTC_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_CTC, true);
            RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_CTCRST, true);
            RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_CTCRST, false);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace ctc

extern ctc::CTC CTC_DEVICE;
