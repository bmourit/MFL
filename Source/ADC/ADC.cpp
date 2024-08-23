// gd32f30x ADC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "ADC.hpp"

// Initialize the static member
bool adc::ADC::is_clock_enabled = false;

namespace adc {

void ADC::enable()
{
    if (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON)) == 0) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON), 1);
    }
}

void ADC::disable()
{
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON), 0);
}

void ADC::calibration_enable()
{
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RSTCLB), 1);

    while (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RSTCLB))) {
    }

    // Start the calibration
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CLB), 1);

    while (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CLB))) {
    }
}

// Enable or disable DMA
void ADC::dma_enable(bool enable)
{
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMA), enable ? 1 : 0);
}

// Enable or disable VREFINT
void ADC::vrefint_temp_enable(bool enable)
{
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TSVREN), enable ? 1 : 0);
}

// Set the resolution to 12Bit, 10Bit, 8Bit, or 6Bit
void ADC::set_resolution(ADC_Resolution resolution)
{
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::DRES), static_cast<uint32_t>(resolution));
}

void ADC::discontinuous_mode_config(Channel_Group_Type channel_group, uint8_t length)
{
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISRC), 0);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISIC), 0);

    using enum Channel_Group_Type;
    switch(channel_group) {
    case REGULAR_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISNUM), static_cast<uint32_t>(length) - 1);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISRC), 1);
        break;
    case INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISIC), 1);
        break;
    case CHANNEL_DISCON_DISABLE:
    default:
        break;
    }
}

void ADC::set_mode(Sync_Mode mode)
{
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SYNCM), static_cast<uint32_t>(mode));
}

void ADC::set_special_function(Special_Function function, bool enable)
{
    using enum Special_Function;
    switch (function) {
    case SCAN_MODE:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SM), enable ? 1 : 0);
        break;
    case INSERTED_CH_MODE:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ICA), enable ? 1 : 0);
        break;
    case CONTINUOUS_MODE:
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CTN), enable ? 1 : 0);
    default:
        break;
    }
}

void ADC::set_data_alignment(Data_Alignment align)
{
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DAL), static_cast<uint32_t>(align));
}

void ADC::set_channel_length(Channel_Group_Type channel_group, uint32_t length)
{
    using enum Channel_Group_Type;
    switch (channel_group) {
    case REGULAR_CHANNEL:
        write_bit(*this, ADC_Regs::RSQ0, static_cast<uint32_t>(RSQX_Bits::RL), length - 1);
        break;
    case INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL), length - 1);
        break;
    default:
        break;
    }
}

void ADC::regular_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time)
{
    uint32_t reg;

    // ADC regular sequence
    if (rank < 6) {
        reg = read_register<uint32_t>(ADC_Regs::RSQ2);
        reg &= ~(0x1F << (rank * 5));
        reg |= (static_cast<uint32_t>(channel) << (rank * 5));
        write_register(ADC_Regs::RSQ2, reg);
    } else if (rank < 12) {
        reg = read_register<uint32_t>(ADC_Regs::RSQ1);
        reg &= ~(0x1F << ((rank - 6) * 5));
        reg |= (static_cast<uint32_t>(channel) << ((rank - 6) * 5));
        write_register(ADC_Regs::RSQ1, reg);
    } else if (rank < 16) {
        reg = read_register<uint32_t>(ADC_Regs::RSQ0);
        reg &= ~(0x1F << ((rank - 12) * 5));
        reg |= (static_cast<uint32_t>(channel) << ((rank - 12) * 5));
        write_register(ADC_Regs::RSQ0, reg);
    } else {
    }

    // ADC sampling time
    if (channel < ADC_Channel::CHANNEL_10) {
        reg = read_register<uint32_t>(ADC_Regs::SAMPT1);
        reg &= ~(0x7 << (static_cast<uint32_t>(channel) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << (static_cast<uint32_t>(channel) * 3));
        write_register(ADC_Regs::SAMPT1, reg);
    } else if (channel <= ADC_Channel::CHANNEL_17) {
        reg = read_register<uint32_t>(ADC_Regs::SAMPT0);
        reg &= ~(0x7 << ((static_cast<uint32_t>(channel) - 10) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << ((static_cast<uint32_t>(channel) - 10) * 3));
        write_register(ADC_Regs::SAMPT0, reg);
    } else {
    }
}

void ADC::inserted_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time)
{

    uint8_t inserted_length = read_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL));
    uint32_t reg = read_register<uint32_t>(ADC_Regs::ISQ);
    reg &= ~(0x1F << (((rank + 3) - inserted_length) * 5));
    reg |= (static_cast<uint32_t>(channel) << (((rank + 3) - inserted_length) * 5));
    write_register(ADC_Regs::ISQ, reg);

    // ADC sampling time
    if (channel < ADC_Channel::CHANNEL_10) {
        reg = read_register<uint32_t>(ADC_Regs::SAMPT1);
        reg &= ~(0x7 << (static_cast<uint32_t>(channel) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << (static_cast<uint32_t>(channel) * 3));
        write_register(ADC_Regs::SAMPT1, reg);
    } else if (channel <= ADC_Channel::CHANNEL_17) {
        reg = read_register<uint32_t>(ADC_Regs::SAMPT0);
        reg &= ~(0x7 << ((static_cast<uint32_t>(channel) - 10) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << ((static_cast<uint32_t>(channel) - 10) * 3));
        write_register(ADC_Regs::SAMPT0, reg);
    } else {
    }
}

void ADC::inserted_channel_offset_config(Inserted_Channel inserted_channel, uint16_t offset)
{
    uint8_t inserted_length = read_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL));
    uint32_t reg = 3 - (inserted_length - static_cast<uint32_t>(inserted_channel));

    if (reg <= 3) {
        reg *= 4;
        write_bit_extra(*this, ADC_Regs::IOFF0, static_cast<uint32_t>(IOFFX_Bits::IOFF), static_cast<uint32_t>(offset), reg);
    }
}

