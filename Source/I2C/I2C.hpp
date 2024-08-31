// gd32f30x I2C peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "GPIO.hpp"
#include "i2c_config.hpp"

namespace i2c {

class I2C {
public:
    static Result<I2C, I2C_Error_Type> get_instance(I2C_Base Base) {
        switch (Base) {
        case I2C_Base::I2C0_BASE:
            return get_enum_instance<I2C_Base, I2C, I2C_Error_Type>(
                       Base, I2C_Base::I2C0_BASE, get_instance_for_base<I2C_Base::I2C0_BASE>()
                   );
        case I2C_Base::I2C1_BASE:
            return get_enum_instance<I2C_Base, I2C, I2C_Error_Type>(
                       Base, I2C_Base::I2C1_BASE, get_instance_for_base<I2C_Base::I2C1_BASE>()
                   );
        default:
            return RETURN_ERROR(I2C, I2C_Error_Type::INVALID_I2C);
        }
    }

    // Initialize
    void init();
    // Reset
    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(I2C_pclk_info_.reset_reg, true);
        RCU_DEVICE.set_pclk_reset_enable(I2C_pclk_info_.reset_reg, false);
    }
    // Configure
    void configure(I2C_Config* config) {
        if (config) {
            config_ = *config;
        }
        init();
    }
    I2C_Error_Type set_clock_speed_duty(uint32_t speed, Duty_Cycle duty);
    void set_address_format(uint32_t address, Address_Format format, Bus_Mode mode);
    void set_smbus_type(Bus_Type type);
    void set_ack_enable(bool enable);
    void set_ack_position(ACK_Select position);
    void set_direction_address(Transfer_Direction direction, uint32_t address);
    void set_dual_address_enable(uint32_t address, bool enable);
    void set_enable(bool enable);
    void generate_start_condition();
    void generate_stop_condition();
    void transmit_data(uint8_t data);
    uint8_t receive_data();
    void set_dma_enable(bool enable);
    void set_dma_transfer_end(bool is_end);
    void set_stretch_low(Stretch_Low stretch);
    void set_general_call_respond(bool respond);
    void set_software_reset_enable(bool reset);
    void set_pec_calculate(bool enable);
    void set_pec_transfer_enable(bool enable);
    uint8_t get_pec();
    void set_smbus_alert_enable(bool enable);
    void set_smbus_arp_enable(bool enable);
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Clear_Flags flag);
    // Interrupt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Clear_Flags flag);
    // Interrupt enable/disable
    void set_interrupt_enable(Interrupt_Type type, bool enable);

    inline volatile uint32_t *reg_address(I2C_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

private:
    I2C(I2C_Base Base) : I2C_pclk_info_(I2C_pclk_index[static_cast<int>(Base)]),
        base_address_(I2C_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(I2C_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(I2C_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(I2C_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    I2C_Clock_Config I2C_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

    I2C_Config default_config = {};
    I2C_Config config_ = default_config;

    template <I2C_Base Base>
    static I2C& get_instance_for_base() {
        static I2C instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(I2C_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(I2C_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    bool get_value(Status_Flags flag) const {
        const auto &info = status_flag_index[static_cast<size_t>(flag)];
        uint32_t reg_value = *reg_address(info.register_offset);

        const uint32_t width = info.bit_info & 0xFF;
        const uint32_t bitno = info.bit_info >> 16;

        reg_value >>= bitno;
        reg_value &= ((1 << width) - 1);

        return reg_value;
    }

    bool get_value(Interrupt_Flags flag) const {
        const auto &info = interrupt_flag_index[static_cast<size_t>(flag)];
        uint32_t reg_value0 = *reg_address(info.register0_offset);
        uint32_t reg_value1 = *reg_address(info.register1_offset);
        uint32_t buff_enable = read_bit(*this, I2C_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::BUFIE));

        const uint32_t width0 = info.bit_info0 & 0xFF;
        const uint32_t bitno0 = info.bit_info0 >> 16;
        const uint32_t width1 = info.bit_info1 & 0xFF;
        const uint32_t bitno1 = info.bit_info1 >> 16;

        reg_value0 >>= bitno0;
        reg_value1 >>= bitno1;
        reg_value0 &= ((1 << width0) - 1);
        reg_value1 &= ((1 << width1) - 1);

        if ((flag == Interrupt_Flags::INTR_FLAG_RBNE) || (flag == Interrupt_Flags::INTR_FLAG_TBE)) {
            reg_value0 = (reg_value0 && buff_enable) ? 1 : 0;
        }

        return ((reg_value0) && (reg_value1));
    }
};

} // namespace i2c
