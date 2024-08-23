// gd32f30x TIMER peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "TIMER.hpp"

// Initialize the static member
bool timer::TIMER::is_clock_enabled = false;

namespace timer {

void TIMER::init()
{
    write_register(TIMER_Regs::PSC, config_.prescaler);

    if ((base_index_ != TIMER_Base::TIMER5_BASE) && (base_index_ != TIMER_Base::TIMER6_BASE)) {
        write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), static_cast<uint32_t>(config_.counting_direction));
        write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CAM), static_cast<uint32_t>(config_.align));
    }
    write_register(TIMER_Regs::CAR, config_.period);

    if ((base_index_ != TIMER_Base::TIMER5_BASE) && (base_index_ != TIMER_Base::TIMER6_BASE)) {
        write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKDIV), static_cast<uint32_t>(config_.divider));
    }
    if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
        write_register(TIMER_Regs::CREP, config_.repetition_count);
    }

    write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(SWEVG_Bits::UPG), 1);
}

void TIMER::enable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CEN), 1);
}

void TIMER::disable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CEN), 0);
}

void TIMER::update_event_enable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPDIS), 0);
}

void TIMER::update_event_disable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPDIS), 1);
}

void TIMER::generate_software_event(Event_Source event)
{
    write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(event), 1);
}

void TIMER::set_update_source(Update_Source source)
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UPS), (source == Update_Source::REGULAR_SOURCE) ? 1 : 0);
}

void TIMER::count_direction_up()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), 0);
}

void TIMER::count_direction_down()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DIR), 1);
}

void TIMER::counter_alignment(Center_Align align)
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CAM), static_cast<uint32_t>(align));
}

void TIMER::set_counter_value(uint16_t counter)
{
    write_register(TIMER_Regs::CNT, static_cast<uint32_t>(counter));
}

uint32_t TIMER::read_counter()
{
    return read_register<uint32_t>(TIMER_Regs::CNT);
}

void TIMER::set_prescaler_reload(uint16_t prescaler, PSC_Reload reload)
{
    write_register(TIMER_Regs::PSC, static_cast<uint32_t>(prescaler));

    if (reload == PSC_Reload::RELOAD_NOW) {
        write_bit(*this, TIMER_Regs::SWEVG, static_cast<uint32_t>(SWEVG_Bits::UPG), 1);
    }
}

uint16_t TIMER::read_prescaler()
{
    return static_cast<uint16_t>(read_register<uint32_t>(TIMER_Regs::PSC));
}

void TIMER::auto_reload_shadow_enable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ARSE), 1);
}

void TIMER::auto_reload_shadow_disable()
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ARSE), 0);
}

void TIMER::set_auto_reload_value(uint16_t auto_reload)
{
    write_register(TIMER_Regs::CAR, static_cast<uint32_t>(auto_reload));
}

void TIMER::repetition_value_config(uint16_t repeat)
{
    write_register(TIMER_Regs::CREP, static_cast<uint32_t>(repeat));
}

void TIMER::set_pulse_mode(Pulse_Mode pulse)
{
    write_bit(*this, TIMER_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPM), (pulse == Pulse_Mode::SINGLE_PULSE) ? 1 : 0);
}

void TIMER::dma_enable(DMAINTEN_Bits dma)
{
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(dma), 1);
}

void TIMER::dma_disable(DMAINTEN_Bits dma)
{
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(dma), 0);
}

void TIMER::set_dma_request_source(DMA_Request request)
{
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAS), (request == DMA_Request::UPDATE_EVENT) ? 1 : 0);
}

void TIMER::dma_transfer_config(DMA_Transfer_Address address, DMA_Burst_Length length)
{
    write_bit(*this, TIMER_Regs::DMACFG, static_cast<uint32_t>(DMACFG_Bits::DMATA), static_cast<uint32_t>(address));
    write_bit(*this, TIMER_Regs::DMACFG, static_cast<uint32_t>(DMACFG_Bits::DMATC), static_cast<uint32_t>(length));
}

