// gd32f30x DMA peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "RegRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"

namespace dma {

class DMA {
public:
    static Result<DMA, DMA_Error_Type> get_instance(DMA_Base Base) {
        switch (Base) {
        case DMA_Base::DMA0_BASE:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA0_BASE, get_instance_for_base<DMA_Base::DMA0_BASE>()
                   );
        case DMA_Base::DMA1_BASE:
            return get_enum_instance<DMA_Base, DMA, DMA_Error_Type>(
                       Base, DMA_Base::DMA1_BASE, get_instance_for_base<DMA_Base::DMA1_BASE>()
                   );
        default:
            return RETURN_ERROR(DMA, DMA_Error_Type::INVALID_DMA);
        }
    }

    // Init
    void init(DMA_Channel channel);
    // Reset
    void reset(DMA_Channel channel);
    void configure(DMA_Channel channel, DMA_Config new_config) {
        config_ = new_config;
        init(channel);
    }
    // Circulation mode
    void set_circulation_mode_enable(DMA_Channel channel, bool enable);
    // M2M
    void set_memory_to_memory_enable(DMA_Channel, bool enable);
    // Channel
    void set_channel_enable(DMA_Channel channel, bool enable);
    // RX/TX addresses
    void set_data_address(DMA_Channel channel, Data_Type type, uint32_t address);
    // Transfer count
    void set_transfer_count(DMA_Channel channel, uint32_t count);
    uint32_t get_transfer_count(DMA_Channel channel);
    // Priority
    void set_channel_priority(DMA_Channel channel, Channel_Priority priority);
    // Bit width
    void set_bit_width(DMA_Channel channel, Data_Type type, Bit_Width width);
    // Increase mode
    void set_increase_mode_enable(DMA_Channel channel, Data_Type type, bool enable);
    // Direction
    void set_transfer_direction(DMA_Channel channel, Transfer_Direction direction);
    // Flags
    bool get_flag(DMA_Channel channel, Status_Flags flag);
    void clear_flag(DMA_Channel channel, Status_Flags flag);
    // Interrupt flags
    bool get_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag);
    void clear_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag);
    // Interrupts
    void set_interrupt_enable(DMA_Channel channel, Interrupt_Type type, bool enable);

    // Register address
    volatile uint32_t *reg_address(DMA_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }
    volatile uint32_t *reg_address(DMA_Regs reg, DMA_Channel channel) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(channel) * 0x14U + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}

    DMA_Base dma_base_index_;

private:
    DMA(DMA_Base Base) : dma_base_index_(Base),
        DMA_pclk_info_(DMA_pclk_index[static_cast<int>(Base)]),
        base_address_(DMA_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(DMA_pclk_info_.clock_reg, true);
            is_clock_enabled = true;
        }
    }

    DMA_Clock_Config DMA_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

    // Default initialization config
    DMA_Config default_config = {
        .peripheral_address = 0,
        .peripheral_bit_width = Bit_Width::WIDTH_8BIT,
        .memory_address = 0,
        .memory_bit_width = Bit_Width::WIDTH_8BIT,
        .count = 0,
        .peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE,
        .memory_increase = dma::Increase_Mode::INCREASE_DISABLE,
        .channel_priority = dma::Channel_Priority::LOW_PRIORITY,
        .direction = dma::Transfer_Direction::P2M,
    };
    DMA_Config config_ = default_config;

    template <DMA_Base Base>
    static DMA& get_instance_for_base() {
        static DMA instance(Base);
        return instance;
    }

    inline bool channel_validity(DMA_Channel channel);
};

} // namespace dma
