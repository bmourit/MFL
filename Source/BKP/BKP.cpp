// gd32f30x BKP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "BKP.hpp"

// Initialize the static member
bool bkp::BKP::is_clock_enabled = false;

namespace bkp {

#define TAMPER_FLAG_SHIFT          ((uint8_t)8U)

void BKP::set_data(Backup_Data datax, uint16_t data) {
    uint32_t reg_offset;
    if (static_cast<uint32_t>(datax) <= static_cast<uint32_t>(Backup_Data::DATA_9)) {
        // For DATA_0 to DATA_9
        reg_offset = 0x04 + (static_cast<uint32_t>(datax) - 1) * 0x04;
    } else if (static_cast<uint32_t>(datax) <= static_cast<uint32_t>(Backup_Data::DATA_41)) {
        // For DATA_10 to DATA_41
        reg_offset = 0x40 + (static_cast<uint32_t>(datax) - 10) * 0x04;
    } else {
        return;
    }

    // Write the data to the computed register address
    *reinterpret_cast<volatile uint16_t*>(BKP_baseAddress + reg_offset) = data;
}

uint16_t BKP::get_data(Backup_Data datax) {
    uint32_t reg_offset;

    if (static_cast<uint32_t>(datax) <= static_cast<uint32_t>(Backup_Data::DATA_9)) {
        // For DATA_0 to DATA_9
        reg_offset = 0x04 + (static_cast<uint32_t>(datax) - 1) * 0x04;
    } else if (static_cast<uint32_t>(datax) <= static_cast<uint32_t>(Backup_Data::DATA_41)) {
        // For DATA_10 to DATA_41
        reg_offset = 0x40 + (static_cast<uint32_t>(datax) - 10) * 0x04;
    } else {
        return 0;
    }
    return *reinterpret_cast<volatile uint16_t*>(BKP_baseAddress + reg_offset);
}

void BKP::set_rtc_output_calibration_enable(bool enable) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::COEN), enable ? Set : Clear);
}

void BKP::set_rtc_output_signal_enable(bool enable) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::ASOEN), enable ? Set : Clear);
}

void BKP::set_rtc_output_pulse(Output_Pulse pulse) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::ROSEL), (pulse == Output_Pulse::SECOND_PULSE) ? OUTPUT_SECOND_PULSE : Clear);
}

void BKP::set_rtc_clock_divider(Clock_Divider divider) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::CCOSEL), (divider == Clock_Divider::DIV_1) ? CLOCK_DIV_1 : Clear);
}

void BKP::set_rtc_clock_calibration_type(Calibration_Type type) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::CALDIR), (type == Calibration_Type::SPEED_UP) ? CLOCK_SPEED_UP : Clear);
}

void BKP::set_rtc_calibration_value(uint8_t value) {
    write_bit(*this, BKP_Regs::OCTL, static_cast<uint32_t>(OCTL_Bits::RCCV), static_cast<uint32_t>(value));
}

void BKP::set_tamper_detection_enable(bool enable) {
    write_bit(*this, BKP_Regs::TPCTL, static_cast<uint32_t>(TPCTL_Bits::TPEN), enable ? Set : Clear);
}

void BKP::set_tamper_level(Tamper_Level level) {
    write_bit(*this, BKP_Regs::TPCTL, static_cast<uint32_t>(TPCTL_Bits::TPAL), static_cast<uint32_t>(level));
}

void BKP::set_tamper_interrupt_enable(bool enable) {
    write_bit(*this, BKP_Regs::TPCS, static_cast<uint32_t>(TPCS_Bits::TPIE), enable ? Set : Clear);
}

bool BKP::get_flag(Status_Flags flag) {
    return (read_bit(*this, BKP_Regs::TPCS, static_cast<uint32_t>(flag)) != Clear);
}

void BKP::clear_flag(Clear_Flags flag) {
    write_bit(*this, BKP_Regs::TPCS, static_cast<uint32_t>(flag), Set);
}

bool BKP::get_interrupt_flag(Interrupt_Flags flag) {
    return (read_bit(*this, BKP_Regs::TPCS, static_cast<uint32_t>(flag)) != Clear);
}

void BKP::clear_interrupt_flag(Clear_Flags flag) {
    write_bit(*this, BKP_Regs::TPCS, static_cast<uint32_t>(flag), Set);
}

} // namespace bkp

bkp::BKP BKP_DEVICE;