void TIMER::break_configure(TIMER_Break *break_config)
{
    config_.break_params = {
        .break_state = break_config->break_state,
        .break_polarity = break_config->break_polarity,
        .output_auto_state = break_config->output_auto_state,
        .ros_state = break_config->ros_state,
        .ios_state = break_config->ios_state,
        .protection = break_config->protection,
        .dead_time = break_config->dead_time,
    };
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), static_cast<uint32_t>(config_.break_params.break_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKP), static_cast<uint32_t>(config_.break_params.break_polarity));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), static_cast<uint32_t>(config_.break_params.output_auto_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::ROS), static_cast<uint32_t>(config_.break_params.ros_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::IOS), static_cast<uint32_t>(config_.break_params.ios_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::PROT), static_cast<uint32_t>(config_.break_params.protection));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::DTCFG), static_cast<uint32_t>(config_.break_params.dead_time));
}

void TIMER::break_init()
{
    // Load stored parameters
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), static_cast<uint32_t>(config_.break_params.break_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKP), static_cast<uint32_t>(config_.break_params.break_polarity));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), static_cast<uint32_t>(config_.break_params.output_auto_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::ROS), static_cast<uint32_t>(config_.break_params.ros_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::IOS), static_cast<uint32_t>(config_.break_params.ios_state));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::PROT), static_cast<uint32_t>(config_.break_params.protection));
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::DTCFG), static_cast<uint32_t>(config_.break_params.dead_time));
}

void TIMER::break_enable()
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), 1);
}

void TIMER::break_disable()
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), 0);
}

void TIMER::set_break_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::BRKEN), enable ? 1 : 0);
}

void TIMER::output_auto_enable()
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), 1);
}

void TIMER::output_auto_disable()
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), 0);
}

void TIMER::set_output_auto_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::OAEN), enable ? 1 : 0);
}

void TIMER::set_primary_output_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CCHP, static_cast<uint32_t>(CCHP_Bits::POEN), enable ? 1 : 0);
}

void TIMER::set_channel_shadow_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CCSE), enable ? 1 : 0);
}

void TIMER::channel_shadow_update_configure(Shadow_Update update)
{
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CCUC), (update == Shadow_Update::SHADOW_CCUTRI) ? 1 : 0);
}

void TIMER::output_compare_init(Timer_Channel channel)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), static_cast<uint32_t>(config_.output_params.state));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(config_.output_params.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NEN), 0);
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NEN), static_cast<uint32_t>(config_.output_params.companion_state));
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), static_cast<uint32_t>(config_.output_params.companion_polarity));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO0), 0);
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO0), static_cast<uint32_t>(config_.output_params.idle_state));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO0N), 0);
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO0), static_cast<uint32_t>(config_.output_params.companion_idle_state));
        }
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), 0);
        // Set CH1EN
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), static_cast<uint32_t>(config_.output_params.state));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), 0);
        // Set CH1P
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), static_cast<uint32_t>(config_.output_params.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NEN), 0);
            // Set CH1NEN
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NEN), static_cast<uint32_t>(config_.output_params.companion_state));
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
            // Set CH1NP
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), static_cast<uint32_t>(config_.output_params.companion_polarity));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO1), 0);
            // Set ISO1
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO1), static_cast<uint32_t>(config_.output_params.idle_state));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO1N), 0);
            // Set ISO1N
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO1N), static_cast<uint32_t>(config_.output_params.companion_idle_state));
        }
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2MS), 0);
        // Set CH2EN
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), static_cast<uint32_t>(config_.output_params.state));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2P), 0);
        // Set CH2P
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2P), static_cast<uint32_t>(config_.output_params.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE ) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NEN), 0);
            // Set CH2NEN
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NEN), static_cast<uint32_t>(config_.output_params.companion_state));
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NP), 0);
            // Set CH2NP
            write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NP), static_cast<uint32_t>(config_.output_params.companion_polarity));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO2), 0);
            // Set ISO2
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO2), static_cast<uint32_t>(config_.output_params.idle_state));
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO2N), 0);
            // Set ISO2N
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO2N), static_cast<uint32_t>(config_.output_params.companion_idle_state));
        }
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3MS), 0);
        // Set CH3EN
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), static_cast<uint32_t>(config_.output_params.state));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3P), 0);
        // Set CH3P
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3P), static_cast<uint32_t>(config_.output_params.polarity));
        if ((base_index_ == TIMER_Base::TIMER0_BASE) || (base_index_ == TIMER_Base::TIMER7_BASE)) {
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO3), 0);
            // Set ISO3
            write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ISO3), static_cast<uint32_t>(config_.output_params.idle_state));;
        }
        break;
    default:
        break;
    }
}

