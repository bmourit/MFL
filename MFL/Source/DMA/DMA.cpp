//
// MFL gd32f30x DMA peripheral register access in C++
//
// Copyright (C) 2024 B. Mouritsen <bnmguy@gmail.com>. All rights reserved.
//
// This file is part of the Microcontroller Firmware Library (MFL).
//
// MFL is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// MFL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with MFL.
// If not, see <https://www.gnu.org/licenses/>.
//

#include "DMA.hpp"
#include "RCU.hpp"

namespace dma {

template <DMA_Base Base, DMA_Channel Channel>
DMA& get_instance_for_base() {
    static DMA instance(Base, Channel);
    return instance;
}

Result<DMA, DMA_Error_Type> DMA::get_instance(DMA_Base Base, DMA_Channel Channel) {
    if (Base == DMA_Base::DMA0_BASE) {
        switch (Channel) {
        case DMA_Channel::CHANNEL0:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL0>()
                   );
        case DMA_Channel::CHANNEL1:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL1>()
                   );
        case DMA_Channel::CHANNEL2:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL2>()
                   );
        case DMA_Channel::CHANNEL3:
        return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL3>()
                   );
        case DMA_Channel::CHANNEL4:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL4>()
                   );
        case DMA_Channel::CHANNEL5:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL5>()
                   );
        case DMA_Channel::CHANNEL6:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE, DMA_Channel::CHANNEL6>()
                   );
        case DMA_Channel::INVALID:
        default:
            return RETURN_ERROR(DMA, DMA_Error_Type::INVALID_DMA);
        }
    } else if (Base == DMA_Base::DMA1_BASE) {
        switch (Channel) {
        case DMA_Channel::CHANNEL0:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE, DMA_Channel::CHANNEL0>()
                   );
        case DMA_Channel::CHANNEL1:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE, DMA_Channel::CHANNEL1>()
                   );
        case DMA_Channel::CHANNEL2:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE, DMA_Channel::CHANNEL2>()
                   );
        case DMA_Channel::CHANNEL3:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE, DMA_Channel::CHANNEL3>()
                   );
        case DMA_Channel::CHANNEL4:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE, DMA_Channel::CHANNEL4>()
                   );
        case DMA_Channel::INVALID:
        default:
            return RETURN_ERROR(DMA, DMA_Error_Type::INVALID_DMA);
        }
    }
    return RETURN_ERROR(DMA, DMA_Error_Type::INVALID_DMA);
}

std::array<bool, static_cast<size_t>(DMA_Base::INVALID)> DMA::clock_enabled_ = {false};

DMA::DMA(DMA_Base Base, DMA_Channel Channel) :
    base_(Base),
    channel_(Channel),
    DMA_pclk_info_(DMA_pclk_index[static_cast<size_t>(Base)]),
    base_address_(DMA_baseAddress[static_cast<size_t>(Base)]),
    config_(default_config)
{
    if (!clock_enabled_[static_cast<size_t>(Base)]) {
        RCU_I.set_pclk_enable(DMA_pclk_info_.clock_reg, true);
        clock_enabled_[static_cast<size_t>(Base)] = true;
    }
    // Initialize default values
    init();
}

/**
 * @brief Initialize the DMA channel
 *
 * This function will set the registers for the DMA channel with the given configuration.
 * It will set the priority, memory and peripheral bit widths, memory and peripheral increase modes, and the direction of the transfer.
 * It will also set the memory and peripheral addresses, and the transfer count.
 * Finally, it will store the configuration in the object for later use.
 *
 * @param config The configuration for the DMA channel
 */
