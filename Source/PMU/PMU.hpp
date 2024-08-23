// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "pmu_config.hpp"

namespace pmu {

class PMU {
public:
    PMU();

    // Reset
    void reset();
    // Clock
    void set_pclk_enable(bool enable);
    // Low volatage detection
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
    void set_deep_sleep_enable(Power_Driver driver, Bit_State state, PMU_Commands cmd);
    // Wakeup
    void wakeup_pin_enable();
    void wakeup_pin_disable();
    // Backup
    void backup_write_enable();
    void backup_write_disable();
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Reset_State_Flags flag);

    static constexpr uint32_t PMU_baseAddress = 0x40007000;

    inline volatile uint32_t *reg_address(PMU_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(PMU_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(PMU_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(PMU_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace pmu

extern pmu::PMU PMU_DEVICE;