void ADC::set_external_trigger_enable(Channel_Group_Type channel_group, bool enable)
{
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETERC), enable ? 1 : 0);
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETEIC), enable ? 1 : 0);
    }
}

void ADC::set_external_group_source(Channel_Group_Type channel_group, External_Trigger_Source source)
{
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETSRC), static_cast<uint32_t>(source));
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETSIC), static_cast<uint32_t>(source));
    }
}

void ADC::set_software_trigger_group(Channel_Group_Type channel_group)
{
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::SWRCST), 1);
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::SWICST), 1);
    }
}

uint16_t ADC::get_regular_data()
{
    return read_register<uint32_t>(ADC_Regs::RDATA);
}

uint16_t ADC::get_inserted_data(Inserted_Channel inserted_channel)
{
    uint32_t data = 0;

    using enum Inserted_Channel;
    switch (inserted_channel) {
    case INSERTED_CHANNEL_0:
        data = read_register<uint32_t>(ADC_Regs::IDATA0);
        break;
    case INSERTED_CHANNEL_1:
        data = read_register<uint32_t>(ADC_Regs::IDATA1);
        break;
    case INSERTED_CHANNEL_2:
        data = read_register<uint32_t>(ADC_Regs::IDATA2);
        break;
    case INSERTED_CHANNEL_3:
        data = read_register<uint32_t>(ADC_Regs::IDATA3);
        break;
    default:
        break;
    }

    return (uint16_t)data;
}

uint32_t ADC::get_sync_mode_convert_value(void)
{
    return read_register<uint32_t>(ADC_Regs::RDATA);
}

bool ADC::get_status_flag(STAT_Bits flag)
{
    return (read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag))) ? true : false;
}

void ADC::clear_status_flag(STAT_Bits flag)
{
    write_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag), 0);
}

bool ADC::get_interrupt_flag(Interrupt_Flags flag)
{
    bool is_flag = false;
    uint32_t stat;

    using enum Interrupt_Flags;
    switch (flag) {
    case INTR_FLAG_WDE:
        stat = read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(STAT_Bits::WDE));
        if (read_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDEIE)) && stat) {
            is_flag = true;
        }
        break;
    case INTR_FLAG_EOC:
        stat = read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(STAT_Bits::EOC));
        if (read_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOCIE)) && stat) {
            is_flag = true;
        }
        break;
    case INTR_FLAG_EOIC:
        stat = read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(STAT_Bits::EOIC));
        if (read_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOICIE)) && stat) {
            is_flag = true;
        }
        break;
    default:
        break;
    }

    return is_flag;
}

void ADC::clear_interrupt_flag(Interrupt_Flags flag)
{
    write_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag), 0);
}

void ADC::interrupt_enable(Interrupt_Type type)
{
    if (type == Interrupt_Type::INTR_WDE) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDEIE), 1);
    }
    if (type == Interrupt_Type::INTR_EOC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOCIE), 1);
    }
    if (type == Interrupt_Type::INTR_EOIC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOICIE), 1);
    }
}

void ADC::interrupt_disable(Interrupt_Type type)
{
    if (type == Interrupt_Type::INTR_WDE) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDEIE), 0);
    }
    if (type == Interrupt_Type::INTR_EOC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOCIE), 0);
    }
    if (type == Interrupt_Type::INTR_EOIC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOICIE), 0);
    }
}

void ADC::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    if (type == Interrupt_Type::INTR_WDE) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDEIE), enable ? 1 : 0);
    }
    if (type == Interrupt_Type::INTR_EOC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOCIE), enable ? 1 : 0);
    }
    if (type == Interrupt_Type::INTR_EOIC) {
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::EOICIE), enable ? 1 : 0);
    }
}

void ADC::single_channel_watchdog_enable(ADC_Channel channel)
{
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDCHSEL), static_cast<uint32_t>(channel));
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), 1);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), 1);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), 1);
}

void ADC::group_channel_watchdog_enable(Channel_Group_Type channel_group)
{
    using enum Channel_Group_Type;
    switch (channel_group) {
    case REGULAR_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), 0);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), 0);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), 1);
        break;
    case INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), 0);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), 0);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), 1);
        break;
    case REGULAR_INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), 0);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), 1);
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), 1);
        break;
    default:
        break;
    }
}

void ADC::watchdog_disable()
{
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), 0);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), 0);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), 0);
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDCHSEL), 0);
}

void ADC::set_watchdog_threshold(uint16_t low, uint16_t high)
{
    write_bit(*this, ADC_Regs::WDLT, static_cast<uint32_t>(WDLT_Bits::WDLT), static_cast<uint32_t>(low));
    write_bit(*this, ADC_Regs::WDHT, static_cast<uint32_t>(WDHT_Bits::WDHT), static_cast<uint32_t>(high));
}

void ADC::set_oversampling_configuration(Oversampling_Convertion mode, Oversampling_Shift shift, Oversampling_Ratio ratio)
{
    if (mode == Oversampling_Convertion::OVERSAMPLING_CONVERT_ONE) {
        write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::TOVS), 1);
    } else {
        write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::TOVS), 0);
    }

    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSR), 0);
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSS), 0);

    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSS), static_cast<uint32_t>(shift));
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSR), static_cast<uint32_t>(ratio));
}

void ADC::set_oversampling_enable(bool enable)
{
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSEN), enable ? 1 : 0);
}

} // namespace adc
