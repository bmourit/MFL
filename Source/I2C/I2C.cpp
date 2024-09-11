// gd32f30x I2C peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <algorithm>

#include "I2C.hpp"

// Initialize the static member
bool i2c::I2C::is_clock_enabled = false;

namespace i2c {

//
// Initialize the I2C gpio pins according to their set parameters
//
void I2C::pin_config_init() {
    auto configure_pin = [](gpio::GPIO& port, const I2C_Pin_Config& pin_config) {
        port.init_pin(pin_config.pin, pin_config.mode, pin_config.speed);
    };

    auto sda_result = gpio::GPIO::get_instance(pin_config_.sda_pin.gpio_port);
    if (sda_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& sda_port = sda_result.value();
    configure_pin(sda_port, pin_config_.sda_pin);

    if (pin_config_.sda_pin.gpio_port != pin_config_.scl_pin.gpio_port) {
        auto scl_result = gpio::GPIO::get_instance(pin_config_.scl_pin.gpio_port);
        if (scl_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& scl_port = scl_result.value();
        configure_pin(scl_port, pin_config_.scl_pin);
    } else {
        configure_pin(sda_port, pin_config_.scl_pin);
    }
}

//
// Standard mode max: 100 kHz
// Fast mode max: 400 kHz
// Fast mode plus max: 1MHz
//
I2C_Error_Type I2C::set_clock_speed_duty(uint32_t speed, Duty_Cycle duty) {
    if (speed == 0) {
        return I2C_Error_Type::INVALID_CLOCK_FREQUENCY;
    }

    const uint32_t apb1_clock = RCU_DEVICE.get_clock_frequency(rcu::Clock_Frequency::CK_APB1);
    const uint32_t frequency = std::min(apb1_clock / 1'000'000, MaximumClockSpeed);

    write_bit(*this, I2C_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::I2CCLK), frequency);

    uint32_t rise_time = (apb1_clock / 1'000'000) + 1;
    uint32_t clkc;

    if (speed <= 100'000) {
        // Standard mode, max SCL rise time is 1000ns
        rise_time = std::clamp(rise_time, MinimumClockSpeed, MaximumClockSpeed);
        write_register(*this, I2C_Regs::RT, rise_time);

        clkc = apb1_clock / (speed * 2);
        clkc = std::max(clkc, uint32_t{4});  // Standard mode, min is 4
        write_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::CLKC), clkc);

    } else if (speed <= 400'000) {
        // Fast mode, max SCL rise time is 300ns
        write_register(*this, I2C_Regs::RT, (frequency * 300) / 1000 + 1);

        if (duty == Duty_Cycle::DTCY_2) {
            clkc = apb1_clock / (speed * 3);
            write_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::DTCY), static_cast<uint32_t>(Duty_Cycle::DTCY_2));
        } else {
            clkc = (apb1_clock / (speed * 25));
            write_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::DTCY), static_cast<uint32_t>(Duty_Cycle::DTCY_16_9));
        }

        if (read_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::CLKC)) == 0) {
            clkc |= 1;
        }

        write_bits(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::FAST), Set,
                   static_cast<uint32_t>(CKCFG_Bits::CLKC), clkc);

    } else {
        // Fast mode plus, max SCL rise time is 120ns
        write_register(*this, I2C_Regs::RT, (frequency * 120) / 1000 + 1);

        if (duty == Duty_Cycle::DTCY_2) {
            clkc = (apb1_clock / (speed * 3));
            write_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::DTCY), static_cast<uint32_t>(Duty_Cycle::DTCY_2));
        } else {
            clkc = (apb1_clock / (speed * 25));
            write_bit(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::DTCY), static_cast<uint32_t>(Duty_Cycle::DTCY_16_9));
        }

        write_bits(*this, I2C_Regs::CKCFG, static_cast<uint32_t>(CKCFG_Bits::FAST), Set,
                   static_cast<uint32_t>(CKCFG_Bits::CLKC), clkc);

        write_bit(*this, I2C_Regs::FMPCFG, static_cast<uint32_t>(FMPCFG_Bits::FMPEN), Set);
    }

    return I2C_Error_Type::OK;
}