void DMA::init(DMA_Config config) {
    // Set parameters
    DMA_Regs ctl_offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit_ranges(*this, ctl_offset,
               static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(config.channel_priority),
               static_cast<uint32_t>(CHXCTL_Bits::MWIDTH), static_cast<uint32_t>(config.memory_bit_width),
               static_cast<uint32_t>(CHXCTL_Bits::PWIDTH), static_cast<uint32_t>(config.peripheral_bit_width));
    write_bits_ordered(*this, ctl_offset,
               static_cast<uint32_t>(CHXCTL_Bits::PNAGA), (config.peripheral_increase == Increase_Mode::INCREASE_ENABLE),
               static_cast<uint32_t>(CHXCTL_Bits::MNAGA), (config.memory_increase == Increase_Mode::INCREASE_ENABLE),
               static_cast<uint32_t>(CHXCTL_Bits::DIR), (config.direction == Transfer_Direction::M2P));

    // Addresses
    DMA_Regs maddr_offset = get_channel_offset_from_reg(Channel_Regs::CHXMADDR);
    DMA_Regs paddr_offset = get_channel_offset_from_reg(Channel_Regs::CHXPADDR);
    write_register(*this, maddr_offset, config.memory_address);
    write_register(*this, paddr_offset, config.peripheral_address);

    // Count
    DMA_Regs cnt_offset = get_channel_offset_from_reg(Channel_Regs::CHXCNT);
    write_register(*this, cnt_offset, config.count & Lower16BitMask);

    // Circulation mode
    DMA_Regs circular_offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, circular_offset, static_cast<uint32_t>(CHXCTL_Bits::CMEN), config.circular_mode);

    // Memory to memory mode
    DMA_Regs m2m_offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, m2m_offset, static_cast<uint32_t>(CHXCTL_Bits::M2M), config.memory_to_memory);

    // Store the new cofig
    config_ = config;
}

/**
 * @brief Reset the DMA channel to its default values
 *
 * This function will reset the DMA channel to its default values.
 * It will disable the DMA channel, set the count register to 0, and set the memory and peripheral addresses to 0.
 * It will also clear the interrupt flags for the channel.
 * Finally, it will load the default configuration into the object.
 */
void DMA::reset() {
    DMA_Regs ctl_offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    DMA_Regs cnt_offset = get_channel_offset_from_reg(Channel_Regs::CHXCNT);
    DMA_Regs paddr_offset = get_channel_offset_from_reg(Channel_Regs::CHXPADDR);
    DMA_Regs maddr_offset = get_channel_offset_from_reg(Channel_Regs::CHXMADDR);

    // Disable DMA channel
    write_bit(*this, ctl_offset, static_cast<uint32_t>(CHXCTL_Bits::CHEN), false);
    // Set register to default reset value
    write_register(*this, cnt_offset, Clear);
    write_register(*this, paddr_offset, Clear);
    write_register(*this, paddr_offset, Clear);
    write_register(*this, maddr_offset, Clear);
    write_register(*this, DMA_Regs::INTC, (0xFU << (static_cast<uint32_t>(channel_) * 4)));

    // Load default config
    config_ = default_config;
}

/**
 * @brief Enable or disable circulation mode for the DMA channel.
 *
 * This function sets the circulation mode for the DMA channel by writing to
 * the channel control register. When enabled, the DMA channel will continue
 * to circulate through the buffer after reaching the end, allowing for
 * continuous data transfer.
 *
 * @param enable Set to true to enable circulation mode, false to disable it.
 */
void DMA::set_circulation_mode_enable(bool enable) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::CMEN), enable);
}

/**
 * @brief Enable or disable memory-to-memory mode for the DMA channel.
 *
 * This function sets the memory-to-memory mode for the DMA channel by writing to
 * the channel control register. When enabled, the DMA channel will only transfer
 * data between memory locations, and will not transfer to or from peripherals.
 *
 * @param enable Set to true to enable memory-to-memory mode, false to disable it.
 */
void DMA::set_memory_to_memory_enable(bool enable) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::M2M), enable);
}

/**
 * @brief Enable or disable the DMA channel.
 *
 * This function sets the channel enable bit in the channel control register.
 * When enabled, the DMA channel will start a transfer based on the configured
 * settings. When disabled, the DMA channel will stop any active transfers.
 *
 * @param enable Set to true to enable the DMA channel, false to disable it.
 */
void DMA::set_channel_enable(bool enable) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::CHEN), enable);
}

/**
 * @brief Sets the peripheral or memory address for the DMA channel.
 *
 * This function sets the peripheral or memory address for the DMA channel by
 * writing to the channel peripheral address register (CHXPADDR) or channel
 * memory address register (CHXMADDR) based on the type of address specified.
 *
 * @param type Specifies whether the address is for a peripheral or memory.
 * @param address The peripheral or memory address to set.
 */
void DMA::set_data_address(Data_Type type, uint32_t address) {
    DMA_Regs offset = get_channel_offset_from_reg((type == Data_Type::PERIPHERAL_ADDRESS) ?
           Channel_Regs::CHXPADDR : Channel_Regs::CHXMADDR);
    write_register(*this, offset, address);
}