void TIMER::output_compare_configure(Timer_Channel channel, TIMER_Output_Compare *output_compare)
{
    config_.output_params = {
        .state = output_compare->state,                                 // CH_CC_DISABLE;
        .companion_state = output_compare->companion_state,             // CH_CC_DISABLE;
        .polarity = output_compare->polarity,                           // CH_CC_HIGH;
        .companion_polarity = output_compare->companion_polarity,       // CH_CC_HIGH;
        .idle_state = output_compare->idle_state,                       // CH_IDLE_LOW;
        .companion_idle_state = output_compare->companion_idle_state,   // CH_IDLE_LOW;
    };
    output_compare_init(channel);
}

void TIMER::set_output_mode(Timer_Channel channel, Output_Compare_Mode mode)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCTL), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCTL), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCTL), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCTL), static_cast<uint32_t>(mode));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCTL), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCTL), static_cast<uint32_t>(mode));
        break;
    default:
        break;
    }
}

void TIMER::set_output_pulse(Timer_Channel channel, uint32_t pulse)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_register(TIMER_Regs::CH0CV, pulse);
        break;
    case Timer_Channel::CH1:
        write_register(TIMER_Regs::CH1CV, pulse);
        break;
    case Timer_Channel::CH2:
        write_register(TIMER_Regs::CH2CV, pulse);
        break;
    case Timer_Channel::CH3:
        write_register(TIMER_Regs::CH3CV, pulse);
        break;
    default:
        break;
    }
}

void TIMER::set_output_shadow(Timer_Channel channel, Output_Compare_Shadow shadow)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMSEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMSEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMSEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMSEN), static_cast<uint32_t>(shadow));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMSEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMSEN), static_cast<uint32_t>(shadow));
        break;
    default:
        break;
    }
}

void TIMER::set_output_fast(Timer_Channel channel, Output_Compare_Fast fast)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMFEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMFEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMFEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMFEN), static_cast<uint32_t>(fast));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMFEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMFEN), static_cast<uint32_t>(fast));
        break;
    default:
        break;
    }
}

void TIMER::set_output_clear(Timer_Channel channel, Output_Compare_Clear clear)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0COMCEN), static_cast<uint32_t>(clear));
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1COMCEN), static_cast<uint32_t>(clear));
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2COMCEN), static_cast<uint32_t>(clear));
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCEN), 0);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3COMCEN), static_cast<uint32_t>(clear));
        break;
    default:
        break;
    }
}

void TIMER::set_output_polarity(Timer_Channel channel, uint16_t polarity)
{
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

void TIMER::set_complement_output_polarity(Timer_Channel channel, uint16_t polarity)
{
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

void TIMER::set_channel_output_enable(Timer_Channel channel, bool enable)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), enable ? 1 : 0);
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), enable ? 1 : 0);
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), enable ? 1 : 0);
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), enable ? 1 : 0);
        break;
    default:
        break;
    }
}

void TIMER::set_compliment_output_enable(Timer_Channel channel, bool enable)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NEN), enable ? 1 : 0);
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NEN), enable ? 1 : 0);
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NEN), enable ? 1 : 0);
        break;
    default:
        break;
    }
}

