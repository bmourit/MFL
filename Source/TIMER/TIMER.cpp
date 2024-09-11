// gd32f30x TIMER peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "TIMER.hpp"

// Initialize the static member
bool timer::TIMER::is_clock_enabled = false;

namespace timer {

void TIMER::init() {
    write_register(*this, TIMER_Regs::PSC, config_.prescaler);

    if ((base_index_ != TIMER_Base::TIMER5_BASE) && (base_index_ != TIMER_Base::TIMER6_BASE)) {
        write_bits(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), static_cast<uint32_t>(config_.counting_direction),
                   static_cast<uint32_t>(CTL0_Bits::CAM), static_cast<uint32_t>(config_.align));
    }
    write_register(*this, TIMER_Regs::CAR, config_.period);

    if ((base_index_ != TIMER_Base::TIMER5_BASE) && (base_index_ != TIMER_Base::TIMER6_BASE)) {
        write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKDIV), static_cast<uint32_t>(config_.divider));
    }
    if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
        write_register(*this, TIMER_Regs::CREP, config_.repetition_count);
    }

    write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(SWEVG_Bits::UPG), Set);
}

void TIMER::enable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CEN), Set);
}

void TIMER::disable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CEN), Clear);
}

void TIMER::pin_config_init() {
    auto configure_pin = [](gpio::GPIO& port, const TIMER_Pin_Config& pin_config) {
        port.init_pin(pin_config.pin, pin_config.mode, pin_config.speed);
    };

    auto result = gpio::GPIO::get_instance(pin_config_.gpio_port);
    if (result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& port = result.value();
    configure_pin(port, pin_config_);
}

void TIMER::update_event_enable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPDIS), Clear);
}

void TIMER::update_event_disable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPDIS), Set);
}

void TIMER::generate_software_event(Event_Source event) {
    write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(event), Set);
}

void TIMER::set_update_source(Update_Source source) {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPS), (source == Update_Source::REGULAR_SOURCE) ? Set : Clear);
}

void TIMER::count_direction_up() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), Clear);
}

void TIMER::count_direction_down() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), Set);
}

void TIMER::counter_alignment(Center_Align align) {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CAM), static_cast<uint32_t>(align));
}

void TIMER::set_counter_value(uint16_t counter) {
    write_register(*this, TIMER_Regs::CNT, static_cast<uint32_t>(counter));
}

uint32_t TIMER::read_counter()
{
    return read_register<uint32_t>(*this, TIMER_Regs::CNT);
}

void TIMER::set_prescaler_reload(uint16_t prescaler, PSC_Reload reload) {
    write_register(*this, TIMER_Regs::PSC, static_cast<uint32_t>(prescaler));

    if (reload == PSC_Reload::RELOAD_NOW) {
        write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(SWEVG_Bits::UPG), Set);
    }
}

uint16_t TIMER::read_prescaler() {
    return static_cast<uint16_t>(read_register<uint32_t>(*this, TIMER_Regs::PSC));
}

void TIMER::auto_reload_shadow_enable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ARSE), Set);
}

void TIMER::auto_reload_shadow_disable() {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ARSE), Clear);
}

void TIMER::set_auto_reload_value(uint16_t auto_reload) {
    write_register(*this, TIMER_Regs::CAR, static_cast<uint32_t>(auto_reload));
}

void TIMER::repetition_value_config(uint16_t repeat) {
    write_register(*this, TIMER_Regs::CREP, static_cast<uint32_t>(repeat));
}

void TIMER::set_pulse_mode(Pulse_Mode pulse) {
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPM), (pulse == Pulse_Mode::SINGLE_PULSE) ? Set : Clear);
}

void TIMER::dma_enable(DMAINTEN_Bits dma) {
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(dma), Set);
}

void TIMER::dma_disable(DMAINTEN_Bits dma) {
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(dma), Clear);
}

void TIMER::set_dma_request_source(DMA_Request request) {
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAS), (request == DMA_Request::UPDATE_EVENT) ? Set : Clear);
}

void TIMER::dma_transfer_config(DMA_Transfer_Address address, DMA_Burst_Length length) {
    write_bits(*this, TIMER_Regs::DMACFG, static_cast<uint32_t>(DMACFG_Bits::DMATA), static_cast<uint32_t>(address),
               static_cast<uint32_t>(DMACFG_Bits::DMATC), static_cast<uint32_t>(length));
}

