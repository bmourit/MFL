// gd32f30x TIMER peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "timer_config.hpp"

namespace timer {

class TIMER {
public:
    static Result<TIMER, TIMER_Error_Type> get_instance(TIMER_Base Base) {
        switch (Base) {
        case TIMER_Base::TIMER0_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER0_BASE, get_instance_for_base<TIMER_Base::TIMER0_BASE>()
                   );
        case TIMER_Base::TIMER1_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER1_BASE, get_instance_for_base<TIMER_Base::TIMER1_BASE>()
                   );
        case TIMER_Base::TIMER2_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER2_BASE, get_instance_for_base<TIMER_Base::TIMER2_BASE>()
                   );
        case TIMER_Base::TIMER3_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER3_BASE, get_instance_for_base<TIMER_Base::TIMER3_BASE>()
                   );
        case TIMER_Base::TIMER4_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER4_BASE, get_instance_for_base<TIMER_Base::TIMER4_BASE>()
                   );
        case TIMER_Base::TIMER5_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER5_BASE, get_instance_for_base<TIMER_Base::TIMER5_BASE>()
                   );
        case TIMER_Base::TIMER6_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER6_BASE, get_instance_for_base<TIMER_Base::TIMER6_BASE>()
                   );
        case TIMER_Base::TIMER7_BASE:
            return get_enum_instance<TIMER_Base, TIMER, TIMER_Error_Type>(
                       Base, TIMER_Base::TIMER7_BASE, get_instance_for_base<TIMER_Base::TIMER7_BASE>()
                   );
        default:
            return RETURN_ERROR(TIMER, TIMER_Error_Type::INVALID_TIMER);
        }
    }

    // Initialization
    void init();
    // Enable
    void enable();
    void disable();
    // Configure
    void configure(TIMER_Config* config) {
        if (config) {
            config_ = *config;
        }
        init();
    }
    // Events
    void update_event_enable();
    void update_event_disable();
    void generate_software_event(Event_Source event);
    void set_update_source(Update_Source source);
    // Counter
    void count_direction_up();
    void count_direction_down();
    void counter_alignment(Center_Align align);
    void set_counter_value(uint16_t counter);
    uint32_t read_counter();
    // Precaler
    void set_prescaler_reload(uint16_t prescaler, PSC_Reload reload);
    uint16_t read_prescaler();
    // Auto reload and repetition
    void auto_reload_shadow_enable();
    void auto_reload_shadow_disable();
    void repetition_value_config(uint16_t repeat);
    void set_auto_reload_value(uint16_t auto_reload);
    void set_pulse_mode(Pulse_Mode pulse);
    // DMA
    void dma_enable(DMAINTEN_Bits dma);
    void dma_disable(DMAINTEN_Bits dma);
    void set_dma_request_source(DMA_Request request);
    void dma_transfer_config(DMA_Transfer_Address address, DMA_Burst_Length length);
    // Break
    void break_configure(TIMER_Break *break_config);
    void break_init();
    void break_enable();
    void break_disable();
    void set_break_enable(bool enable);
    // Output
    void output_auto_enable();
    void output_auto_disable();
    void set_output_auto_enable(bool enable);
    void set_primary_output_enable(bool enable);
    void set_channel_shadow_enable(bool enable);
    void channel_shadow_update_configure(Shadow_Update update);
    // Output compare
    void output_compare_init(Timer_Channel channel);
    void output_compare_configure(Timer_Channel channel, TIMER_Output_Compare *output_compare);
    void set_output_mode(Timer_Channel channel, Output_Compare_Mode mode);
    void set_output_pulse(Timer_Channel channel, uint32_t pulse);
    void set_output_shadow(Timer_Channel channel, Output_Compare_Shadow shadow);
    void set_output_fast(Timer_Channel channel, Output_Compare_Fast fast);
    void set_output_clear(Timer_Channel channel, Output_Compare_Clear clear);
    void set_output_polarity(Timer_Channel channel, uint16_t polarity);
    void set_complement_output_polarity(Timer_Channel channel, uint16_t polarity);
    void set_channel_output_enable(Timer_Channel channel, bool enable);
    void set_compliment_output_enable(Timer_Channel channel, bool enable);
    // Input capture
    void input_capture_init(Timer_Channel channel);
    void input_capture_configure(Timer_Channel channel, TIMER_Input_Capture *input_capture);
    void set_input_capture_prescaler(Timer_Channel channel, Input_Capture_Prescaler prescaler);
    void input_pwm_capture_enable(Timer_Channel channel, TIMER_Input_Capture *pwm);
    // Hall
    void set_hall_mode_enable(bool enable);
    // Trigger
    void set_input_trigger(Trigger_Select trigger);
    void external_trigger_configure(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter);
    // Get input capture or output compare value
    uint32_t get_capture_compare_value(Timer_Channel channel);
    // Mode
    void set_master_output_trigger(Master_Control mode);
    void set_slave(Slave_Control mode);
    void set_master_slave_enable(bool enable);
    // Quadrature decoder
    void quadrature_decoder_configure(Decode_Mode mode, Polarity_Select polarity1, Polarity_Select polarity2);
    // Clocks
    void set_internal_clock();
    void set_internal_trigger_as_clock(Trigger_Select trigger);
    void set_external_trigger_as_clock(Trigger_Select trigger, Polarity_Select polarity, uint32_t filter);
    void set_clock_mode0(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter);
    void set_clock_mode1(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter);
    void clock_mode1_disable();
    // Config
    void set_write_chxval_enable(bool enable);
    void set_output_value_enable(bool enable);
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    // Interrupt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    inline volatile uint32_t *reg_address(TIMER_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

private:
    TIMER(TIMER_Base Base) : base_index_(Base),
        TIMER_pclk_info_(TIMER_pclk_index[static_cast<int>(Base)]),
        base_address_(TIMER_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(TIMER_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(TIMER_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(TIMER_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    TIMER_Base base_index_;
    TIMER_Clock_Config TIMER_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

    // Default dummy config
    TIMER_Config default_config = {};
    TIMER_Config& config_ = default_config;

    template <TIMER_Base Base>
    static TIMER& get_instance_for_base() {
        static TIMER instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(TIMER_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(TIMER_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace timer
