// gd32f30x DAC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "dac_config.hpp"

namespace dac {

class DAC {
public:
    DAC() {}

    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_DACRST, true);
        RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_DACRST, false);
    }
    void enable(Internal_Device instance);
    void disable(Internal_Device instance);
    void set_dma_enable(Internal_Device instance, bool enable);
    void set_output_buffer_enable(Internal_Device instance, bool enable);
    uint16_t get_output_value(Internal_Device instance);
    void set_data(Internal_Device instance, Data_Align align, uint16_t data);
    void set_trigger_enable(Internal_Device instance, bool enable);
    void set_trigger_source(Internal_Device instance, Trigger_Source source);
    void set_software_trigger_enable(Internal_Device instance, bool enable);
    void set_wave_mode(Internal_Device instance, Wave_Type type);
    void set_wave_bit_width(Internal_Device instance, Bit_Width width);
    void set_lfsr_noise(Internal_Device instance, LFSR_Noise noise);
    void set_triangle_amplitude(Internal_Device instance, Triangle_Amplitude amplitude);
    void set_dual_mode_enable(bool enable);
    void set_dual_software_trigger_enable(bool enable);
    void set_dual_output_buffer_enable(bool enable);
    void set_dual_data(Data_Align align, uint16_t data0, uint16_t data1);

    static constexpr uint32_t DAC_baseAddress = 0x40007400;

    inline volatile uint32_t *reg_address(DAC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(DAC_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    inline void ensure_clock_enabled() const {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_DAC, true);
            RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_DACRST, true);
            RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_DACRST, false);
            is_clock_enabled = true;
        }
    }

    template<typename T>
    inline T read_register(DAC_Regs reg) const {
        ensure_clock_enabled();
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(DAC_Regs reg, T value) {
        ensure_clock_enabled();
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    static bool is_clock_enabled;
};

} // namespace dac

extern dac::DAC DAC_DEVICE;
