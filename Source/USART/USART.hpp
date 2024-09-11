// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "RegRW.hpp"
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

    void init();
    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, true);
        RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, false);
    }
    void release();
    void configure(USART_Config new_config) {
        config_ = new_config;
        init();
    }
    void pin_config_init();
    void pins_configure(USART_Pins pin_config) {
        pin_config_ = pin_config;
        pin_config_init();
    }
    inline void set_baudrate(uint32_t baudrate);
    void set_parity(Parity_Mode parity);
    void set_word_length(Word_Length word_length);
    void set_stop_bits(Stop_Bits stop_bits);
    void enable();
    void disable();
    inline void set_direction(Direction_Mode direction);
    void set_msb(MSBF_Mode msbf);
    void set_inversion_method_enable(Inversion_Method method, bool enable);
    void set_rx_timeout_enable(bool enable);
    void set_rx_timeout_threshold(uint32_t timeout);
    void send_data(uint16_t data);
    uint16_t receive_data();
    void set_wakeup_address(uint8_t address);
    void mute_mode_enable(bool enable);
    void set_mute_mode_wakeup(Wakeup_Mode method);
    void set_half_duplex_enable(bool enable);
    void set_synchronous_clock_enable(bool enable);
    void synchronous_clock_configure(Pulse_Length length, Clock_Phase phase, Clock_Polarity polarity);
    void receive_data_dma(bool enable);
    void send_data_dma(bool enable);
    // LIN mode
    void set_lin_mode_enable(bool enable);
    void set_lin_frame_break_length(Break_Length length);
    void send_lin_frame_break();
    void set_guard_time(uint8_t guard_time);
    // Smartcard mode
    void set_smardcard_mode_enable(bool enable);
    void set_smartcard_nack_mode_enable(bool enable);
    void set_smartcard_auto_retry(uint8_t retry_count);
    void set_smartcard_block_size(uint8_t size);
    // IrDA mode
    void set_irda_mode_enable(bool enable);
    void set_irda_low_power_prescaler(uint8_t prescaler);
    void set_irda_power_mode(IrDA_Power power);
    // HWFC
    void set_hwfc_rts_enable(bool enable);
    void set_hwfc_cts_enable(bool enable);
    // Interrupt and flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    inline volatile uint32_t *reg_address(USART_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}

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

    USART_Config config_;
    USART_Pins pin_config_;

    template <USART_Base Base>
    static USART& get_instance_for_base() {
        static USART instance(Base);
        return instance;
    }
};

} // namespace usart
