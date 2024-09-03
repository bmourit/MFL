// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "SPI.hpp"

// Initialize the static member
bool spi::SPI::is_clock_enabled = false;

namespace spi {

void SPI::init() {
    auto configure_pin = [](gpio::GPIO& port, const SPI_Pin_Config& pin_config) {
        port.init_pin(pin_config.pin, pin_config.mode, pin_config.speed);
    };

    auto mosi_result = gpio::GPIO::get_instance(config_.mosi_pin.gpio_port);
    if (mosi_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }

    gpio::GPIO& mosi_port = mosi_result.value();
    configure_pin(mosi_port, config_.mosi_pin);

    if (config_.miso_pin.gpio_port != config_.mosi_pin.gpio_port) {
        auto miso_result = gpio::GPIO::get_instance(config_.miso_pin.gpio_port);
        if (miso_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& miso_port = miso_result.value();
        configure_pin(miso_port, config_.miso_pin);
    } else {
        configure_pin(mosi_port, config_.miso_pin);
    }

    if (config_.sclk_pin.gpio_port == config_.mosi_pin.gpio_port) {
        configure_pin(mosi_port, config_.sclk_pin);
    } else {
        auto sclk_result = gpio::GPIO::get_instance(config_.sclk_pin.gpio_port);
        if (sclk_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& sclk_port = sclk_result.value();
        configure_pin(sclk_port, config_.sclk_pin);
    }

    if (config_.use_ssel_pin == true) {
        if (config_.ssel_pin.gpio_port == config_.mosi_pin.gpio_port) {
            configure_pin(mosi_port, config_.ssel_pin);
        } else {
            auto ssel_result = gpio::GPIO::get_instance(config_.ssel_pin.gpio_port);
            if (ssel_result.error() != gpio::GPIO_Error_Type::OK) {
                return;
            }
            gpio::GPIO& ssel_port = ssel_result.value();
            configure_pin(ssel_port, config_.ssel_pin);
        }
    }

    // Frame format
    if (config_.frame_format == Frame_Format::FF_16BIT) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::FF16), Set);
    }
    // Polarity
    if (config_.polarity_pull == Clock_Polarity::PULL_HIGH) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKPL), Set);
    }
    // Clock phase
    if (config_.clock_phase == Clock_Phase::PHASE_SECOND_EDGE) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKPH), Set);
    }
    // MSBF
    if (config_.msbf == Endian_Type::LSBF) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LF), Set);
    }
    // NSS
    if (config_.nss_type == NSS_Type::SOFTWARE_NSS) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSSEN), Set);
    }
    // TODO:
    // 	Check this needed for initialization.
    // 	Datasheet says it isn't, but equivilant
    // 	STM32 drivers do this. Keeping for now
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PSC), static_cast<uint32_t>(config_.pclk_divider));

    // Set SPI operational mode
    // Operational_Mode in config file
    switch (config_.operational_mode) {
    case Operational_Mode::MFD_MODE:
    case Operational_Mode::MTU_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), Set);
        break;
    case Operational_Mode::MRU_MODE:
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), Set,
                   static_cast<uint32_t>(CTL0_Bits::RO), Set);
        break;
    case Operational_Mode::MTB_MODE:
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), Set,
                   static_cast<uint32_t>(CTL0_Bits::BDEN), Set,
                   static_cast<uint32_t>(CTL0_Bits::BDOEN), Set);
        break;
    case Operational_Mode::MRB_MODE:
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), Set,
                   static_cast<uint32_t>(CTL0_Bits::BDEN), Set);
        break;
    case Operational_Mode::SRU_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RO), Set);
        break;
    case Operational_Mode::STB_MODE:
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), Set,
                   static_cast<uint32_t>(CTL0_Bits::BDOEN), Set);
        break;
    case Operational_Mode::SRB_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), Set);
        break;
    case Operational_Mode::SFD_MODE:
    case Operational_Mode::STU_MODE:
    default:
        break;
    }
}

void SPI::enable() {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPIEN), Set);
}

void SPI::disable() {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPIEN), Clear);
}