void TIMER::input_capture_init(Timer_Channel channel)
{
    switch (channel) {
    case Timer_Channel::CH0:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(config_.input_params.polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(config_.input_params.source_select));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(config_.input_params.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 1);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH1:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(config_.input_params.polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(config_.input_params.source_select));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(config_.input_params.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 1);
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH2:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2P), static_cast<uint32_t>(config_.input_params.polarity));
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2MS), static_cast<uint32_t>(config_.input_params.source_select));
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2CAPFLT), static_cast<uint32_t>(config_.input_params.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH2EN), 1);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH2CAPPSC), config_.prescaler);
        break;
    case Timer_Channel::CH3:
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3P), static_cast<uint32_t>(config_.input_params.polarity));
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3MS), static_cast<uint32_t>(config_.input_params.source_select));
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3CAPFLT), static_cast<uint32_t>(config_.input_params.digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH3EN), 1);
        write_bit(*this, TIMER_Regs::CHCTL1, static_cast<uint32_t>(CHCTL1_Bits::CH3CAPPSC), config_.prescaler);
        break;
    default:
        break;
    }
}

void TIMER::input_capture_configure(Timer_Channel channel, TIMER_Input_Capture *input_capture)
{
    config_.input_params = {
        .polarity  = input_capture->polarity,
        .source_select = input_capture->source_select,
        .prescaler = input_capture->prescaler,
        .digital_filter = input_capture->digital_filter,
    };
    input_capture_init(channel);
}

void TIMER::set_input_capture_prescaler(Timer_Channel channel, Input_Capture_Prescaler prescaler)
{
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

void TIMER::input_pwm_capture_enable(Timer_Channel channel, TIMER_Input_Capture *pwm)
{
    Polarity_Select input_polarity;
    Input_Capture_Select source;

    // Set polarity
    if (pwm->polarity == Polarity_Select::HIGH_RISING) {
        input_polarity = Polarity_Select::LOW_FALLING;
    } else {
        input_polarity = Polarity_Select::HIGH_RISING;
    }

    // Set source select
    if (pwm->source_select == Input_Capture_Select::IO_INPUT_CI0FE0) {
        source = Input_Capture_Select::IO_INPUT_CI1FE0;
    } else {
        source = Input_Capture_Select::IO_INPUT_CI0FE0;
    }

    if (channel == Timer_Channel::CH0) {
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(pwm->polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(pwm->source_select));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(pwm->digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 1);
        set_input_capture_prescaler(Timer_Channel::CH0, pwm->prescaler);
        // CH1
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(input_polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(source));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(pwm->digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 1);
        set_input_capture_prescaler(Timer_Channel::CH1, pwm->prescaler);
    } else {
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(pwm->polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(pwm->source_select));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), static_cast<uint32_t>(pwm->digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 1);
        set_input_capture_prescaler(Timer_Channel::CH1, pwm->prescaler);
        // CH0
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(input_polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(source));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), static_cast<uint32_t>(pwm->digital_filter));
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 1);
        set_input_capture_prescaler(Timer_Channel::CH0, pwm->prescaler);
    }
}

void TIMER::set_hall_mode_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TI0S), enable ? 1 : 0);
}

void TIMER::set_input_trigger(Trigger_Select trigger)
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::TRGS), static_cast<uint32_t>(trigger));
}

void TIMER::external_trigger_configure(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter)
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::ETP), static_cast<uint32_t>(polarity));
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::ETPSC), static_cast<uint32_t>(prescaler));
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::ETFC), filter);
}

uint32_t TIMER::get_capture_compare_value(Timer_Channel channel)
{
    switch (channel) {
    case Timer_Channel::CH0:
        return read_register<uint32_t>(TIMER_Regs::CH0CV);
    case Timer_Channel::CH1:
        return read_register<uint32_t>(TIMER_Regs::CH1CV);
    case Timer_Channel::CH2:
        return read_register<uint32_t>(TIMER_Regs::CH2CV);
    case Timer_Channel::CH3:
        return read_register<uint32_t>(TIMER_Regs::CH3CV);
    default:
        return 0;
    }
}

