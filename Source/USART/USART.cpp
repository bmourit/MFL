// gd32f30x USART peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "USART.hpp"

// Initialize the static member
bool usart::USART::is_clock_enabled = false;

namespace usart {

//
// USART init() may only be called after a USART instance is created and a configuration passed.
// Calling init() will get the instance(s) of the required GPIO ports and initialize the pins
// based on the stored paramaters. This includes the pin number, mode, and speed.
// The rest of the paramaters are then use to finish the USART setup of
// baudrate, parity mode, word length, stop bits, and MSB mode (with HWFC, half-duplex to come).
// The USART instance configuration will also store parameters for the DMA mode.
// The DMA options are NONE, RX, TX, or DUAL, and they are used at operation time.
//
// Calling init() is the preferred and fastest way to set up a U(S)ART. Everything is done
// in a single function call, except the direction and baudrate calculation which are inlined to save
// function calling overhead. This is in constrast to calling 5+ different functions for
// the same task.
//
void USART::init() {
    // Set the rx/tx pin configuration
    auto result = gpio::GPIO::get_instance(config_.rx_pin_config.gpio_port);
    if (result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& port = result.value();
    // Initialize GPIO pin
    port.init_pin(config_.rx_pin_config.pin, config_.rx_pin_config.mode, config_.rx_pin_config.speed);
    // Check if the tx pin needs a different port
    if (config_.tx_pin_config.gpio_port != config_.rx_pin_config.gpio_port) {
        auto result1 = gpio::GPIO::get_instance(config_.rx_pin_config.gpio_port);
        if (result1.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& tx_port = result1.value();
        tx_port.init_pin(config_.tx_pin_config.pin, config_.tx_pin_config.mode, config_.tx_pin_config.speed);
    } else {
        // Same port
        port.init_pin(config_.tx_pin_config.pin, config_.tx_pin_config.mode, config_.tx_pin_config.speed);
    }

    // Some bits cannot be writing unless USART is disabled
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UEN), Clear);

    // Set USART configuration parameters
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WL), static_cast<uint32_t>(config_.word_length));
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PMEN), static_cast<uint32_t>(config_.parity));
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::STB), static_cast<uint32_t>(config_.stop_bits));
    write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::MSBF), static_cast<uint32_t>(config_.msbf));
    set_direction(config_.direction);
    set_baudrate(config_.baudrate);
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::UEN), Set);
}

void inline USART::set_baudrate(uint32_t baudrate)
{
    uint32_t freq = 0;

    // Must not be zero
    if (baudrate == 0) {
        return;
    }

    if (base_index_ == USART_Base::USART0_BASE) {
        freq = RCU_DEVICE.get_clock_frequency(rcu::Clock_Frequency::CK_APB2);
    } else {
        freq = RCU_DEVICE.get_clock_frequency(rcu::Clock_Frequency::CK_APB1);
    }

    // Simplified mantissa overflow fraction with 16bit oversampling
    uint32_t value = (freq + baudrate / 2) / baudrate;
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

inline void USART::set_direction(Direction_Mode direction)
{
    switch (direction) {
    case Direction_Mode::RX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), Set);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), Clear);
        break;
    case Direction_Mode::TX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), Clear);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), Set);
        break;
    case Direction_Mode::RXTX_MODE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), Set);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), Set);
        break;
    case Direction_Mode::RXTX_OFF:
    default:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::REN), Clear);
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TEN), Clear);
        break;
    }
}

void USART::set_msb(MSBF_Mode msbf)
{
    write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::MSBF), static_cast<uint32_t>(msbf));
}

void USART::rx_timeout_enable(bool enable)
{
    write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTEN), enable ? Set : Clear);
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
    return read_bit16(*this, USART_Regs::DATA, static_cast<uint32_t>(DATA_Bits::DATA));
}

void USART::set_address(uint8_t address)
{
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ADDR), static_cast<uint32_t>(address));
}

void USART::mute_mode_enable(bool enable)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RWU), enable ? Set : Clear);
}

void USART::configure_mute_mode_wakeup(Wakeup_Mode wakeup_mode)
{
    write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::WM), static_cast<uint32_t>(wakeup_mode));
}

void USART::set_halfduplex_enable(bool enable)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::HDEN), enable ? Set : Clear);
}

void USART::synchronous_clock_enable(bool enable)
{
    write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::CKEN), enable ? Set : Clear);
}

void USART::receive_data_dma(bool enable)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::DENR), enable ? Set : Clear);
}

void USART::send_data_dma(bool enable)
{
    write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::DENT), enable ? Set : Clear);
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

    return (value != 0);
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

void USART::interrupt_enable(Interrupt_Type type)
{
    using enum Interrupt_Type;
    switch (type) {
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

void USART::interrupt_disable(Interrupt_Type type)
{
    using enum Interrupt_Type;
    switch (type) {
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

// Enable or disable interrupt base on the set value
void USART::set_interrupt_enable(Interrupt_Type type, bool enable) {
    switch (type) {
    case Interrupt_Type::INTR_PERRIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PERRIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_TBEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TBEIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_TCIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TCIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_RBNEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_IDLEIE:
        write_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IDLEIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_LBDIE:
        write_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LBDIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_CTSIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::CTSIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_ERRIE:
        write_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_EBIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::EBIE), enable ? Set : Clear);
        break;
    case Interrupt_Type::INTR_RTIE:
        write_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTIE), enable ? Set : Clear);
        break;
    default:
        break;
    }
}

bool USART::get_interrupt_flag(Interrupt_Flags flag)
{
    uint32_t intr_flag = 0;
    uint32_t state = 0;

    using enum Interrupt_Flags;
    switch (flag) {
    case INTR_FLAG_CTL0_PERR:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PERR));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PERRIE));
        break;
    case INTR_FLAG_CTL0_TBE:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TBE));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TBEIE));
        break;
    case INTR_FLAG_CTL0_TC:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::TC));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::TCIE));
        break;
    case INTR_FLAG_CTL0_RBNE:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::RBNE));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE));
        break;
    case INTR_FLAG_CTL0_ORERR:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RBNEIE));
        break;
    case INTR_FLAG_CTL0_IDLEF:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::IDLEF));
        state = read_bit(*this, USART_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::IDLEIE));
        break;
    case INTR_FLAG_CTL1_LBDF:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::LBDF));
        state = read_bit(*this, USART_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LBDIE));
        break;
    case INTR_FLAG_CTL2_CTSF:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::CTSF));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::CTSIE));
        break;
    case INTR_FLAG_CTL2_ORERR:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ORERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL2_NERR:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::NERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL2_FERR:
        intr_flag = read_bit(*this, USART_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::FERR));
        state = read_bit(*this, USART_Regs::CTL2, static_cast<uint32_t>(CTL2_Bits::ERRIE));
        break;
    case INTR_FLAG_CTL3_EBF:
        intr_flag = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::EBF));
        state = read_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::EBIE));
        break;
    case INTR_FLAG_CTL3_RTF:
        intr_flag = read_bit(*this, USART_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::RTF));
        state = read_bit(*this, USART_Regs::CTL3, static_cast<uint32_t>(CTL3_Bits::RTIE));
        break;
    default:
        break;
    }

    return ((intr_flag != 0) && (state != 0));
}

void USART::clear_interrupt_flag(Interrupt_Flags flag)
{
    using enum Interrupt_Flags;
    switch (flag) {
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
