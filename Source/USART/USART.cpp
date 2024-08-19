// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "USART.hpp"

namespace usart {

void USART::reset()
{
    RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, true);
    RCU_DEVICE.set_pclk_reset_enable(USART_pclk_info_.reset_reg, false);
}

void USART::set_pclk_enable(bool enable)
{
    RCU_DEVICE.set_pclk_enable(USART_pclk_info_.clock_reg, enable ? true : false);
}

void USART::set_baudrate(uint32_t baudrate)
{
    uint32_t freq, value = 0;

    if (base_index_ == USART_Base::USART0_BASE) {
        freq = RCU_DEVICE.get_clock_frequency(rcu::Clock_Frequency::CK_APB2);
    } else {
        freq = RCU_DEVICE.get_clock_frequency(rcu::Clock_Frequency::CK_APB1);
    }

    // Configure the baudrate oversampling by 16
    value = (freq + baudrate / 2U) / baudrate;
    write_register(USART_Regs::BAUD, value);
}

void USART::set_parity(Parity_Mode parity)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PMEN), static_cast<uint32_t>(parity));
}

void USART::set_word_length(Word_Length word_length)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WL), static_cast<uint32_t>(word_length));
}

void USART::set_stop_bits(Stop_Bits stop_bits)
{
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::STB), static_cast<uint32_t>(stop_bits));
}

void USART::enable()
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UEN), 1);
}

void USART::disable()
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UEN), 0);
}

void USART::set_direction(Direction_Mode mode)
{
    using enum Direction_Mode;
    switch (mode) {
    case RX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), 0);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), 1);
        break;
    case TX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), 0);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), 1);
        break;
    case RXTX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), 1);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), 1);
        break;
    case RXTX_OFF:
    default:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), 0);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), 0);
        break;
    }
}

void USART::set_msb(MSBF_Mode msbf)
{
    write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::MSBF), static_cast<uint32_t>(msbf));
}

void USART::rx_timeout_enable(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTEN), static_cast<uint32_t>(state));
}

void USART::set_rx_timeout_threshold(uint32_t timeout)
{
    write_bit(*this, USART_Regs::RT, static_cast<uint32_t>(RT_Bits::RT), static_cast<uint32_t>(timeout));
}

void USART::send_data(uint16_t data)
{
    write_bit(*this, USART_Regs::DATA, static_cast<uint32_t>(DATA_Bits::DATA), static_cast<uint32_t>(data));
}

uint16_t USART::receive_data()
{
    uint32_t read_value = read_bit(*this, USART_Regs::DATA, static_cast<uint32_t>(DATA_Bits::DATA));
    return static_cast<uint16_t>(read_value);
}

void USART::set_address(uint8_t addr)
{
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADDR), static_cast<uint32_t>(addr));
}

void USART::mute_mode_enable(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWU), static_cast<uint32_t>(state));
}

void USART::configure_mute_mode_wakeup(Wakeup_Mode wakeup_mode)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WM), static_cast<uint32_t>(wakeup_mode));
}

void USART::set_halfduplex_enable(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::HDEN), static_cast<uint32_t>(state));
}

void USART::synchronous_clock_enable(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CKEN), static_cast<uint32_t>(state));
}

void USART::receive_data_dma(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::DENR), static_cast<uint32_t>(state));
}

void USART::send_data_dma(Bit_State state)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::DENT), static_cast<uint32_t>(state));
}

bool USART::get_flag(Status_Flags flag)
{
    uint32_t value = 0;

    using enum Status_Flags;
    switch (flag) {
    case FLAG_PERR:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PERR));
        break;
    case FLAG_FERR:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::FERR));
        break;
    case FLAG_NERR:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::NERR));
        break;
    case FLAG_ORERR:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR));
        break;
    case FLAG_IDLEF:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::IDLEF));
        break;
    case FLAG_RBNE:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::RBNE));
        break;
    case FLAG_TC:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TC));
        break;
    case FLAG_TBE:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TBE));
        break;
    case FLAG_LBDF:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::LBDF));
        break;
    case FLAG_CTSF:
        value = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::CTSF));
        break;
    case FLAG_RTF:
        value = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::RTF));
        break;
    case FLAG_EBF:
        value = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::EBF));
        break;
    case FLAG_BSY:
        value = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BSY));
        break;
    default:
        break;
    }

    return (value != 0) ? true : false;
}

