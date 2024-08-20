// gd32f30x DMA peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "DMA.hpp"

namespace dma {

void DMA::init(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }
    // Set the Peripheral address
    write_register(DMA_Regs::CHXPADDR, channel, config_.peripheral_address);

    // Set the memory address
    write_register(DMA_Regs::CHXMADDR, channel, config_.memory_address);

    // Set count of remaining data
    write_register(DMA_Regs::CHXCNT, channel, config_.count & 0x0000FFFF);

    // Set transfer bit width and priority
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PWIDTH), static_cast<uint32_t>(config_.peripheral_bit_width));
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MWIDTH), static_cast<uint32_t>(config_.memory_bit_width));
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(config_.channel_priority));

    // Set peripheral incr mode
    if (config_.peripheral_increase == Increase_Mode::INCREASE_ENABLE) {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PNAGA), 1);
    } else {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PNAGA), 0);
    }

    // Set memory incr mode
    if (config_.memory_increase == Increase_Mode::INCREASE_ENABLE) {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MNAGA), 1);
    } else {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MNAGA), 0);
    }

    // Set the transfer direction
    if (config_.direction == Transfer_Direction::P2M) {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::DIR), 0);
    } else {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::DIR), 1);
    }
}

void DMA::reset(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    // Disable DMA channel
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), 0);
    // Set register to default reset value
    write_register(DMA_Regs::CHXCTL, channel, 0x00000000);
    write_register(DMA_Regs::CHXCNT, channel, 0x00000000);
    write_register(DMA_Regs::CHXPADDR, channel, 0x00000000);
    write_register(DMA_Regs::CHXMADDR, channel, 0x00000000);
    write_register(DMA_Regs::INTC, (0xF << (static_cast<uint32_t>(channel) * 4)));
}

void DMA::set_pclk_enable(bool enable)
{
    RCU_DEVICE.set_pclk_enable(DMA_pclk_info_.clock_reg, enable ? true : false);
}

void DMA::set_new_parameters(DMA_Channel channel, DMA_Config *params)
{
    if (params == nullptr) {
        return;
    }

    config_.peripheral_address = params->peripheral_address;
    config_.peripheral_bit_width = params->peripheral_bit_width;
    config_.memory_address = params->memory_address;
    config_.memory_bit_width = params->memory_bit_width;
    config_.count = params->count;
    config_.peripheral_increase = params->peripheral_increase;
    config_.memory_increase = params->memory_increase;
    config_.channel_priority = params->channel_priority;
    config_.direction = params->direction;

    // Initialize the new configuration
    init(channel);
}

// DEPRECATED - use set_circulation_mode_enable
void DMA::circular_mode_enable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CMEN), 1);
}

// DEPRECATED - use set_circulation_mode_enable
void DMA::circular_mode_disable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CMEN), 0);
}

// Enable of disable cirulation (circular) mode
void DMA::set_circulation_mode_enable(DMA_Channel channel, bool enable)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CMEN), enable ? 1 : 0);
}

// DEPRECATED - use set_memory_to_memory_enable
void DMA::memory_to_memory_enable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::M2M), 1);
}

// DEPRECATED - use set_memory_to_memory_enable
void DMA::memory_to_memory_disable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::M2M), 0);
}

// Enable of disable M2M mode
void DMA::set_memory_to_memory_enable(DMA_Channel channel, bool enable)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::M2M), enable ? 1 : 0);
}

// DEPRECATED - use set_channel_enable
void DMA::channel_enable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), 1);
}

// DEPRECATED - use set_channel_enable
void DMA::channel_disable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), 0);
}

// Enable of disable DMA channel
void DMA::set_channel_enable(DMA_Channel channel, bool enable)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::CHEN), enable ? 1 : 0);
}

// DEPRECATED - use set_data_address
void DMA::set_peripheral_address(DMA_Channel channel, uint32_t address)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_register(DMA_Regs::CHXPADDR, channel, address);
}

// DEPRECATED - use set_data_address
void DMA::set_memory_address(DMA_Channel channel, uint32_t address)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_register(DMA_Regs::CHXMADDR, channel, address);
}

// Set peripheral or memory data address
void DMA::set_data_address(DMA_Channel channel, Data_Type type, uint32_t address)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_register((type == Data_Type::PERIPHERAL_ADDRESS) ? DMA_Regs::CHXPADDR : DMA_Regs::CHXMADDR, channel, address);
}

void DMA::set_transfer_count(DMA_Channel channel, uint32_t count)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }
    write_register(DMA_Regs::CHXCNT, channel, count & 0x0000FFFF);
}

uint32_t DMA::get_transfer_count(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return 0;
        // TODO: Error handling
    }

    return read_register<uint32_t>(DMA_Regs::CHXCNT, channel);
}

