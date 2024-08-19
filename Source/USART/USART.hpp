// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "usart_config.hpp"

#define MAX_USARTS  5

namespace usart {

class USART {
public:
    static USART& instance(USART_Base Base) {
        static USART instances[MAX_USARTS] = {
            USART(USART_Base::USART0_BASE),
            USART(USART_Base::USART1_BASE),
            USART(USART_Base::USART2_BASE),
            USART(USART_Base::UART3_BASE),
            USART(USART_Base::UART4_BASE),
        };
        return instances[static_cast<int>(Base)];
    }

    void reset();
    void set_pclk_enable(bool enable);
    void set_baudrate(uint32_t baudrate);
    void set_parity(Parity_Mode parity);
    void set_word_length(Word_Length word_length);
    void set_stop_bits(Stop_Bits stop_bits);
    void enable();
    void disable();
    void set_direction(Direction_Mode mode);
    void set_msb(MSBF_Mode msbf);
    void rx_timeout_enable(Bit_State state);
    void set_rx_timeout_threshold(uint32_t timeout);
    void send_data(uint16_t data);	// TX
    uint16_t receive_data();        // RX
    void set_address(uint8_t address);
    void mute_mode_enable(Bit_State state);
    void configure_mute_mode_wakeup(Wakeup_Mode wakeup_method);
    void set_halfduplex_enable(Bit_State state);
    void synchronous_clock_enable(Bit_State state);
    void receive_data_dma(Bit_State state);
    void send_data_dma(Bit_State state);
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    void interrupt_enable(Interrupt_Type interrupt);
    void interrupt_disable(Interrupt_Type interrupt);
    bool get_interrupt_flag(Interrupt_Flags interrupt_flag);
    void clear_interrupt_flag(Interrupt_Flags interrupt_flag);

    inline volatile uint32_t *reg_address(USART_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    USART_Base base_index_;
    USART_Clock_Config USART_pclk_info_;

private:
    explicit USART(USART_Base Base) : base_index_(Base),
        USART_pclk_info_(get_clock_config(Base)),
        base_address_(get_base_address(Base)) {}

    uint32_t base_address_;

    template<typename T>
    inline T read_register(USART_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(USART_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    uint32_t get_base_address(USART_Base Base) {
        return USART_baseAddress[static_cast<int>(Base)];
    }

    USART_Clock_Config get_clock_config(USART_Base Base) {
        return USART_pclk_index[static_cast<int>(Base)];
    }
};

} // namespace usart

// Usage example:
// USART& usart0 = USART::instance(USART_Base::USART0);
// uint32_t data = usart0.read_register<uint32_t>(USART_Regs::DATA);