/**
 * @brief Sets the transfer count for the DMA channel.
 *
 * This function sets the transfer count for the DMA channel by writing to
 * the channel transfer count register (CHXCNT). The transfer count is
 * limited to 16 bits, so the upper 16 bits of the count parameter are
 * ignored. The DMA channel will perform the specified number of transfers
 * before stopping.
 *
 * @param count The number of transfers to perform.
 */
void DMA::set_transfer_count(uint32_t count) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCNT);
    write_register(*this, offset, count & Lower16BitMask);
}

/**
 * @brief Returns the current transfer count for the DMA channel.
 *
 * This function returns the current transfer count for the DMA channel by
 * reading from the channel transfer count register (CHXCNT). The return
 * value is limited to 16 bits, so the upper 16 bits are ignored.
 *
 * @return The current transfer count for the DMA channel.
 */
uint32_t DMA::get_transfer_count() {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCNT);
    return read_register<uint32_t>(*this, offset);
}

/**
 * @brief Sets the priority for the DMA channel.
 *
 * This function sets the priority for the DMA channel by writing to
 * the channel control register. The priority determines the order in
 * which the DMA channels are serviced. A lower priority number (0-3)
 * indicates a higher priority.
 *
 * @param priority The priority value for the DMA channel.
 */
void DMA::set_channel_priority(Channel_Priority priority) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit_range(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::PRIO), static_cast<uint32_t>(priority));
}

/**
 * @brief Sets the bit width for the DMA channel peripheral or memory data.
 *
 * This function sets the bit width for the DMA channel peripheral or memory
 * data by writing to the channel control register (CHXCTL). The bit width
 * determines how many bits are transfered in each data transfer. The bit width
 * is applied to either the peripheral address or the memory address, depending
 * on the type argument.
 *
 * @param type The type of data to set the bit width for. Must be a value from
 *             the Data_Type enumeration.
 * @param width The bit width to set. Must be a value from the Bit_Width
 *              enumeration.
 */
void DMA::set_bit_width(Data_Type type, Bit_Width width) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit_range(*this, offset, (type == Data_Type::PERIPHERAL_ADDRESS) ?
          static_cast<uint32_t>(CHXCTL_Bits::PWIDTH) : static_cast<uint32_t>(CHXCTL_Bits::MWIDTH),
          static_cast<uint32_t>(width));
}

/**
 * @brief Enables or disables peripheral or memory increase mode for the DMA channel.
 *
 * This function sets the peripheral or memory increase mode bit in the channel
 * control register (CHXCTL). If the increase mode is enabled, the peripheral or
 * memory address is incremented by the size of the data transfer after each
 * data transfer.
 *
 * @param type The type of data to enable or disable increase mode for. Must be
 *             a value from the Data_Type enumeration.
 * @param enable True to enable increase mode, false to disable it.
 */
void DMA::set_increase_mode_enable(Data_Type type, bool enable) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, (type == Data_Type::PERIPHERAL_ADDRESS) ?
          static_cast<uint32_t>(CHXCTL_Bits::PNAGA) :
          static_cast<uint32_t>(CHXCTL_Bits::MNAGA),
          enable);
}

/**
 * @brief Sets the transfer direction for the DMA channel.
 *
 * This function sets the transfer direction bit in the channel control
 * register (CHXCTL). The transfer direction determines the direction of the
 * data transfer. If the direction is set to M2P, data is transferred from
 * memory to peripheral. If the direction is set to P2M, data is transferred
 * from peripheral to memory.
 *
 * @param direction The direction of the data transfer. Must be a value from
 *                  the Transfer_Direction enumeration.
 */
void DMA::set_transfer_direction(Transfer_Direction direction) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::DIR),
           (direction == Transfer_Direction::M2P) ? true : false);
}

/**
 * @brief Clears the DMA channel by setting all the control register bits to
 *        zero. This function is useful for resetting the DMA channel to its
 *        default state after a transfer is complete.
 */
void DMA::clear_channel() {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit_range(*this, offset, static_cast<uint32_t>(CHXCTL_Bits::ALL), Clear);
}

/**
 * @brief Returns the status of the specified flag.
 *
 * This function retrieves the status of the specified flag from the DMA
 * status register (INTF). If the flag is not applicable to the channel, this
 * function returns false.
 *
 * @param flag The status flag to retrieve. Must be a value from the
 *             Status_Flags enumeration.
 * @return true if the flag is set, false otherwise.
 */
bool DMA::get_flag(Status_Flags flag) {
    INTF_Bits bits = get_channel_bits_from_flag(flag);
    if (bits != INTF_Bits::INVALID) {
        return read_bit(*this, DMA_Regs::INTF, static_cast<uint32_t>(bits));
    }
    return false;
}

