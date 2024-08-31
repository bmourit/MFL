// gd32f30x RTC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "RTC.hpp"

namespace rtc {

void RTC::interrupt_enable(Interrupt_Type type)
{
    write_bit(*this, RTC_Regs::INTEN, static_cast<uint32_t>(type), Set);
}

void RTC::interrupt_disable(Interrupt_Type type)
{
    write_bit(*this, RTC_Regs::INTEN, static_cast<uint32_t>(type), Clear);
}

void RTC::start_configuration()
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CMF), Set);
}

void RTC::stop_configuration()
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CMF), Clear);
}

void RTC::lwoff_wait()
{
    // Loop until LWOFF flag gets set
    while (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LWOFF)) == Clear) {
    }
}

void RTC::sync_register_wait()
{
    // Clear RSYNF
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RSYNF), Clear);
    // Loop until RSYNF flag gets set
    while (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RSYNF)) == Clear) {
    }
}

uint32_t RTC::get_counter()
{
    return ((read_bit(*this, RTC_Regs::CNTH, static_cast<uint32_t>(CNTH_Bits::HIGH_CNT)) << 16) | read_bit(*this, RTC_Regs::CNTL, static_cast<uint32_t>(CNTL_Bits::LOW_CNT)));
}

void RTC::set_counter(uint32_t counter)
{
    start_configuration();
    // Set CNTH
    write_bit(*this, RTC_Regs::CNTH, static_cast<uint32_t>(CNTH_Bits::HIGH_CNT), counter >> 16);
    // Set CNTL
    write_bit(*this, RTC_Regs::CNTL, static_cast<uint32_t>(CNTL_Bits::LOW_CNT), counter & 0x0000FFFFU);
    stop_configuration();
}

void RTC::set_prescaler(uint32_t prescaler)
{
    start_configuration();
    // Set PSCH
    write_bit(*this, RTC_Regs::PSCH, static_cast<uint32_t>(PSCH_Bits::HIGH_PSC), (prescaler & 0x000F0000U) >> 16);
    // Set PSCL
    write_bit(*this, RTC_Regs::PSCL, static_cast<uint32_t>(PSCL_Bits::LOW_PSC), prescaler & 0x0000FFFFU);
    stop_configuration();
}

void RTC::set_alarm(uint32_t alarm)
{
    start_configuration();
    // Set ALRMH
    write_bit(*this, RTC_Regs::ALRMH, static_cast<uint32_t>(ALRMH_Bits::HIGH_ALRM), alarm >> 16);
    // Set ALRML
    write_bit(*this, RTC_Regs::ALRML, static_cast<uint32_t>(ALRML_Bits::LOW_ALRM), alarm & 0x0000FFFFU);
    stop_configuration();
}

uint32_t RTC::get_divider()
{
    return ((read_bit(*this, RTC_Regs::DIVH, static_cast<uint32_t>(DIVH_Bits::HIGH_DIV)) << 16) | read_bit(*this, RTC_Regs::DIVL, static_cast<uint32_t>(DIVL_Bits::LOW_DIV)));
}

bool RTC::get_flag(Status_Flags flag)
{
    return (read_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(flag)) ? true : false);
}

void RTC::clear_flag(Status_Flags flag)
{
    write_bit(*this, RTC_Regs::CTL, static_cast<uint32_t>(flag), Clear);
}

} // namespace rtc

// Instantiate class of global device
rtc::RTC RTC_DEVICE;