void TIMER::break_init() {
    write_bits(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), static_cast<uint32_t>(break_config_.break_state),
               static_cast<uint32_t>(CCHP_Bits::BRKP), static_cast<uint32_t>(break_config_.break_polarity),
               static_cast<uint32_t>(CCHP_Bits::OAEN), static_cast<uint32_t>(break_config_.output_auto_state),
               static_cast<uint32_t>(CCHP_Bits::ROS), static_cast<uint32_t>(break_config_.ros_state),
               static_cast<uint32_t>(CCHP_Bits::IOS), static_cast<uint32_t>(break_config_.ios_state),
               static_cast<uint32_t>(CCHP_Bits::PROT), static_cast<uint32_t>(break_config_.protection),
               static_cast<uint32_t>(CCHP_Bits::DTCFG), static_cast<uint32_t>(break_config_.dead_time));
}

void TIMER::break_enable() {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), Set);
}

void TIMER::break_disable() {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), Clear);
}

void TIMER::set_break_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), enable ? Set : Clear);
}

void TIMER::output_auto_enable() {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), Set);
}

void TIMER::output_auto_disable() {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), Clear);
}

void TIMER::set_output_auto_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), enable ? Set : Clear);
}

void TIMER::set_primary_output_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::POEN), enable ? Set : Clear);
}

void TIMER::set_channel_shadow_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CCSE), enable ? Set : Clear);
}

void TIMER::channel_shadow_update_configure(Shadow_Update update) {
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CCUC), (update == Shadow_Update::SHADOW_CCUTRI) ? Set : Clear);
}

void TIMER::output_compare_init(Timer_Channel channel) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), Clear);
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), static_cast<uint32_t>(compare_config_.state),
                   static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(compare_config_.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NEN), static_cast<uint32_t>(compare_config_.companion_state),
                       static_cast<uint32_t>(CHCTL2_Bits::CH0NP), static_cast<uint32_t>(compare_config_.companion_polarity));
            write_bits(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO0), static_cast<uint32_t>(compare_config_.idle_state),
                       static_cast<uint32_t>(CTL1_Bits::ISO0), static_cast<uint32_t>(compare_config_.companion_idle_state));
        }
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), Clear);
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), static_cast<uint32_t>(compare_config_.state),
                   static_cast<uint32_t>(CHCTL2_Bits::CH1EN), static_cast<uint32_t>(compare_config_.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NEN), static_cast<uint32_t>(compare_config_.companion_state),
                       static_cast<uint32_t>(CHCTL2_Bits::CH1NP), static_cast<uint32_t>(compare_config_.companion_polarity));
            write_bits(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO1), static_cast<uint32_t>(compare_config_.idle_state),
                       static_cast<uint32_t>(CTL1_Bits::ISO1N), static_cast<uint32_t>(compare_config_.companion_idle_state));
        }
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2MS), Clear);
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), static_cast<uint32_t>(compare_config_.state),
                   static_cast<uint32_t>(CHCTL2_Bits::CH2P), static_cast<uint32_t>(compare_config_.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE ) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NEN), static_cast<uint32_t>(compare_config_.companion_state),
                       static_cast<uint32_t>(CHCTL2_Bits::CH2NP), static_cast<uint32_t>(compare_config_.companion_polarity));
            write_bits(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO2), static_cast<uint32_t>(compare_config_.idle_state),
                       static_cast<uint32_t>(CTL1_Bits::ISO2N), static_cast<uint32_t>(compare_config_.companion_idle_state));
        }
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3MS), Clear);
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), static_cast<uint32_t>(compare_config_.state),
                   static_cast<uint32_t>(CHCTL2_Bits::CH3P), static_cast<uint32_t>(compare_config_.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO3), static_cast<uint32_t>(compare_config_.idle_state));;
        }
        break;
    default:
        break;
    }
}

void TIMER::set_output_mode(Timer_Channel channel, Output_Compare_Mode mode) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCTL), static_cast<uint32_t>(mode));
        break;
    default:
        break;
    }
}

void TIMER::set_output_pulse(Timer_Channel channel, uint32_t pulse) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_register(*this, TIMER_Regs::CH0CV, pulse);
        break;
    case Timer_Channel::CH1:
        write_register(*this, TIMER_Regs::CH1CV, pulse);
        break;
    case Timer_Channel::CH2:
        write_register(*this, TIMER_Regs::CH2CV, pulse);
        break;
    case Timer_Channel::CH3:
        write_register(*this, TIMER_Regs::CH3CV, pulse);
        break;
    default:
        break;
    }
}

