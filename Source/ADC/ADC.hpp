// gd32f30x ADC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "adc_config.hpp"

#define MAX_ADCS    3

namespace adc {

class ADC {
public:
    static ADC& instance(ADC_Base Base) {
        static ADC instances[MAX_ADCS] = {
            ADC(ADC_Base::ADC0_BASE),
            ADC(ADC_Base::ADC1_BASE),
            ADC(ADC_Base::ADC2_BASE),
        };
        return instances[static_cast<int>(Base)];
    }

    // Enable
    void enable();
    void disable();

    // Reset
    void reset();

    // Clocks
    void set_pclk_enable(bool enable);

    // Configuration
    void calibration_enable();
    void dma_enable(bool enable);
    void vrefint_temp_enable(bool enaable);
    void set_resolution(ADC_Resolution resolution);
    void discontinuous_mode_config(Channel_Group_Type channel_group, uint8_t length);
    void set_mode(Sync_Mode mode);
    void set_special_function(Special_Function function, bool enable);
    void set_data_alignment(Data_Alignment align);
    void set_channel_length(Channel_Group_Type channel_group, uint32_t length);
    void regular_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time);
    void inserted_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time);
    void inserted_channel_offset_config(Inserted_Channel inserted_channel, uint16_t offset);

    // Triggers
    void set_external_trigger_enable(Channel_Group_Type channel_group, bool enable);
    void set_external_group_source(Channel_Group_Type channel_group, External_Trigger_Source source);
    void set_software_trigger_group(Channel_Group_Type channel_group);

    // Read data
    uint16_t get_regular_data();
    uint16_t get_inserted_data(Inserted_Channel inserted_channel);
    uint32_t get_sync_mode_convert_value();

    // Flags
    bool get_status_flag(STAT_Bits flag);
    void clear_status_flag(STAT_Bits flag);

    // Interrupt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);

    // Interrupts
    void interrupt_enable(Interrupt_Type interrupt);
    void interrupt_disable(Interrupt_Type interrupt);
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    // Watchdog
    void single_channel_watchdog_enable(ADC_Channel channel);
    void group_channel_watchdog_enable(Channel_Group_Type channel_group);
    void watchdog_disable();
    void set_watchdog_threshold(uint16_t low, uint16_t high);

    // Oversample
    void set_oversampling_configuration(Oversampling_Convertion mode, Oversampling_Shift shift, Oversampling_Ratio ratio);
    void set_oversampling_enable(bool enable);

    inline volatile uint32_t *reg_address(ADC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    inline volatile uint32_t *reg_address(ADC_Regs reg, uint32_t extra_offset) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + (static_cast<uint32_t>(reg) + extra_offset));
    }

    ADC_Clock_Config ADC_pclk_info_;

private:
    explicit ADC(ADC_Base Base) : ADC_pclk_info_(get_clock_config(Base)), base_address_(get_base_address(Base)) {}

    uint32_t base_address_;

    template<typename T>
    inline T read_register(ADC_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(ADC_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    ADC_Clock_Config get_clock_config(ADC_Base Base) {
        return ADC_pclk_index[static_cast<int>(Base)];
    }

    uint32_t get_base_address(ADC_Base Base) {
        return ADC_baseAddress[static_cast<int>(Base)];
    }
};

}  // namespace adc

// Usage example:
// adc::ADC& adc0 = adc::ADC::instance(adc::ADC_Base::ADC0_BASE);