/**
 * Clears the specified flag in the DMA channel.
 *
 * This function clears the specified flag by writing a 1 to the
 * corresponding bit in the INTC register. The flag is cleared
 * regardless of whether or not the interrupt is enabled.
 *
 * @param flag The flag to clear, specified as a Status_Flags enumeration
 *             value.
 */
void DMA::clear_flag(Status_Flags flag) {
    INTF_Bits bits = get_channel_bits_from_flag(flag);
    if (bits == INTF_Bits::INVALID) {
        return;
    }
    write_bit(*this, DMA_Regs::INTC, static_cast<uint32_t>(bits), true);
}

void DMA::clear_flags(uint32_t flags) {
    uint32_t combined_bits = 0U;
    for (uint32_t flag = 1U; flag != 0U; flag <<= 1U) {
        if (flags & flag) {
            INTF_Bits bits = get_channel_bits_from_flag(static_cast<Status_Flags>(flag));
            if (bits != INTF_Bits::INVALID) {
                combined_bits |= static_cast<uint32_t>(bits);
            }
        }
    }
    atomic_write_bits(*this, DMA_Regs::INTC, combined_bits);
}

/**
 * @brief Retrieves the status of a specified interrupt flag for the DMA channel.
 *
 * This function retrieves the status of the specified interrupt flag for the
 * DMA channel. If the flag is not applicable to the channel, this function
 * returns false.
 *
 * @param flag The interrupt flag to retrieve. Must be a value from the
 *             Interrupt_Flags enumeration.
 * @return true if the flag is set, false otherwise.
 */
bool DMA::get_interrupt_flag(Interrupt_Flags flag) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    INTF_Bits bits = INTF_Bits::INVALID;

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_FTFIF: bits = get_channel_bits_from_flag(Status_Flags::FLAG_FTFIF); break;
    case Interrupt_Flags::INTR_FLAG_HTFIF: bits = get_channel_bits_from_flag(Status_Flags::FLAG_HTFIF); break;
    case Interrupt_Flags::INTR_FLAG_ERRIF: bits = get_channel_bits_from_flag(Status_Flags::FLAG_ERRIF); break;
    case Interrupt_Flags::INTR_FLAG_GIF: default: break;
    }

    if (offset == DMA_Regs::INVALID || bits == INTF_Bits::INVALID) {
        return false;
    }

    bool intr_flag = read_bit(*this, DMA_Regs::INTF, static_cast<uint32_t>(bits));
    bool intr_enable = read_bit(*this, offset, static_cast<uint32_t>(bits));

    return (intr_flag && intr_enable);
}

/**
 * Clears a specified interrupt flag for the DMA channel.
 *
 * This function clears a specified interrupt flag for the DMA channel. If the
 * flag is not applicable to the channel, this function does nothing.
 *
 * @param flag The interrupt flag to clear. Must be a value from the
 *             Interrupt_Flags enumeration.
 */
void DMA::clear_interrupt_flag(Interrupt_Flags flag) {
    Status_Flags status_flag = Status_Flags::INVALID;

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_FTFIF: status_flag = Status_Flags::FLAG_FTFIF; break;
    case Interrupt_Flags::INTR_FLAG_HTFIF: status_flag = Status_Flags::FLAG_HTFIF; break;
    case Interrupt_Flags::INTR_FLAG_ERRIF: status_flag = Status_Flags::FLAG_ERRIF; break;
    case Interrupt_Flags::INTR_FLAG_GIF: default: break;
    }

    INTF_Bits bits = get_channel_bits_from_flag(status_flag);
    if (bits == INTF_Bits::INVALID) {
        return;
    }

    return write_bit(*this, DMA_Regs::INTC, static_cast<uint32_t>(bits), true);
}

/**
 * Enables or disables a specific DMA channel interrupt.
 *
 * This function configures the interrupt settings for the DMA channel by
 * enabling or disabling the specified interrupt type. It modifies the control
 * register to reflect the desired interrupt configuration.
 *
 * @param type The type of interrupt to configure. Must be a value from the
 *             Interrupt_Type enumeration.
 * @param enable Set to true to enable the interrupt or false to disable it.
 */
void DMA::set_interrupt_enable(Interrupt_Type type, bool enable) {
    DMA_Regs offset = get_channel_offset_from_reg(Channel_Regs::CHXCTL);
    write_bit(*this, offset, static_cast<uint32_t>(type), enable);
}

