// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "SPI.hpp"

namespace spi {

void SPI::init()
{
    // Enable SPI pclk
    RCU_DEVICE.set_pclk_enable(SPI_pclk_info_.clock_reg, true);

    // Configure frame format
    if (config_.frame_format == Frame_Format::FF_16BIT) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::FF16), 1);
    }
    // Set the polarity
    if (config_.polarity_pull == Clock_Polarity::PULL_HIGH) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKPL), 1);
    }
    // Set the phase
    if (config_.clock_phase == Clock_Phase::PHASE_SECOND_EDGE) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CKPH), 1);
    }
    // Set First bit MSB or LSB
    if (config_.endian == Endian_Type::LSBF) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LF), 1);
    }
    if (config_.nss_type == NSS_Type::SOFTWARE_NSS) {
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSSEN), 1);
    }
    // TODO:
    // 	Check this needed for initialization.
    // 	Datasheet says it isn't, but equivilant
    // 	STM32 drivers do this. Keeping for now
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PSC), static_cast<uint32_t>(config_.pclk_divider));

    // Set SPI operational mode
    // Operational_Mode in config file
    using enum Operational_Mode;
    switch (config_.operational_mode) {
    case MFD_MODE:
    case MTU_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), 1);
        break;
    case MRU_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RO), 1);
        break;
    case MTB_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), 1);
        break;
    case MRB_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MSTMOD), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
        break;
    case SRU_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::RO), 1);
        break;
    case STB_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), 1);
        break;
    case SRB_MODE:
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
        break;
    case SFD_MODE:
    case STU_MODE:
    default:
        break;
    }
}

void SPI::deinit()
{
    RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, true);
    RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, false);
}

void SPI::enable()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPIEN), 1);
}

void SPI::disable()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SPIEN), 0);
}

void SPI::nss_output_enable()
{
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSDRV), 1);
}

void SPI::nss_output_disable()
{
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSDRV), 0);
}

void SPI::nss_internal_high()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSS), 1);
}

void SPI::nss_internal_low()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::SWNSS), 0);
}

void SPI::dma_enable(DMA_Direction dma)
{
    if (dma == DMA_Direction::DMA_TX) {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMATEN), 1);
    } else {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAREN), 1);
    }
}

void SPI::dma_disable(DMA_Direction dma)
{
    if (dma == DMA_Direction::DMA_TX) {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMATEN), 0);
    } else {
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::DMAREN), 0);
    }
}

void SPI::data_frame_format_config(Frame_Format frame_format)
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::FF16), static_cast<uint32_t>(frame_format));
}

void SPI::data_transmit(uint16_t data)
{
    write_register(SPI_Regs::DATA, static_cast<uint32_t>(data));
}

uint16_t SPI::data_receive()
{
    uint32_t value = read_register<uint32_t>(SPI_Regs::DATA);
    return static_cast<uint16_t>(value);
}

void SPI::bidirectional_transfer_config(Direction_Mode transfer_direction)
{
    if (transfer_direction == Direction_Mode::BIDIRECTIONAL_TRANSMIT) {
        // Set tx only mode
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), 1);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
    } else {
        // Set rx only mode
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDOEN), 0);
        write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::BDEN), 1);
    }
}

void SPI::set_crc_polynomial(uint16_t crc_poly)
{
    write_register(SPI_Regs::CRCPOLY, static_cast<uint32_t>(crc_poly));
}

uint16_t SPI::get_crc_polynomial()
{
    uint32_t value = read_register<uint32_t>(SPI_Regs::CRCPOLY);
    return static_cast<uint16_t>(value);
}

void SPI::crc_enable()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CRCEN), 1);
}

void SPI::crc_disable()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CRCEN), 0);
}

void SPI::set_crc_next()
{
    write_bit(*this, SPI_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::CRCNT), 1);
}

uint16_t SPI::get_crc(CRC_Direction crc)
{
    uint32_t value = 0;
    if (crc == CRC_Direction::CRC_TX) {
        value = read_register<uint32_t>(SPI_Regs::TCRC);
        return static_cast<uint16_t>(value);
    } else {
        value = read_register<uint32_t>(SPI_Regs::RCRC);
        return static_cast<uint16_t>(value);
    }
}

void SPI::clear_crc_error()
{
    write_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CRCERR), 0);
}

void SPI::nssp_mode_enable()
{
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSP), 1);
}

void SPI::nssp_mode_disable()
{
    write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::NSSP), 0);
}

void SPI::quad_mode_enable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QMOD), 1);
}

void SPI::quad_mode_disable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QMOD), 0);
}

void SPI::quad_write_enable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QRD), 0);
}

void SPI::quad_read_enable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::QRD), 1);
}

void SPI::quad_io23_output_enable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::IO23_DRV), 1);
}

void SPI::quad_io23_output_disable()
{
    write_bit(*this, SPI_Regs::QCTL, static_cast<uint32_t>(QCTL_Bits::IO23_DRV), 0);
}

void SPI::interrupt_enable(Interrupt_Type interrupt)
{
    using enum Interrupt_Type;
    switch (interrupt) {
    case INTR_TBE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TBEIE), 1);
        break;
    case INTR_RBNE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RBNEIE), 1);
        break;
    case INTR_ERR:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 1);
        break;
    default:
        break;
    }
}

void SPI::interrupt_disable(Interrupt_Type interrupt)
{
    using enum Interrupt_Type;
    switch (interrupt) {
    case INTR_TBE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TBEIE), 0);
        break;
    case INTR_RBNE:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RBNEIE), 0);
        break;
    case INTR_ERR:
        write_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 0);
        break;
    default :
        break;
    }
}

bool SPI::get_interrupt_flag(Interrupt_Flags interrupt)
{
    uint32_t stat = read_register<uint32_t>(SPI_Regs::STAT);
    uint32_t ctl = read_register<uint32_t>(SPI_Regs::CTL1);

    using enum Interrupt_Flags;
    switch (interrupt) {
    case INTR_FLAG_TBE:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::TBE));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::TBEIE));
        break;
    case INTR_FLAG_RBNE:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RBNE));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::RBNEIE));
        break;
    case INTR_FLAG_RXORERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::RXORERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case INTR_FLAG_CONFERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CONFERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case INTR_FLAG_CRCERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::CRCERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case INTR_FLAG_TXURERR:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::TXURERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case INTR_FLAG_FER:
        stat = read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(STAT_Bits::FERR));
        ctl = read_bit(*this, SPI_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    default:
        break;
    }

    return (stat && ctl) ? true : false;
}

bool SPI::get_flag(STAT_Bits flag)
{
    return (read_bit(*this, SPI_Regs::STAT, static_cast<uint32_t>(flag)) != 0) ? true : false;
}

} // namespace spi
