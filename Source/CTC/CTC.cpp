// gd32f30x CTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "CTC.hpp"

namespace ctc {

void CTC::set_trim_counter_enable(bool enable) {
    write_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CNTEN), enable ? Set : Clear, true);
}

void CTC::set_irc48m_trim(uint8_t value) {
    write_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRIMVALUE), static_cast<uint32_t>(value), true);
}

void CTC::generate_reference_source_pulse() {
    write_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWREFPUL), Set, true);
}

void CTC::set_hardware_auto_trim_enable(bool enable) {
    write_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::AUTOTRIM), enable ? Set : Clear, true);
}

void CTC::set_reference_source_polarity(Reference_Polarity polarity) {
    write_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::REFPOL), static_cast<uint32_t>(polarity), true);
}

void CTC::set_refenece_source_signal(Reference_Select reference) {
    write_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::REFSEL), static_cast<uint32_t>(reference), true);
}
void CTC::set_reference_source_prescaler(Reference_Prescaler prescaler) {
    write_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::REFPSC), static_cast<uint32_t>(prescaler), true);
}

void CTC::set_clock_trim_limit(uint8_t limit) {
    write_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CKLIM), static_cast<uint32_t>(limit), true);
}

void CTC::set_trim_counter_reload(uint16_t reload) {
    write_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RLVALUE), static_cast<uint32_t>(reload), true);
}

uint16_t CTC::get_trim_counter_capture() {
    return static_cast<uint16_t>(read_bit(*this, CTC_Regs::STAT, static_cast<uint32_t>(STAT_Bits::REFCAP)), true);
}

bool CTC::get_trim_counter_direction() {
    return read_bit(*this, CTC_Regs::STAT, static_cast<uint32_t>(STAT_Bits::REFDIR), true) != Clear;
}


uint16_t CTC::get_trim_counter_reload() {
    return static_cast<uint16_t>(read_bit(*this, CTC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RLVALUE), true));
}

uint8_t CTC::get_trim_irc48m() {
    return static_cast<uint8_t>(read_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TRIMVALUE), true));
}

bool CTC::get_flag(Status_Flags flag) {
    return read_bit(*this, CTC_Regs::STAT, static_cast<uint32_t>(flag), true) != Clear;
}

void CTC::clear_flag(Clear_Flags flag) {
    write_bit(*this, CTC_Regs::INTC, static_cast<uint32_t>(flag), Set, true);
}

bool CTC::get_interrupt_flag(Interrupt_Flags flag) {
    uint32_t enabled = 0;

    if ((static_cast<uint32_t>(flag) & static_cast<uint32_t>(Interrupt_Flags::INTR_ERRIC_FLAG_MASK)) != Clear) {
        enabled = read_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), true);
    } else {
        enabled = read_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(flag), true);
    }
    uint32_t status_flag = read_bit(*this, CTC_Regs::STAT, static_cast<uint32_t>(flag), true);

    return (enabled && status_flag);
}

void CTC::clear_interrupt_flag(Clear_Flags flag) {
    if ((static_cast<uint32_t>(flag) & static_cast<uint32_t>(Interrupt_Flags::INTR_ERRIC_FLAG_MASK)) != Clear) {
        write_bit(*this, CTC_Regs::INTC, static_cast<uint32_t>(INTC_Bits::ERRIC), Set, true);
    } else {
        write_bit(*this, CTC_Regs::INTC, static_cast<uint32_t>(flag), Set, true);
    }
}

void CTC::set_interrupt_enable(Interrupt_Type type, bool enable) {
    write_bit(*this, CTC_Regs::CTL0, static_cast<uint32_t>(type), enable ? Set : Clear, true);
}

} // namespace ctc

ctc::CTC CTC_DEVICE;