/**
 * Converts a DMA channel status flag to its corresponding INTF flag.
 *
 * @param flag A Status_Flags value representing a DMA channel status flag.
 * @return The corresponding INTF flag for the given status flag.
 */
inline INTF_Bits DMA::get_channel_bits_from_flag(Status_Flags flag) {
    static const INTF_Bits gif_map[] = {
        INTF_Bits::GIF0, INTF_Bits::GIF1, INTF_Bits::GIF2,
        INTF_Bits::GIF3, INTF_Bits::GIF4, INTF_Bits::GIF5,
        INTF_Bits::GIF6
    };
    static const INTF_Bits ftfif_map[] = {
        INTF_Bits::FTFIF0, INTF_Bits::FTFIF1, INTF_Bits::FTFIF2,
        INTF_Bits::FTFIF3, INTF_Bits::FTFIF4, INTF_Bits::FTFIF5,
        INTF_Bits::FTFIF5
    };
    static const INTF_Bits htfif_map[] = {
        INTF_Bits::HTFIF0, INTF_Bits::HTFIF1, INTF_Bits::HTFIF2,
        INTF_Bits::HTFIF3, INTF_Bits::HTFIF4, INTF_Bits::HTFIF5,
        INTF_Bits::HTFIF6
    };
    static const INTF_Bits errif_map[] = {
        INTF_Bits::ERRIF0, INTF_Bits::ERRIF1, INTF_Bits::ERRIF2,
        INTF_Bits::ERRIF3, INTF_Bits::ERRIF4, INTF_Bits::ERRIF5,
        INTF_Bits::ERRIF6
    };

    switch (flag) {
    case Status_Flags::FLAG_GIF: return gif_map[static_cast<size_t>(channel_)];
    case Status_Flags::FLAG_FTFIF: return ftfif_map[static_cast<size_t>(channel_)];
    case Status_Flags::FLAG_HTFIF: return htfif_map[static_cast<size_t>(channel_)];
    case Status_Flags::FLAG_ERRIF: return errif_map[static_cast<size_t>(channel_)];
    case Status_Flags::INVALID: default: return INTF_Bits::INVALID;
    }
}

/**
 * Converts a Channel_Regs enum value to the corresponding DMA register offset.
 *
 * @param reg The Channel_Regs enum value to convert.
 * @return The corresponding DMA register offset.
 */
inline DMA_Regs DMA::get_channel_offset_from_reg(Channel_Regs reg) {
    static const DMA_Regs chxctl_map[] = {
        DMA_Regs::CH0CTL, DMA_Regs::CH1CTL, DMA_Regs::CH2CTL,
        DMA_Regs::CH3CTL, DMA_Regs::CH4CTL, DMA_Regs::CH6CTL,
        DMA_Regs::CH6CTL
    };
    static const DMA_Regs chxcnt_map[] = {
        DMA_Regs::CH0CNT, DMA_Regs::CH1CNT, DMA_Regs::CH2CNT,
        DMA_Regs::CH3CNT, DMA_Regs::CH4CNT, DMA_Regs::CH5CNT,
        DMA_Regs::CH6CNT
    };
    static const DMA_Regs chxpaddr_map[] = {
        DMA_Regs::CH0PADDR, DMA_Regs::CH1PADDR, DMA_Regs::CH2PADDR,
        DMA_Regs::CH3PADDR, DMA_Regs::CH4PADDR, DMA_Regs::CH5PADDR,
        DMA_Regs::CH6PADDR
    };
    static const DMA_Regs chxmaddr_map[] = {
        DMA_Regs::CH0MADDR, DMA_Regs::CH1MADDR, DMA_Regs::CH2MADDR,
        DMA_Regs::CH3MADDR, DMA_Regs::CH4MADDR, DMA_Regs::CH5MADDR,
        DMA_Regs::CH6MADDR
    };
    switch (reg) {
    case Channel_Regs::CHXCTL: return chxctl_map[static_cast<size_t>(channel_)];
    case Channel_Regs::CHXCNT: return chxcnt_map[static_cast<size_t>(channel_)];
    case Channel_Regs::CHXPADDR: return chxpaddr_map[static_cast<size_t>(channel_)];
    case Channel_Regs::CHXMADDR: return chxmaddr_map[static_cast<size_t>(channel_)];
    case Channel_Regs::INVALID: default: return DMA_Regs::INVALID;
    }
}

} // namespace dma