void USART::clear_flag(Status_Flags flag)
{
    using enum Status_Flags;
    switch (flag) {
    case FLAG_PERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PERR), 0);
        break;
    case FLAG_FERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::FERR), 0);
        break;
    case FLAG_NERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::NERR), 0);
        break;
    case FLAG_ORERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR), 0);
        break;
    case FLAG_IDLEF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::IDLEF), 0);
        break;
    case FLAG_RBNE:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::RBNE), 0);
        break;
    case FLAG_TC:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TC), 0);
        break;
    case FLAG_TBE:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TBE), 0);
        break;
    case FLAG_LBDF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::LBDF), 0);
        break;
    case FLAG_CTSF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::CTSF), 0);
        break;
    case FLAG_RTF:
        write_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::RTF), 0);
        break;
    case FLAG_EBF:
        write_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::EBF), 0);
        break;
    case FLAG_BSY:
        write_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BSY), 0);
        break;
    default:
        break;
    }
}

void USART::interrupt_enable(Interrupt_Type interrupt)
{
    using enum Interrupt_Type;
    switch (interrupt) {
    case INTR_PERRIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PERRIE), 1);
        break;
    case INTR_TBEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TBEIE), 1);
        break;
    case INTR_TCIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TCIE), 1);
        break;
    case INTR_RBNEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE), 1);
        break;
    case INTR_IDLEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IDLEIE), 1);
        break;
    case INTR_LBDIE:
        write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LBDIE), 1);
        break;
    case INTR_CTSIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::CTSIE), 1);
        break;
    case INTR_ERRIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE), 1);
        break;
    case INTR_EBIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::EBIE), 1);
        break;
    case INTR_RTIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTIE), 1);
        break;
    default:
        break;
    }
}

void USART::interrupt_disable(Interrupt_Type interrupt)
{
    using enum Interrupt_Type;
    switch (interrupt) {
    case INTR_PERRIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PERRIE), 0);
        break;
    case INTR_TBEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TBEIE), 0);
        break;
    case INTR_TCIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TCIE), 0);
        break;
    case INTR_RBNEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE), 0);
        break;
    case INTR_IDLEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IDLEIE), 0);
        break;
    case INTR_LBDIE:
        write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LBDIE), 0);
        break;
    case INTR_CTSIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::CTSIE), 0);
        break;
    case INTR_ERRIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE), 0);
        break;
    case INTR_EBIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::EBIE), 0);
        break;
    case INTR_RTIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTIE), 0);
        break;
    default:
        break;
    }
}

bool USART::get_interrupt_flag(Interrupt_Flags interrupt_flag)
{
    uint32_t flag = 0;
    uint32_t state = 0;

    using enum Interrupt_Flags;
    switch (interrupt_flag) {
    case INTR_FLAG_CTL0_PERR:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PERR));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PERRIE));
        break;
    case INTR_FLAG_CTL0_TBE:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TBE));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TBEIE));
        break;
    case INTR_FLAG_CTL0_TC:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TC));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TCIE));
        break;
    case INTR_FLAG_CTL0_RBNE:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::RBNE));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE));
        break;
    case INTR_FLAG_CTL0_ORERR:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE));
        break;
    case INTR_FLAG_CTL0_IDLEF:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::IDLEF));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IDLEIE));
        break;
    case INTR_FLAG_CTL1_LBDF:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::LBDF));
        state = read_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LBDIE));
        break;
    case INTR_FLAG_CTL2_CTSF:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::CTSF));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::CTSIE));
        break;
    case INTR_FLAG_CTL2_ORERR:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL2_NERR:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::NERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL2_FERR:
        flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::FERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL3_EBF:
        flag = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::EBF));
        state = read_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::EBIE));
        break;
    case INTR_FLAG_CTL3_RTF:
        flag = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::RTF));
        state = read_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTIE));
        break;
    default:
        break;
    }

    return ((flag != 0) && (state != 0)) ? true : false;
}

void USART::clear_interrupt_flag(Interrupt_Flags interrupt_flag)
{
    using enum Interrupt_Flags;
    switch (interrupt_flag) {
    case INTR_FLAG_CTL0_PERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PERR), 0);
        break;
    case INTR_FLAG_CTL0_TBE:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TBE), 0);
        break;
    case INTR_FLAG_CTL0_TC:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TC), 0);
        break;
    case INTR_FLAG_CTL0_RBNE:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::RBNE), 0);
        break;
    case INTR_FLAG_CTL0_ORERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR), 0);
        break;
    case INTR_FLAG_CTL0_IDLEF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::IDLEF), 0);
        break;
    case INTR_FLAG_CTL1_LBDF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::LBDF), 0);
        break;
    case INTR_FLAG_CTL2_CTSF:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::CTSF), 0);
        break;
    case INTR_FLAG_CTL2_ORERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR), 0);
        break;
    case INTR_FLAG_CTL2_NERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::NERR), 0);
        break;
    case INTR_FLAG_CTL2_FERR:
        write_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::FERR), 0);
        break;
    case INTR_FLAG_CTL3_EBF:
        write_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::EBF), 0);
        break;
    case INTR_FLAG_CTL3_RTF:
        write_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::RTF), 0);
        break;
    default:
        break;
    }
}

} // namespace usart