void TIMER::set_output_shadow(Timer_Channel channel, Output_Compare_Shadow shadow) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMSEN), static_cast<uint32_t>(shadow));
        break;
    default:
        break;
    }
}

void TIMER::set_output_fast(Timer_Channel channel, Output_Compare_Fast fast) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMFEN), static_cast<uint32_t>(fast));
        break;
    default:
        break;
    }
}

void TIMER::set_output_clear(Timer_Channel channel, Output_Compare_Clear oc_clear) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCEN), static_cast<uint32_t>(oc_clear));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCEN), static_cast<uint32_t>(oc_clear));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCEN), static_cast<uint32_t>(oc_clear));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCEN), static_cast<uint32_t>(oc_clear));
        break;
    default:
        break;
    }
}

void TIMER::set_output_polarity(Timer_Channel channel, uint16_t polarity) {
    switch(channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(polarity));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(polarity));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2P), static_cast<uint32_t>(polarity));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3P), static_cast<uint32_t>(polarity));
        break;
    default:
        break;
    }
}

void TIMER::set_complement_output_polarity(Timer_Channel channel, uint16_t polarity) {
    switch(channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), static_cast<uint32_t>(polarity));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), static_cast<uint32_t>(polarity));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NP), static_cast<uint32_t>(polarity));
        break;
    default:
        break;
    }
}

void TIMER::set_channel_output_enable(Timer_Channel channel, bool enable) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), enable ? Set : Clear);
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), enable ? Set : Clear);
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), enable ? Set : Clear);
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), enable ? Set : Clear);
        break;
    default:
        break;
    }
}

void TIMER::set_compliment_output_enable(Timer_Channel channel, bool enable) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NEN), enable ? Set : Clear);
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NEN), enable ? Set : Clear);
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NEN), enable ? Set : Clear);
        break;
    default:
        break;
    }
}

void TIMER::input_capture_init(Timer_Channel channel) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(capture_config_.polarity),
                   static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Set);
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter),
                   static_cast<uint32_t>(CHCTL0_Bits::CH0CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH1:
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(capture_config_.polarity),
                   static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Set);
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter),
                   static_cast<uint32_t>(CHCTL0_Bits::CH1CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH2:
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH2NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH2P), static_cast<uint32_t>(capture_config_.polarity),
                   static_cast<uint32_t>(CHCTL2_Bits::CH2EN), Set);
        write_bits(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL1_Bits::CH2CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter),
                   static_cast<uint32_t>(CHCTL1_Bits::CH2CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH3:
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH3P), static_cast<uint32_t>(capture_config_.polarity),
                   static_cast<uint32_t>(CHCTL2_Bits::CH3EN), Set);

        write_bits(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL1_Bits::CH3CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter),
                   static_cast<uint32_t>(CHCTL1_Bits::CH3CAPPSC), config_.prescaler);
        break;
    default:
        break;
    }
}

void TIMER::set_input_capture_prescaler(Timer_Channel channel, Input_Capture_Prescaler prescaler) {
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPPSC), static_cast<uint32_t>(prescaler));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPPSC), static_cast<uint32_t>(prescaler));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2CAPPSC), static_cast<uint32_t>(prescaler));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3CAPPSC), static_cast<uint32_t>(prescaler));
        break;
    default:
        break;
    }
}

void TIMER::input_pwm_capture_enable(Timer_Channel channel) {
    // Set polarity
    Polarity_Select input_polarity = (capture_config_.polarity == Polarity_Select::HIGH_RISING) ? Polarity_Select::LOW_FALLING : Polarity_Select::HIGH_RISING;

    // Set source select
    Input_Capture_Select source = (capture_config_.source_select == Input_Capture_Select::IO_INPUT_CI0FE0) ? Input_Capture_Select::IO_INPUT_CI1FE0 : Input_Capture_Select::IO_INPUT_CI0FE0;

    if (channel == Timer_Channel::CH0) {
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(capture_config_.polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Set);
        set_input_capture_prescaler(Timer_Channel::CH0, capture_config_.prescaler);
        // CH1
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(input_polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(source),
                   static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Set);
        set_input_capture_prescaler(Timer_Channel::CH1, capture_config_.prescaler);
    } else {
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(capture_config_.polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(capture_config_.source_select),
                   static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Set);
        set_input_capture_prescaler(Timer_Channel::CH1, capture_config_.prescaler);
        // CH0
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(input_polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(source),
                   static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(capture_config_.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Set);
        set_input_capture_prescaler(Timer_Channel::CH0, capture_config_.prescaler);
    }
}

void TIMER::set_hall_mode_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TI0S), enable ? Set : Clear);
}

