// gd32f30x BKP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "bkp_config.hpp"

namespace bkp {

class BKP {
public:
    BKP() {}

    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_BKPIRST, true);
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_BKPIRST, false);
    }
    void set_data(Backup_Data datax, uint16_t data);
    uint16_t get_data(Backup_Data datax);

    void set_rtc_output_calibration_enable(bool enable);
    void set_rtc_output_signal_enable(bool enable);
    void set_rtc_output_pulse(Output_Pulse pulse);
    void set_rtc_clock_divider(Clock_Divider divider);
    void set_rtc_clock_calibration_type(Calibration_Type type);
    void set_rtc_calibration_value(uint8_t value);

    void set_tamper_detection_enable(bool enable);
    void set_tamper_level(Tamper_Level level);
    void set_tamper_interrupt_enable(bool enable);
    void tamper_interrupt_enable();
    void tamper_interrupt_disable();

    bool get_flag(Status_Flags flag);
    void clear_flag(Clear_Flags flag);
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Clear_Flags flag);

    static constexpr uint32_t BKP_baseAddress = 0x40006C00;

    inline volatile uint32_t *reg_address(BKP_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(BKP_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_PMU, true);
            is_clock_enabled = true;
        }
    }

    template<typename T>
    inline T read_register(BKP_Regs reg) const {
        ensure_clock_enabled();
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(BKP_Regs reg, T value) {
        ensure_clock_enabled();
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    static bool is_clock_enabled;
};

} // namespace bkp

extern bkp::BKP BKP_DEVICE;
