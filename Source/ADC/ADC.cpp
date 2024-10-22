// gd32f30x ADC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "ADC.hpp"

// Initialize the static member
bool adc::ADC::is_clock_enabled = false;

namespace adc {

void ADC::enable() {
    if (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON)) == 0) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON), Set);
    }
}

void ADC::disable() {
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADCON), Clear);
}

void ADC::calibration_enable() {
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RSTCLB), Set);
    while (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RSTCLB))) {
    }

    // Start the calibration
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CLB), Set);
    while (read_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CLB))) {
    }
}

// Enable or disable DMA
void ADC::dma_enable(bool enable) {
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMA), enable ? Set : Clear);
}

// Enable or disable VREFINT
void ADC::vrefint_temp_enable(bool enable) {
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TSVREN), enable ? Set : Clear);
}

// Set the resolution to 12Bit, 10Bit, 8Bit, or 6Bit
void ADC::set_resolution(ADC_Resolution resolution) {
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::DRES), static_cast<uint32_t>(resolution));
}

void ADC::discontinuous_mode_config(Channel_Group_Type channel_group, uint8_t length) {
    // Clear discontinuous mode bits
    write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISRC), Clear,
               static_cast<uint32_t>(CTL0_Bits::DISIC), Clear);

    // Set the relevant bits based on the channel group
    set_discontinuous_mode_bits(channel_group, length);
}

void ADC::set_mode(Sync_Mode mode) {
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SYNCM), static_cast<uint32_t>(mode));
}

void ADC::set_special_function(Special_Function function, bool enable) {
    switch (function) {
    case Special_Function::SCAN_MODE:
        toggle_special_function(ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SM), enable);
        break;
    case Special_Function::INSERTED_CH_MODE:
        toggle_special_function(ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ICA), enable);
        break;
    case Special_Function::CONTINUOUS_MODE:
        toggle_special_function(ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CTN), enable);
        break;
    default:
        break;
    }
}

void ADC::set_data_alignment(Data_Alignment align) {
    write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DAL), static_cast<uint32_t>(align));
}

void ADC::set_channel_length(Channel_Group_Type channel_group, uint32_t length)
{
    switch (channel_group) {
    case Channel_Group_Type::REGULAR_CHANNEL:
        write_bit(*this, ADC_Regs::RSQ0, static_cast<uint32_t>(RSQX_Bits::RL), length - 1);
        break;
    case Channel_Group_Type::INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL), length - 1);
        break;
    default:
        break;
    }
}

void ADC::regular_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time) {
    if (rank < 6) {
        uint32_t reg = read_register<uint32_t>(*this, ADC_Regs::RSQ2);
        configure_channel(reg, rank, channel);
        write_register(*this, ADC_Regs::RSQ2, reg);
    } else if (rank < 12) {
        uint32_t reg = read_register<uint32_t>(*this, ADC_Regs::RSQ1);
        configure_channel(reg, rank - 6, channel);
        write_register(*this, ADC_Regs::RSQ1, reg);
    } else if (rank < 16) {
        uint32_t reg = read_register<uint32_t>(*this, ADC_Regs::RSQ0);
        configure_channel(reg, rank - 12, channel);
        write_register(*this, ADC_Regs::RSQ0, reg);
    }

    set_sampling_time(channel, sample_time);
}

void ADC::inserted_channel_config(uint8_t rank, ADC_Channel channel, ADC_Sample_Time sample_time) {
    uint8_t inserted_length = read_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL));
    uint32_t reg = read_register<uint32_t>(*this, ADC_Regs::ISQ);
    configure_channel(reg, (rank + 3) - inserted_length, channel);
    write_register(*this, ADC_Regs::ISQ, reg);

    set_sampling_time(channel, sample_time);
}