void I2C::set_address_format(uint32_t address, Address_Format format, Bus_Mode mode) {
    address &= AddressMask;
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SMBEN), static_cast<uint32_t>(mode));
    write_bits(*this, I2C_Regs::SADDR0, static_cast<uint32_t>(SADDR0_Bits::ADDFORMAT), static_cast<uint32_t>(format),
               static_cast<uint32_t>(SADDR0_Bits::ADDRESS_MASK), address);
}

void I2C::set_smbus_type(Bus_Type type) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SMBSEL), static_cast<uint32_t>(type));
}

void I2C::set_ack_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ACKEN), enable ? Set : Clear);
}

void I2C::set_ack_position(ACK_Select select) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::POAP), static_cast<uint32_t>(select));
}

void I2C::set_direction_address(Transfer_Direction direction, uint32_t address) {
    if (direction == Transfer_Direction::TRANSMIT) {
        address &= ~1U; // Clear LSB for transmit
    } else {
        address |= 0x00000001;  // Set LSB for receive
    }
    write_register(*this, I2C_Regs::DATA, address);
}

void I2C::set_dual_address_enable(uint32_t address, bool enable) {
    if (enable) {
        address &= Address2Mask;
        write_bit(*this, I2C_Regs::SADDR1, static_cast<uint32_t>(SADDR1_Bits::ADDRESS2), address);
    }

    write_bit(*this, I2C_Regs::SADDR1, static_cast<uint32_t>(SADDR1_Bits::DUADEN), enable ? Set : Clear);
}

void I2C::set_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::I2CEN), enable ? Set : Clear);
}

void I2C::generate_start_condition() {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), Set);
}

void I2C::generate_stop_condition() {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::STOP), Set);
}

void I2C::transmit_data(uint8_t data) {
    write_bit(*this, I2C_Regs::DATA, static_cast<uint32_t>(DATA_Bits::TRB), static_cast<uint32_t>(data));
}

uint8_t I2C::receive_data() {
    return read_bit8(*this, I2C_Regs::DATA, static_cast<uint32_t>(DATA_Bits::TRB));
}

void I2C::set_dma_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAON), enable ? Set : Clear);
}

void I2C::set_dma_transfer_end(bool is_end) {
    write_bit(*this, I2C_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMALST), is_end ? Set : Clear);
}

void I2C::set_stretch_low(Stretch_Low stretch) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SS), (stretch == Stretch_Low::SCLSTRETCH_ENABLE) ? Clear : Set);
}

void I2C::set_general_call_respond(bool respond) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::GCEN), respond ? Set : Clear);
}

void I2C::set_software_reset_enable(bool reset) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SRESET), reset ? Set : Clear);
}

void I2C::set_pec_calculate(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PECEN), enable ? Set : Clear);
}

void I2C::set_pec_transfer_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PECTRANS), enable ? Set : Clear);
}

uint8_t I2C::get_pec() {
    return read_bit8(*this, I2C_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PECV));
}

void I2C::set_smbus_alert_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SALT), enable ? Set : Clear);
}

void I2C::set_smbus_arp_enable(bool enable) {
    write_bit(*this, I2C_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ARPEN), enable ? Set : Clear);
}

bool I2C::get_flag(Status_Flags flag) {
    return get_value(flag);
}

void I2C::clear_flag(Clear_Flags flag) {
    if (flag == Clear_Flags::FLAG_ADDSEND) {
        // Read STAT0 and STAT1 to clear
        read_register<uint32_t>(*this, I2C_Regs::STAT0);
        read_register<uint32_t>(*this, I2C_Regs::STAT1);
    } else {
        const auto& info = clear_flag_index[static_cast<size_t>(flag)];
        write_bit(*this, info.register_offset, info.bit_info, Clear);
    }
}

bool I2C::get_interrupt_flag(Interrupt_Flags flag) {
    return get_value(flag);
}

void I2C::clear_interrupt_flag(Clear_Flags flag) {
    if (flag == Clear_Flags::FLAG_ADDSEND) {
        // Read STAT0 and STAT1 to clear
        read_register<uint32_t>(*this, I2C_Regs::STAT0);
        read_register<uint32_t>(*this, I2C_Regs::STAT1);
    } else {
        const auto& info = clear_flag_index[static_cast<size_t>(flag)];
        write_bit(*this, info.register_offset, info.bit_info, Clear);
    }
}

void I2C::set_interrupt_enable(Interrupt_Type type, bool enable) {
    const auto& info = interrupt_type_index[static_cast<size_t>(type)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

} // namespace i2c
