// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "pmu_config.hpp"

namespace pmu {

class PMU {
public:
    PMU() {}

    // Reset
    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, true);
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, false);
    }
    // Low voltage detection
    void lvd_enable(LVD_Threshold threshold);
    void lvd_disable();
    // Output voltage
    void set_ldo_output(Output_Voltage level);
    // High driver mode
    void high_driver_switch(bool enable);
    void high_driver_enable();
    void high_driver_disable();
    void set_high_driver_enable(bool enable);
    // Low driver mode
    void low_driver_on_deep_sleep_enable();
    void low_driver_on_deep_sleep_disable();
    void set_low_driver_on_deep_sleep(Low_Driver mode);
    // Driver selection for power modes
    void set_driver_on_low_power(Power_Driver driver);
    void set_driver_on_normal_power(Power_Driver driver);
    // Sleep/Standby
    void set_standby_enable();
    void set_sleep_enable(PMU_Commands cmd);
    void set_deep_sleep_enable(Power_Driver driver, PMU_Commands command, bool enable);
    // Wakeup
    void wakeup_pin_enable();
    void wakeup_pin_disable();
    // Backup
    void backup_write_enable();
    void backup_write_disable();
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Clear_Flags flag);

    static constexpr uintptr_t PMU_baseAddress = 0x40007000;

    inline volatile uint32_t *reg_address(PMU_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(PMU_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_PMU, true);
            is_clock_enabled = true;
        }
    }

private:
    static bool is_clock_enabled;
};

} // namespace pmu

extern pmu::PMU PMU_DEVICE;