void TIMER::set_master_output_trigger(Master_Control mode)
{
    write_bit(*this, TIMER_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MMC), static_cast<uint32_t>(mode));
}

void TIMER::set_slave(Slave_Control mode)
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(mode));
}

void TIMER::set_master_slave_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::MSM), enable ? 1 : 0);
}

void TIMER::quadrature_decoder_configure(Decode_Mode mode, Polarity_Select polarity1, Polarity_Select polarity2)
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(mode));
    write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0));
    write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0));
    write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
    write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
    write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(polarity1));
    write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(polarity2));
}

void TIMER::set_internal_clock()
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), 0);
}

// Set internal trigger as external clock
void TIMER::set_internal_trigger_as_clock(Trigger_Select trigger)
{
    set_input_trigger(trigger);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), 0);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0));
}

void TIMER::set_external_trigger_as_clock(Trigger_Select trigger, Polarity_Select polarity, uint32_t filter)
{
    if (read_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::TRGS)) == static_cast<uint32_t>(Trigger_Select::CI1FE1)) {

        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1P), static_cast<uint32_t>(polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH1CAPFLT), filter);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH1EN), 1);
    } else {
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0NP), 0);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0P), static_cast<uint32_t>(polarity));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0MS), static_cast<uint32_t>(Input_Capture_Select::IO_INPUT_CI0FE0));
        write_bit(*this, TIMER_Regs::CHCTL0, static_cast<uint32_t>(CHCTL0_Bits::CH0CAPFLT), filter);
        write_bit(*this, TIMER_Regs::CHCTL2, static_cast<uint32_t>(CHCTL2_Bits::CH0EN), 1);
    }
    set_input_trigger(trigger);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0));
}

void TIMER::set_clock_mode0(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter)
{
    \
    external_trigger_configure(prescaler, polarity, filter);


    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC), static_cast<uint32_t>(Slave_Control::EXTERNAL0));
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::TRGS), static_cast<uint32_t>(Trigger_Select::ETIFP));
}

void TIMER::set_clock_mode1(External_Trigger_Prescaler prescaler, Polarity_Select polarity, uint32_t filter)
{
    external_trigger_configure(prescaler, polarity, filter);
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC1), 1);
}

void TIMER::clock_mode1_disable()
{
    write_bit(*this, TIMER_Regs::SMCFG, static_cast<uint32_t>(SMCFG_Bits::SMC1), 0);
}

void TIMER::set_write_chxval_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CFG, static_cast<uint32_t>(CFG_Bits::CHVSEL), enable ? 1 : 0);
}

void TIMER::set_output_value_enable(bool enable)
{
    write_bit(*this, TIMER_Regs::CFG, static_cast<uint32_t>(CFG_Bits::OUTSEL), enable ? 1 : 0);
}

bool TIMER::get_flag(Status_Flags flag)
{
    return (read_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag)) != 0);
}

void TIMER::clear_flag(Status_Flags flag)
{
    write_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag), 0);
}

bool TIMER::get_interrupt_flag(Interrupt_Flags flag)
{
    return ((read_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag)) != 0) &&
            (read_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(flag)) != 0));
}

void TIMER::clear_interrupt_flag(Interrupt_Flags flag)
{
    write_bit(*this, TIMER_Regs::INTF, static_cast<uint32_t>(flag), 0);
}

void TIMER::interrupt_enable(Interrupt_Type type)
{
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(type), 1);
}

void TIMER::interrupt_disable(Interrupt_Type type)
{
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(type), 0);
}

void TIMER::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    write_bit(*this, TIMER_Regs::DMAINTEN, static_cast<uint32_t>(type), enable ? 1 : 0);
}

} // namespace timer