void TIMER::set_input_trigger(Trigger_Select trigger) {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::TRGS), static_cast<uint32_t>(trigger));
}

void TIMER::external_trigger_configure(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter) {
    write_bits(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::ETP), static_cast<uint32_t>(polarity),
               static_cast<uint32_t>(SMCFG_Bits::ETPSC), static_cast<uint32_t>(prescaler),
               static_cast<uint32_t>(SMCFG_Bits::ETFC), filter);
}

uint32_t TIMER::get_capture_compare_value(Timer_Channel channel) {
    switch (channel) {
    case Timer_Channel::CH0:
        return read_register<uint32_t>(*this, TIMER_Regs::CH0CV);
    case Timer_Channel::CH1:
        return read_register<uint32_t>(*this, TIMER_Regs::CH1CV);
    case Timer_Channel::CH2:
        return read_register<uint32_t>(*this, TIMER_Regs::CH2CV);
    case Timer_Channel::CH3:
        return read_register<uint32_t>(*this, TIMER_Regs::CH3CV);
    default:
        return 0;
    }
}

void TIMER::set_master_output_trigger(Master_Control mode) {
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MMC), static_cast<uint32_t>(mode));
}

void TIMER::set_slave(Slave_Control mode) {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(mode));
}

void TIMER::set_master_slave_enable(bool enable) {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::MSM), enable ? Set : Clear);
}

void TIMER::quadrature_decoder_configure(Decode_Mode mode, Polarity_Select polarity1, Polarity_Select polarity2) {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(mode));
    write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0),
               static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0));
    write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), Clear,
               static_cast<uint32_t>(CHCTL2_Bits::CH1NP), Clear,
               static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(polarity1),
               static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(polarity2));
}

void TIMER::set_internal_clock() {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), Clear);
}

// Set internal trigger as external clock
void TIMER::set_clock_from_internal_trigger(Trigger_Select trigger) {
    set_input_trigger(trigger);
    write_bits(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), Clear,
               static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0));
}

void TIMER::set_clock_from_external_trigger(Trigger_Select trigger, Polarity_Select polarity, uint32_t filter) {
    if (read_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::TRGS)) == static_cast<uint32_t>(Trigger_Select::CI1FE1)) {
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0),
                   static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), filter);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), Set);
    } else {
        write_bits(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0NP), Clear,
                   static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(polarity));
        write_bits(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0),
                   static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), filter);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), Set);
    }
    set_input_trigger(trigger);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0));
}

void TIMER::set_clock_mode0(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter) {
    external_trigger_configure(prescaler, polarity, filter);
    write_bits(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0),
               static_cast<uint32_t>(SMCFG_Bits::TRGS), static_cast<uint32_t>(Trigger_Select::ETIFP));
}

void TIMER::set_clock_mode1(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter) {
    external_trigger_configure(prescaler, polarity, filter);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC1), Set);
}

void TIMER::clock_mode1_disable() {
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC1), Clear);
}

void TIMER::set_write_chxval_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CFG, static_cast<uint32_t>(CFG_Bits::CHVSEL), enable ? Set : Clear);
}

void TIMER::set_output_value_enable(bool enable) {
    write_bit(*this, TIMER_Regs::CFG, static_cast<uint32_t>(CFG_Bits::OUTSEL), enable ? Set : Clear);
}

bool TIMER::get_flag(Status_Flags flag) {
    return (read_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag)) != Clear);
}

void TIMER::clear_flag(Status_Flags flag) {
    write_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag), Clear);
}

bool TIMER::get_interrupt_flag(Interrupt_Flags flag) {
    return ((read_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag)) != Clear) &&
            (read_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(flag)) != Clear));
}

void TIMER::clear_interrupt_flag(Interrupt_Flags flag) {
    write_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag), Clear);
}

void TIMER::set_interrupt_enable(Interrupt_Type type, bool enable) {
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(type), enable ? Set : Clear);
}

} // namespace timer