void ADC::inserted_channel_offset_config(Inserted_Channel inserted_channel, uint16_t offset) {
    uint8_t inserted_length = read_bit(*this, ADC_Regs::ISQ, static_cast<uint32_t>(ISQ_Bits::IL));
    uint32_t reg = 3 - (inserted_length - static_cast<uint32_t>(inserted_channel));

    if (reg <= 3) {
        reg *= 4;
        write_bit_extra(*this, ADC_Regs::IOFF0, static_cast<uint32_t>(IOFFX_Bits::IOFF), static_cast<uint32_t>(offset), reg);
    }
}

void ADC::set_external_trigger_enable(Channel_Group_Type channel_group, bool enable) {
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETERC), enable ? Set : Clear);
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETEIC), enable ? Set : Clear);
    }
}

void ADC::set_external_group_source(Channel_Group_Type channel_group, External_Trigger_Source source) {
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETSRC), static_cast<uint32_t>(source));
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ETSIC), static_cast<uint32_t>(source));
    }
}

void ADC::set_software_trigger_group(Channel_Group_Type channel_group) {
    if (channel_group == Channel_Group_Type::REGULAR_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::SWRCST), Set);
    }
    if (channel_group == Channel_Group_Type::INSERTED_CHANNEL) {
        write_bit(*this, ADC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::SWICST), Set);
    }
}

uint16_t ADC::get_regular_data() {
    return static_cast<uint16_t>(read_register<uint32_t>(*this, ADC_Regs::RDATA));
}

uint16_t ADC::get_inserted_data(Inserted_Channel inserted_channel) {
    static const ADC_Regs inserted_data_regs[] = {
        ADC_Regs::IDATA0, ADC_Regs::IDATA1, ADC_Regs::IDATA2, ADC_Regs::IDATA3
    };

    if (static_cast<size_t>(inserted_channel) >= sizeof(inserted_data_regs) / sizeof(inserted_data_regs[0])) {
        return 0;
    }

    return static_cast<uint16_t>(read_register<uint32_t>(*this, inserted_data_regs[static_cast<size_t>(inserted_channel)]));
}

uint32_t ADC::get_sync_mode_convert_value(void) {
    return read_register<uint32_t>(*this, ADC_Regs::RDATA);
}

void ADC::single_channel_watchdog_enable(ADC_Channel channel) {
    write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDCHSEL), static_cast<uint32_t>(channel),
               static_cast<uint32_t>(CTL0_Bits::RWDEN), Set,
               static_cast<uint32_t>(CTL0_Bits::IWDEN), Set,
               static_cast<uint32_t>(CTL0_Bits::WDSC), Set);
}

void ADC::group_channel_watchdog_enable(Channel_Group_Type channel_group) {
    switch (channel_group) {
    case Channel_Group_Type::REGULAR_CHANNEL:
        write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), Clear,
                   static_cast<uint32_t>(CTL0_Bits::WDSC), Clear,
                   static_cast<uint32_t>(CTL0_Bits::RWDEN), Set);
        break;
    case Channel_Group_Type::INSERTED_CHANNEL:
        write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWDEN), Clear,
                   static_cast<uint32_t>(CTL0_Bits::WDSC), Clear,
                   static_cast<uint32_t>(CTL0_Bits::IWDEN), Set);
        break;
    case Channel_Group_Type::REGULAR_INSERTED_CHANNEL:
        write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WDSC), Clear,
                   static_cast<uint32_t>(CTL0_Bits::RWDEN), Set,
                   static_cast<uint32_t>(CTL0_Bits::IWDEN), Set);
        break;
    default:
        break;
    }
}

void ADC::watchdog_disable() {
    write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IWDEN), Clear,
               static_cast<uint32_t>(CTL0_Bits::WDSC), Clear,
               static_cast<uint32_t>(CTL0_Bits::RWDEN), Clear,
               static_cast<uint32_t>(CTL0_Bits::WDCHSEL), Clear);
}

void ADC::set_watchdog_threshold(uint16_t low, uint16_t high) {
    write_bit(*this, ADC_Regs::WDLT, static_cast<uint32_t>(WDLT_Bits::WDLT), static_cast<uint32_t>(low));
    write_bit(*this, ADC_Regs::WDHT, static_cast<uint32_t>(WDHT_Bits::WDHT), static_cast<uint32_t>(high));
}

