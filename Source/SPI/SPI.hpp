// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include "BitRW.hpp"
#include "RCU.hpp"
#include "spi_config.hpp"

#define MAX_SPIS  3

namespace spi {

class SPI {
public:
    static SPI& instance(SPI_Base Base) {
        static SPI instances[MAX_SPIS] = {
            SPI(SPI_Base::SPI0_BASE),
            SPI(SPI_Base::SPI1_BASE),
            SPI(SPI_Base::SPI2_BASE),
        };
        return instances[static_cast<int>(Base)];
    }

    // Initialize
    void init();
    void deinit();
    // Enable
    void enable();
    void disable();
    // NSS
    void nss_output_enable();
    void nss_output_disable();
    void nss_internal_high();
    void nss_internal_low();
    // DMA
    void dma_enable(DMA_Direction dma);
    void dma_disable(DMA_Direction dma);
    // Configuration
    void data_frame_format_config(Frame_Format frame_format);
    void bidirectional_transfer_config(Direction_Mode transfer_direction);
    // Data
    void data_transmit(uint16_t data);
    uint16_t data_receive();
    // CRC
    void crc_enable();
    void crc_disable();
    void set_crc_next();
    uint16_t get_crc(CRC_Direction crc);
    void clear_crc_error();
    void set_crc_polynomial(uint16_t crc_poly);
    uint16_t get_crc_polynomial();
    // NSSP
    void nssp_mode_enable();
    void nssp_mode_disable();
    // QUAD
    void quad_mode_enable();
    void quad_mode_disable();
    void quad_write_enable();
    void quad_read_enable();
    void quad_io23_output_enable();
    void quad_io23_output_disable();
    // Interrupts and flags
    bool get_flag(STAT_Bits flag);
    bool get_interrupt_flag(Interrupt_Flags interrupt);
    void interrupt_enable(Interrupt_Type interrupt);
    void interrupt_disable(Interrupt_Type interrupt);

    inline volatile uint32_t *reg_address(SPI_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    SPI_Clock_Config SPI_pclk_info_;

private:
    explicit SPI(SPI_Base Base) : SPI_pclk_info_(get_clock_config(Base)),
        base_address_(get_base_address(Base)) {}

    uint32_t base_address_;
    const SPI_Config config_;

    template<typename T>
    inline T read_register(SPI_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(SPI_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }

    uint32_t get_base_address(SPI_Base Base) {
        return SPI_baseAddress[static_cast<int>(Base)];
    }

    SPI_Clock_Config get_clock_config(SPI_Base Base) {
        return SPI_pclk_index[static_cast<int>(Base)];
    }
};

} // namespace spi

// Usage example:
// SPI& spi0 = SPI::instance(SPI_Base::SPI0_BASE);
