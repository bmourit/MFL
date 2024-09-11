// gd32f30x DMA peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "DMA.hpp"

// Initialize the static member
bool dma::DMA::is_clock_enabled = false;

namespace dma {

constexpr uint32_t Lower16BitMask = 0x0000FFFF;

void DMA::init(DMA_Channel channel) {
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }
    // Addresses
    write_register(*this, DMA_Regs::CHXPADDR, channel, config_.peripheral_address);
    write_register(*this, DMA_Regs::CHXMADDR, channel, config_.memory_address);
    // Count
    write_register(*this, DMA_Regs::CHXCNT, channel, config_.count & Lower16BitMask);
    // Set parameters
    write_bits_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PWIDTH), static_cast<uint32_t>(config_.peripheral_bit_width),
                       static_cast<uint32_t>(CHXCTL_Bits::MWIDTH), static_cast<uint32_t>(config_.memory_bit_width),
                       static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(config_.channel_priority),
                       static_cast<uint32_t>(CHXCTL_Bits::PNAGA), (config_.peripheral_increase == Increase_Mode::INCREASE_ENABLE) ? Set : Clear,
                       static_cast<uint32_t>(CHXCTL_Bits::MNAGA), (config_.memory_increase == Increase_Mode::INCREASE_ENABLE) ? Set : Clear,
                       static_cast<uint32_t>(CHXCTL_Bits::DIR), (config_.direction == Transfer_Direction::M2P) ? Set : Clear);
}

void DMA::reset(DMA_Channel channel) {
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    // Disable DMA channel
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), Clear);
    // Set register to default reset value
    write_register(*this, DMA_Regs::CHXCTL, channel, Clear);
    write_register(*this, DMA_Regs::CHXCNT, channel, Clear);
    write_register(*this, DMA_Regs::CHXPADDR, channel, Clear);
    write_register(*this, DMA_Regs::CHXMADDR, channel, Clear);
    write_register(*this, DMA_Regs::INTC, (0xF << (static_cast<uint32_t>(channel) * 4)));
    // Load defaults
    config_ = default_config;
}

// Enable of disable cirulation (circular) mode
void DMA::set_circulation_mode_enable(DMA_Channel channel, bool enable) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CMEN), enable ? Set : Clear);
}

// Enable of disable M2M mode
void DMA::set_memory_to_memory_enable(DMA_Channel channel, bool enable) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::M2M), enable ? Set : Clear);
}

// Enable of disable DMA channel
void DMA::set_channel_enable(DMA_Channel channel, bool enable) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), enable ? Set : Clear);
}

// Set peripheral or memory data address
void DMA::set_data_address(DMA_Channel channel, Data_Type type, uint32_t address) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_register(*this, (type == Data_Type::PERIPHERAL_ADDRESS) ? DMA_Regs::CHXPADDR : DMA_Regs::CHXMADDR, channel, address);
}

void DMA::set_transfer_count(DMA_Channel channel, uint32_t count) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_register(*this, DMA_Regs::CHXCNT, channel, count & Lower16BitMask);
}

uint32_t DMA::get_transfer_count(DMA_Channel channel) {
    if (channel_validity(channel) != true) {
        return 0;
    }
    return read_register<uint32_t>(*this, DMA_Regs::CHXCNT, channel);
}

void DMA::set_channel_priority(DMA_Channel channel, Channel_Priority priority) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(priority));
}

// Set peripheral or memory bit width
void DMA::set_bit_width(DMA_Channel channel, Data_Type type, Bit_Width width) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, (type == Data_Type::PERIPHERAL_ADDRESS) ?
                      static_cast<uint32_t>(CHXCTL_Bits::PWIDTH) :
                      static_cast<uint32_t>(CHXCTL_Bits::MWIDTH),
                      static_cast<uint32_t>(width));
}

// Enable or disable peripheral or memory increase mode
void DMA::set_increase_mode_enable(DMA_Channel channel, Data_Type type, bool enable) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, (type == Data_Type::PERIPHERAL_ADDRESS) ?
                      static_cast<uint32_t>(CHXCTL_Bits::PNAGA) :
                      static_cast<uint32_t>(CHXCTL_Bits::MNAGA),
                      enable ? Set : Clear);
}

void DMA::set_transfer_direction(DMA_Channel channel, Transfer_Direction direction) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::DIR), (direction == Transfer_Direction::M2P) ? Set : Clear);
}

bool DMA::get_flag(DMA_Channel channel, Status_Flags flag) {
    return (read_bit_with_channel_offset(*this, DMA_Regs::INTF, static_cast<uint32_t>(flag), channel) != Clear);
}

void DMA::clear_flag(DMA_Channel channel, Status_Flags flag) {
    write_bit_with_channel_offset(*this, DMA_Regs::INTC, static_cast<uint32_t>(flag), channel, Set);
}

bool DMA::get_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag) {
    uint32_t intr_flag = 0;
    uint32_t intr_enable = 0;
    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_FTFIF:
        intr_flag = read_bit_with_channel_offset(*this, DMA_Regs::INTF, static_cast<uint32_t>(flag), channel);
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_FTFIE));
        break;
    case Interrupt_Flags::INTR_FLAG_HTFIF:
        intr_flag = read_bit_with_channel_offset(*this, DMA_Regs::INTF, static_cast<uint32_t>(flag), channel);
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_HTFIE));
        break;
    case Interrupt_Flags::INTR_FLAG_ERRIF:
        intr_flag = read_bit_with_channel_offset(*this, DMA_Regs::INTF, static_cast<uint32_t>(flag), channel);
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_ERRIE));
        break;
    default:
        break;
    }

    return ((intr_flag != Clear) && (intr_enable != Clear));
}

void DMA::clear_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag) {
    write_bit_with_channel_offset(*this, DMA_Regs::INTC, static_cast<uint32_t>(flag), channel, Set);
}

// Enable or disable interrupt
void DMA::set_interrupt_enable(DMA_Channel channel, Interrupt_Type type, bool enable) {
    if (channel_validity(channel) != true) {
        return;
    }
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(type), enable ? Set : Clear);
}

inline bool DMA::channel_validity(DMA_Channel channel) {
    const uint8_t MaxChannel = (dma_base_index_ == DMA_Base::DMA1_BASE) ? static_cast<uint8_t>(DMA_Channel::CHANNEL4) : static_cast<uint8_t>(DMA_Channel::CHANNEL6);
    return static_cast<uint8_t>(channel) <= MaxChannel;
}

} // namespace dma