void SPI::set_nss_output_enable(bool enabled) {
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSDRV), enabled ? Set : Clear);
}

void SPI::nss_internal_high() {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSS), Set);
}

void SPI::nss_internal_low() {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSS), Clear);
}

void SPI::set_dma_enable(DMA_Direction dma, bool enabled) {
    if (dma == DMA_Direction::DMA_TX) {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMATEN), enabled ? Set : Clear);
    } else {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAREN), enabled ? Set : Clear);
    }
}

void SPI::data_frame_format_config(Frame_Format frame_format) {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::FF16), static_cast<uint32_t>(frame_format));
}

void SPI::data_transmit(uint16_t data) {
    write_register(SPI_Regs::DATA, static_cast<uint32_t>(data));
}

uint16_t SPI::data_receive() {
    uint32_t value = read_register<uint32_t>(SPI_Regs::DATA);
    return static_cast<uint16_t>(value);
}

void SPI::bidirectional_transfer_config(Direction_Mode transfer_direction) {
    if (transfer_direction == Direction_Mode::BIDIRECTIONAL_TRANSMIT) {
        // Set tx only mode
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), Set,
                   static_cast<uint32_t>(CTL0_Bits::BDEN), Set);
    } else {
        // Set rx only mode
        write_bits(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), Clear,
                   static_cast<uint32_t>(CTL0_Bits::BDEN), Set);
    }
}

void SPI::set_crc_polynomial(uint16_t crc_poly) {
    write_register(SPI_Regs::CRCPOLY, static_cast<uint32_t>(crc_poly));
}

uint16_t SPI::get_crc_polynomial() {
    uint32_t value = read_register<uint32_t>(SPI_Regs::CRCPOLY);
    return static_cast<uint16_t>(value);
}

void SPI::set_crc_enable(bool enabled) {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CRCEN), enabled ? Set : Clear);
}

void SPI::set_crc_next() {
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CRCNT), Set);
}

uint16_t SPI::get_crc(CRC_Direction crc) {
    uint32_t value = 0;
    if (crc == CRC_Direction::CRC_TX) {
        value = read_register<uint32_t>(SPI_Regs::TCRC);
        return static_cast<uint16_t>(value);
    } else {
        value = read_register<uint32_t>(SPI_Regs::RCRC);
        return static_cast<uint16_t>(value);
    }
}

void SPI::clear_crc_error() {
    write_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CRCERR), Clear);
}

void SPI::set_nssp_mode_enable(bool enabled) {
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSP), enabled ? Set : Clear);
}

void SPI::set_quad_mode_enable(bool enabled) {
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QMOD), enabled ? Set : Clear);
}

void SPI::quad_write_enable() {
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QRD), Clear);
}

void SPI::quad_read_enable() {
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QRD), Set);
}

void SPI::set_quad_io23_output_enable(bool enabled) {
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::IO23_DRV), enabled ? Set : Clear);
}

bool SPI::get_flag(Status_Flags flag) {
    return (read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(flag)) != Set);
}

bool SPI::get_interrupt_flag(Interrupt_Flags flag) {
    uint32_t stat = read_register<uint32_t>(SPI_Regs::STAT);
    uint32_t ctl = read_register<uint32_t>(SPI_Regs::CTL1);

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_TBE:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::TBE));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TBEIE));
        break;
    case Interrupt_Flags::INTR_FLAG_RBNE:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RBNE));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RBNEIE));
        break;
    case Interrupt_Flags::INTR_FLAG_RXORERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RXORERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_CONFERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CONFERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_CRCERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CRCERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_TXURERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::TXURERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_FER:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::FERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    default:
        break;
    }

    return (stat && ctl);
}

void SPI::set_interrupt_enable(Interrupt_Type type, bool enabled) {
    switch (type) {
    case Interrupt_Type::INTR_TBE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TBEIE), enabled ? Set : Clear);
        break;
    case Interrupt_Type::INTR_RBNE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RBNEIE), enabled ? Set : Clear);
        break;
    case Interrupt_Type::INTR_ERR:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), enabled ? Set : Clear);
        break;
    default:
        break;
    }
}

} // namespace spi