void ADC::set_oversampling_configuration(Oversampling_Conversion mode, Oversampling_Shift shift, Oversampling_Ratio ratio) {
    write_bits(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::TOVS), (mode == Oversampling_Conversion::OVERSAMPLING_CONVERT_ONE) ? Set : Clear,
               static_cast<uint32_t>(OVSAMPCTL_Bits::OVSS), static_cast<uint32_t>(shift),
               static_cast<uint32_t>(OVSAMPCTL_Bits::OVSR), static_cast<uint32_t>(ratio));
}

void ADC::set_oversampling_enable(bool enable) {
    write_bit(*this, ADC_Regs::OVSAMPCTL, static_cast<uint32_t>(OVSAMPCTL_Bits::OVSEN), enable ? Set : Clear);
}

bool ADC::get_flag(Status_Flags flag) {
    return (read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag)) != Clear);
}

void ADC::clear_flag(Status_Flags flag) {
    write_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag), Clear);
}

bool ADC::get_interrupt_flag(Interrupt_Flags flag) {
    auto check_flag = [this](CTL0_Bits control_bit, Interrupt_Flags flag) -> bool {
        uint32_t flag_status = read_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag));
        return flag_status && read_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(control_bit));
    };

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_WDE:
        return check_flag(CTL0_Bits::WDEIE, flag);
    case Interrupt_Flags::INTR_FLAG_EOC:
        return check_flag(CTL0_Bits::EOCIE, flag);
    case Interrupt_Flags::INTR_FLAG_EOIC:
        return check_flag(CTL0_Bits::EOICIE, flag);
    default:
        return false;
    }
}

void ADC::clear_interrupt_flag(Interrupt_Flags flag) {
    write_bit(*this, ADC_Regs::STAT, static_cast<uint32_t>(flag), Clear);
}

void ADC::set_interrupt_enable(Interrupt_Type type, bool enable) {
    write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(type), enable ? Set : Clear);
}

void ADC::set_sampling_time(ADC_Channel channel, ADC_Sample_Time sample_time) {
    uint32_t reg;

    if (channel < ADC_Channel::CHANNEL_10) {
        reg = read_register<uint32_t>(*this, ADC_Regs::SAMPT1);
        reg &= ~(0x7 << (static_cast<uint32_t>(channel) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << (static_cast<uint32_t>(channel) * 3));
        write_register(*this, ADC_Regs::SAMPT1, reg);
    } else if (channel <= ADC_Channel::CHANNEL_17) {
        reg = read_register<uint32_t>(*this, ADC_Regs::SAMPT0);
        reg &= ~(0x7 << ((static_cast<uint32_t>(channel) - 10) * 3));
        reg |= (static_cast<uint32_t>(sample_time) << ((static_cast<uint32_t>(channel) - 10) * 3));
        write_register(*this, ADC_Regs::SAMPT0, reg);
    }
}

void ADC::configure_channel(uint32_t reg, uint8_t rank, ADC_Channel channel) {
    reg &= ~(0x1F << (rank * 5));
    reg |= (static_cast<uint32_t>(channel) << (rank * 5));
}

void ADC::toggle_special_function(ADC_Regs reg, uint32_t bit, bool enable) {
    write_bit(*this, reg, bit, enable ? Set : Clear);
}

void ADC::set_discontinuous_mode_bits(Channel_Group_Type channel_group, uint8_t length) {
    switch (channel_group) {
    case Channel_Group_Type::REGULAR_CHANNEL:
        write_bits(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISNUM), static_cast<uint32_t>(length) - 1,
                   static_cast<uint32_t>(CTL0_Bits::DISRC), Set);
        break;
    case Channel_Group_Type::INSERTED_CHANNEL:
        write_bit(*this, ADC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::DISIC), Set);
        break;
    default:
        break;
    }
}

} // namespace adc
