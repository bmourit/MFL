// gd32f30x ADC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "adc_config.hpp"

namespace adc {

class ADC {
public:
    static Result<ADC, ADC_Error_Type> get_instance(ADC_Base Base) {
        switch (Base) {
        case ADC_Base::ADC0_BASE:
            return get_enum_instance<ADC_Base, ADC, ADC_Error_Type>(
                       Base, ADC_Base::ADC0_BASE, get_instance_for_base<ADC_Base::ADC0_BASE>()
                   );
        case ADC_Base::ADC1_BASE:
            return get_enum_instance<ADC_Base, ADC, ADC_Error_Type>(
                       Base, ADC_Base::ADC1_BASE, get_instance_for_base<ADC_Base::ADC1_BASE>()
                   );
        case ADC_Base::ADC2_BASE:
            return get_enum_instance<ADC_Base, ADC, ADC_Error_Type>(
                       Base, ADC_Base::ADC2_BASE, get_instance_for_base<ADC_Base::ADC2_BASE>()
                   );
        default:
            return RETURN_ERROR(ADC, ADC_Error_Type::INVALID_ADC);
        }
    }

    // Enable
    void enable();
    void disable();
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
    // Watchdog
    void single_channel_watchdog_enable(ADC_Channel channel);
    void group_channel_watchdog_enable(Channel_Group_Type channel_group);
    void watchdog_disable();
    void set_watchdog_threshold(uint16_t low, uint16_t high);
    // Oversample
    void set_oversampling_configuration(Oversampling_Convertion mode, Oversampling_Shift shift, Oversampling_Ratio ratio);
    void set_oversampling_enable(bool enable);
    // Flags
    bool get_flag(STAT_Bits flag);
    void clear_flag(STAT_Bits flag);
    // Interrupt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    inline volatile uint32_t *reg_address(ADC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    inline volatile uint32_t *reg_address(ADC_Regs reg, uint32_t extra_offset) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg) + extra_offset);
    }

private:
    ADC(ADC_Base Base) : ADC_pclk_info_(ADC_pclk_index[static_cast<int>(Base)]),
        base_address_(ADC_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(ADC_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(ADC_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(ADC_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    ADC_Clock_Config ADC_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

    template <ADC_Base Base>
    static ADC& get_instance_for_base() {
        static ADC instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(ADC_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(ADC_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

}  // namespace adc
