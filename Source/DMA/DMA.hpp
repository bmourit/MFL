// gd32f30x DMA peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "dma_config.hpp"

#define MAX_DMAS    2

namespace dma {

class DMA {
public:
    static DMA& instance(DMA_Base Base, DMA_Config& config) {
        static DMA instances[MAX_DMAS] = {
            DMA(DMA_Base::DMA0_BASE, config),
            DMA(DMA_Base::DMA1_BASE, config),
        };
        return instances[static_cast<int>(Base)];
    }

    // Init
    void init(DMA_Channel channel);
    // Reset
    void reset(DMA_Channel channel);
    // Clock
    void set_pclk_enable(bool enable);
    // Circulation mode
    void circular_mode_enable(DMA_Channel channel);                                     // DEPRECATED - use set_circulation_mode_enable
    void circular_mode_disable(DMA_Channel channel);                                    // DEPRECATED - use set_circulation_mode_enable
    void set_circulation_mode_enable(DMA_Channel channel, bool enable);
    // M2M
    void memory_to_memory_enable(DMA_Channel channel);                                  // DEPRECATED - use set_memory_to_memory_enable
    void memory_to_memory_disable(DMA_Channel channel);                                 // DEPRECATED - use set_memory_to_memory_enable
    void set_memory_to_memory_enable(DMA_Channel, bool enable);
    // Channel
    void channel_enable(DMA_Channel channel);                                           // DEPRECATED - use set_channel_enable
    void channel_disable(DMA_Channel channel);                                          // DEPRECATED - use set_channel_enable
    void set_channel_enable(DMA_Channel channel, bool enable);
    // RX/TX addresses
    void set_peripheral_address(DMA_Channel channel, uint32_t address);                 // DEPRECATED - use set_data_address
    void set_memory_address(DMA_Channel channel, uint32_t address);                     // DEPRECATED - use set_data_address
    void set_data_address(DMA_Channel channel, Data_Type type, uint32_t address);
    // Transfer count
    void set_transfer_count(DMA_Channel channel, uint32_t count);
    uint32_t get_transfer_count(DMA_Channel channel);
    // Priority
    void set_channel_priority(DMA_Channel channel, Channel_Priority priority);
    // Bit width
    void set_memory_bit_width(DMA_Channel channel, Bit_Width width);                    // DEPRECATED - use set_bit_width
    void set_peripheral_bit_width(DMA_Channel channel, Bit_Width width);                // DEPRECATED - use set_bit_width
    void set_bit_width(DMA_Channel channel, Data_Type type, Bit_Width width);
    // Increase mode
    void memory_incr_enable(DMA_Channel channel);                                       // DEPRECATED - use set_increase_mode_enable
    void memory_incr_disable(DMA_Channel channel);                                      // DEPRECATED - use set_increase_mode_enable
    void peripheral_incr_enable(DMA_Channel channel);                                   // DEPRECATED - use set_increase_mode_enable
    void peripheral_incr_disable(DMA_Channel channel);                                  // DEPRECATED - use set_increase_mode_enable
    void set_increase_mode_enable(DMA_Channel channel, Data_Type type, bool enable);
    // Direction
    void set_transfer_direction(DMA_Channel channel, Transfer_Direction direction);
    // Flags
    bool get_flag(DMA_Channel channel, State_Flags flag);
    void clear_flag(DMA_Channel channel, State_Flags flag);
    // Interrupt flags
    bool get_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag);
    void clear_interrupt_flag(DMA_Channel channel, Interrupt_Flags flag);
    // Interrupts
    void interrupt_enable(DMA_Channel channel, Interrupt_Type interrupt);               // DEPRECATED - use set_interrupt_enable
    void interrupt_disable(DMA_Channel channel, Interrupt_Type interrupt);              // DEPRECATED - use set_interrupt_enable
    void set_interrupt_enable(DMA_Channel channel, Interrupt_Type type, bool enable);
    // Validity
    inline bool channel_validity(DMA_Channel channel);

    // Register address
    volatile uint32_t *reg_address(DMA_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }
    volatile uint32_t *reg_address(DMA_Regs reg, DMA_Channel channel) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(channel) * 0x14U + static_cast<uint32_t>(reg));
    }

    DMA(const DMA&) = delete;
    DMA& operator = (const DMA&) = delete;

    // Stored paramaters
    DMA_Base dma_base_index_;
    DMA_Clock_Config DMA_pclk_info_;

private:
    explicit DMA(DMA_Base Base, DMA_Config& config) : dma_base_index_(Base),
        DMA_pclk_info_(get_clock_config(Base)),
        base_address_(get_base_address(Base)),
        config_(config) {}

    uint32_t base_address_;
    DMA_Config& config_;

    uint32_t get_base_address(DMA_Base Base) {
        return DMA_baseAddress[static_cast<int>(Base)];
    }

    DMA_Clock_Config get_clock_config(DMA_Base Base) {
        return DMA_pclk_index[static_cast<int>(Base)];
    }

    template<typename T>
    inline T read_register(DMA_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline T read_register(DMA_Regs reg, DMA_Channel channel) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg, channel));
    }

    template<typename T>
    inline void write_register(DMA_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    template<typename T>
    inline void write_register(DMA_Regs reg, DMA_Channel channel, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg, channel)) = value;
    }
};

} // namespace dma

// Usage example:
// dma::DMA& dma0 = dma::DMA::instance(dma::DMA_Base::DMA0_BASE);
