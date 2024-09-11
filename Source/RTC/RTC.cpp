// gd32f30x RTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "RTC.hpp"

// Initialize the static member
bool rtc::RTC::is_clock_enabled = false;

namespace rtc {

void RTC::start_configuration()
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CMF), Set, true);
}

void RTC::stop_configuration()
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CMF), Clear, true);
}

void RTC::lwoff_wait()
{
    // Loop until LWOFF flag gets set
    while (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LWOFF), true) == Clear) {
    }
}

void RTC::sync_register_wait()
{
    // Clear RSYNF
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RSYNF), Clear, true);
    // Loop until RSYNF flag gets set
    while (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RSYNF), true) == Clear) {
    }
}

uint32_t RTC::get_counter()
{
    return ((read_bit(*this, RTC_Regs::CNTH, static_cast<uint32_t>(CNTH_Bits::HIGH_CNT), true) << 16) | read_bit(*this, RTC_Regs::CNTL, static_cast<uint32_t>(CNTL_Bits::LOW_CNT), true));
}

void RTC::set_counter(uint32_t counter)
{
    start_configuration();
    // Set CNTH
    write_bit(*this, RTC_Regs::CNTH, static_cast<uint32_t>(CNTH_Bits::HIGH_CNT), counter >> 16, true);
    // Set CNTL
    write_bit(*this, RTC_Regs::CNTL, static_cast<uint32_t>(CNTL_Bits::LOW_CNT), counter & 0x0000FFFF, true);
    stop_configuration();
}

void RTC::set_prescaler(uint32_t prescaler)
{
    start_configuration();
    // Set PSCH
    write_bit(*this, RTC_Regs::PSCH, static_cast<uint32_t>(PSCH_Bits::HIGH_PSC), (prescaler & 0x000F0000) >> 16, true);
    // Set PSCL
    write_bit(*this, RTC_Regs::PSCL, static_cast<uint32_t>(PSCL_Bits::LOW_PSC), prescaler & 0x0000FFFF, true);
    stop_configuration();
}

void RTC::set_alarm(uint32_t alarm)
{
    start_configuration();
    // Set ALRMH
    write_bit(*this, RTC_Regs::ALRMH, static_cast<uint32_t>(ALRMH_Bits::HIGH_ALRM), alarm >> 16, true);
    // Set ALRML
    write_bit(*this, RTC_Regs::ALRML, static_cast<uint32_t>(ALRML_Bits::LOW_ALRM), alarm & 0x0000FFFF, true);
    stop_configuration();
}

uint32_t RTC::get_divider() {
    return ((read_bit(*this, RTC_Regs::DIVH, static_cast<uint32_t>(DIVH_Bits::HIGH_DIV), true) << 16) | read_bit(*this, RTC_Regs::DIVL, static_cast<uint32_t>(DIVL_Bits::LOW_DIV), true));
}

bool RTC::get_flag(Status_Flags flag)
{
    return (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(flag), true) ? true : false);
}

void RTC::clear_flag(Status_Flags flag)
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(flag), Clear, true);
}

void RTC::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    write_bit(*this, RTC_Regs::INTEN, static_cast<uint32_t>(type), enable ? Set : Clear, true);
}

} // namespace rtc

// Instantiate class of global device
rtc::RTC RTC_DEVICE;
