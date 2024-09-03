// gd32f30x RCU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "rcu_config.hpp"

namespace rcu {

class RCU {
public:
    RCU() {}

    // Reset
    void reset();
    // Set peripheral clock
    void set_pclk_enable(RCU_PCLK pclk, bool enable);
    // Set peripheral clock sleep
    void set_pclk_sleep_enable(RCU_PCLK_Sleep pclk, bool enable);
    // Set peripheral clock reset
    void set_pclk_reset_enable(RCU_PCLK_Reset pclk, bool enable);
    // Backup clock reset
    void set_backup_reset_enable(bool enable);
    // System clock source
    void set_system_source(System_Clock_Source system_source);
    System_Clock_Source get_system_source();
    // AHB/ABP1/APB2 prescaler
    void set_ahb_prescaler(AHB_Prescaler prescaler);
    void set_apb1_prescaler(APB_Prescaler prescaler);
    void set_apb2_prescaler(APB_Prescaler prescaler);
    // CKOUT0 source
    void set_ckout0_source(CKOUT0_Source source);
    // PLL
    void set_pll_config(PLL_Source source, PLLMF_Select multiplier);
    PLL_Source get_pll_source();
    void set_pll_presel(PLL_Presel presel);
    PLL_Presel get_pll_presel();
    // PREDV0
    void set_predv0_config(uint32_t div);
    // ADC/USB prescaler
    void set_adc_prescaler(ADC_Prescaler prescaler);
    void set_usb_prescaler(USB_Prescaler prescaler);
    // RTC source
    void set_rtc_source(RTC_Source source);
    // CK48M source
    void set_ck48m_source(CK48M_Source source);
    // LXTAL campability
    void set_lxtal_drive_capability(LXTAL_Drive drive);
    // OSCI
    void set_osci_enable(OSCI_Select osci, bool enable);
    bool is_osci_stable(OSCI_Select osci);
    // Clock frequency
    uint32_t get_clock_frequency(Clock_Frequency clock);
    // Bypass
    void bypass_mode_enable(OSCI_Select osci);
    void bypass_mode_disable(OSCI_Select osci);
    // Adjust
    void set_irc8m_adjustment_value(uint32_t value);
    // Monitor
    void hxtal_monitor_enable();
    void hxtal_monitor_disable();
    // Deep sleep
    void set_deep_sleep_voltage(DeepSleep_Voltage voltage);
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_all_reset_flags();
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Clear_Flags flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    // System startup functionality
    void update_system_clock();
    inline uint32_t calculate_pll_clock();
    virtual void clocks_init();
    // Optional
    virtual void mfl_init() {};
    virtual void device_init() {};

    static constexpr uint32_t RCU_baseAddress = 0x40021000;

    inline volatile uint32_t *reg_address(RCU_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(RCU_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(RCU_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(RCU_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    // Get value helpers
    bool get_value(Status_Flags flag) const {
        const auto &info = status_flag_index[static_cast<size_t>(flag)];
        uint32_t reg_value = read_register<uint32_t>(info.register_offset);

        const uint32_t width = info.bit_info & 0xFF;
        const uint32_t bitno = info.bit_info >> 16;

        reg_value >>= bitno;
        reg_value &= ((1 << width) - 1);

        return reg_value;
    }

    bool get_value(Interrupt_Flags flag) const {
        const auto &info = interrupt_type_index[static_cast<size_t>(flag)];
        uint32_t reg_value = read_register<uint32_t>(info.register_offset);

        const uint32_t width = info.bit_info & 0xFF;
        const uint32_t bitno = info.bit_info >> 16;

        reg_value >>= bitno;
        reg_value &= ((1 << width) - 1);

        return reg_value;
    }
};

} // namespace rcu

extern unsigned int SystemCoreClock;

extern rcu::RCU RCU_DEVICE;