void DMA::set_channel_priority(DMA_Channel channel, Channel_Priority priority)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(priority));
}

// DEPRICATED - use set_bit_width
void DMA::set_memory_bit_width(DMA_Channel channel, Bit_Width width)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    // Set register
    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MWIDTH), static_cast<uint32_t>(width));
}

// DEPRICATED - use set_bit_width
void DMA::set_peripheral_bit_width(DMA_Channel channel, Bit_Width width)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PWIDTH), static_cast<uint32_t>(width));
}

// Set peripheral or memory bit width
void DMA::set_bit_width(DMA_Channel channel, Data_Type type, Bit_Width width)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, (type == Data_Type::PERIPHERAL_ADDRESS) ?
                      static_cast<uint32_t>(CHXCTL_Bits::PWIDTH) :
                      static_cast<uint32_t>(CHXCTL_Bits::MWIDTH),
                      static_cast<uint32_t>(width));
}

// DEPRICATED - use set_increase_mode_enable
void DMA::memory_incr_enable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MNAGA), 1);
}

// DEPRICATED - use set_increase_mode_enable
void DMA::memory_incr_disable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::MNAGA), 0);
}

// DEPRICATED - use set_increase_mode_enable
void DMA::peripheral_incr_enable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PNAGA), 1);
}

// DEPRICATED - use set_increase_mode_enable
void DMA::peripheral_incr_disable(DMA_Channel channel)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::PNAGA), 0);
}

// Enable or disable peripheral or memory increase mode
void DMA::set_increase_mode_enable(DMA_Channel channel, Data_Type type, bool enable)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, (type == Data_Type::PERIPHERAL_ADDRESS) ?
                      static_cast<uint32_t>(CHXCTL_Bits::PNAGA) :
                      static_cast<uint32_t>(CHXCTL_Bits::MNAGA),
                      enable ? 1 : 0);
}

void DMA::set_transfer_direction(DMA_Channel channel, Transfer_Direction direction)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    if (direction == Transfer_Direction::P2M) {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::DIR), 0);
    } else {
        write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(CHXCTL_Bits::DIR), 1);
    }
}

bool DMA::get_flag(DMA_Channel channel, State_Flags flag)
{
    return (read_register<uint32_t>(DMA_Regs::INTF) & ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4))) != 0;
}

void DMA::clear_flag(DMA_Channel channel, State_Flags flag)
{
    write_register(DMA_Regs::INTC, ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4)));
}

bool DMA::get_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag)
{
    uint32_t intr_flag = 0;
    uint32_t intr_enable = 0;
    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_FTFIF:
        intr_flag = (read_register<uint32_t>(DMA_Regs::INTF) & ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4)));
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_FTFIE));
        break;
    case Interrupt_Flags::INTR_FLAG_HTFIF:
        intr_flag = (read_register<uint32_t>(DMA_Regs::INTF) & ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4)));
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_HTFIE));
        break;
    case Interrupt_Flags::INTR_FLAG_ERRIF:
        intr_flag = (read_register<uint32_t>(DMA_Regs::INTF) & ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4)));
        intr_enable = read_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(Interrupt_Type::INTR_ERRIE));
        break;
    default:
        // TODO: Error handling
        break;
    }

    return (intr_flag && intr_enable) ? true : false;
}

void DMA::clear_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag)
{
    write_register(DMA_Regs::INTC, read_register<uint32_t>(DMA_Regs::INTC) | ((1 << static_cast<uint32_t>(flag)) << (static_cast<uint32_t>(channel) * 4)));
}

// DEPRICATED - use set_interrupt_enable
void DMA::interrupt_enable(DMA_Channel channel, Interrupt_Type type)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(type), 1);
}

// DEPRICATED - use set_interrupt_enable
void DMA::interrupt_disable(DMA_Channel channel, Interrupt_Type type)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(type), 0);
}

// Enable or disable interrupt
void DMA::set_interrupt_enable(DMA_Channel channel, Interrupt_Type type, bool enable)
{
    if (channel_validity(channel) != true) {
        return;
        // TODO: Error handling
    }

    write_bit_channel(*this, DMA_Regs::CHXCTL, channel, static_cast<uint32_t>(type), enable ? 1 : 0);
}

inline bool DMA::channel_validity(DMA_Channel channel)
{
    const uint8_t MaxChannel = (dma_base_index_ == DMA_Base::DMA1_BASE) ? static_cast<uint8_t>(DMA_Channel::CHANNEL4) : static_cast<uint8_t>(DMA_Channel::CHANNEL6);
    return static_cast<uint8_t>(channel) <= MaxChannel;
}

} // namespace dma
