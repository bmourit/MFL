// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "GPIO.hpp"
#include "usart_config.hpp"

namespace usart {

class USART {
public:
    static Result<USART, USART_Error_Type> get_instance(USART_Base Base) {
        switch (Base) {
        case USART_Base::USART0_BASE:
            return get_enum_instance<USART_Base, USART, USART_Error_Type>(
                       Base, USART_Base::USART0_BASE, get_instance_for_base<USART_Base::USART0_BASE>()
                   );
        case USART_Base::USART1_BASE:
            return get_enum_instance<USART_Base, USART, USART_Error_Type>(
                       Base, USART_Base::USART1_BASE, get_instance_for_base<USART_Base::USART1_BASE>()
                   );
        case USART_Base::USART2_BASE:
            return get_enum_instance<USART_Base, USART, USART_Error_Type>(
                       Base, USART_Base::USART2_BASE, get_instance_for_base<USART_Base::USART2_BASE>()
                   );
        case USART_Base::UART3_BASE:
            return get_enum_instance<USART_Base, USART, USART_Error_Type>(
                       Base, USART_Base::UART3_BASE, get_instance_for_base<USART_Base::UART3_BASE>()
                   );
        case USART_Base::UART4_BASE:
            return get_enum_instance<USART_Base, USART, USART_Error_Type>(
                       Base, USART_Base::UART4_BASE, get_instance_for_base<USART_Base::UART4_BASE>()
                   );
        default:
            return RETURN_ERROR(USART, USART_Error_Type::INVALID_USART);
        }
    }

    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, true);
        RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, false);
    }
    void init();
    void configure(USART_Config* config) {
        if (config) {
            config_ = *config;
        }
        init();
    }
    inline void set_baudrate(uint32_t baudrate);
    void set_parity(Parity_Mode parity);
    void set_word_length(Word_Length word_length);
    void set_stop_bits(Stop_Bits stop_bits);
    void enable();
    void disable();
    inline void set_direction(Direction_Mode direction);
    void set_msb(MSBF_Mode msbf);
    void rx_timeout_enable(bool enable);
    void set_rx_timeout_threshold(uint32_t timeout);
    void send_data(uint16_t data);	// TX
    uint16_t receive_data();        // RX
    void set_address(uint8_t address);
    void mute_mode_enable(bool enable);
    void configure_mute_mode_wakeup(Wakeup_Mode method);
    void set_halfduplex_enable(bool enable);
    void synchronous_clock_enable(bool enable);
    void receive_data_dma(bool enable);
    void send_data_dma(bool enable);
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    void interrupt_enable(Interrupt_Type type);
    void interrupt_disable(Interrupt_Type type);
    void set_interrupt_enable(Interrupt_Type type, bool enable);
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);

    inline volatile uint32_t *reg_address(USART_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    USART_Base base_index_;

private:
    USART(USART_Base Base) : base_index_(Base),
        USART_pclk_info_(USART_pclk_index[static_cast<int>(Base)]),
        base_address_(USART_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(USART_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    USART_Clock_Config USART_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

    // Default dummy config
    USART_Config default_config = {};
    USART_Config& config_ = default_config;

    template <USART_Base Base>
    static USART& get_instance_for_base() {
        static USART instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(USART_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(USART_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace usart
