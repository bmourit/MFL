// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "GPIO.hpp"
#include "spi_config.hpp"

namespace spi {

class SPI {
public:
    static Result<SPI, SPI_Error_Type> get_instance(SPI_Base Base) {
        switch (Base) {
        case SPI_Base::SPI0_BASE:
            return get_enum_instance<SPI_Base, SPI, SPI_Error_Type>(
                       Base, SPI_Base::SPI0_BASE, get_instance_for_base<SPI_Base::SPI0_BASE>()
                   );
        case SPI_Base::SPI1_BASE:
            return get_enum_instance<SPI_Base, SPI, SPI_Error_Type>(
                       Base, SPI_Base::SPI1_BASE, get_instance_for_base<SPI_Base::SPI1_BASE>()
                   );
        case SPI_Base::SPI2_BASE:
            return get_enum_instance<SPI_Base, SPI, SPI_Error_Type>(
                       Base, SPI_Base::SPI2_BASE, get_instance_for_base<SPI_Base::SPI2_BASE>()
                   );
        default:
            return RETURN_ERROR(SPI, SPI_Error_Type::INVALID_SPI);
        }
    }

    // Initialize
    void init();
    // Reset
    void reset() {
        RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, true);
        RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, false);
    }
    // Configure
    void configure(SPI_Config* config) {
        if (config) {
            config_ = *config;
        }
        init();
    }
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
    bool get_flag(Status_Flags flag);
    bool get_interrupt_flag(Interrupt_Flags interrupt);
    void interrupt_enable(Interrupt_Type interrupt);
    void interrupt_disable(Interrupt_Type interrupt);

    inline volatile uint32_t *reg_address(SPI_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

private:
    SPI(SPI_Base Base) : SPI_pclk_info_(SPI_pclk_index[static_cast<int>(Base)]),
        base_address_(SPI_baseAddress[static_cast<int>(Base)]) {
        if (!is_clock_enabled) {
            RCU_DEVICE.set_pclk_enable(SPI_pclk_info_.clock_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, true);
            RCU_DEVICE.set_pclk_reset_enable(SPI_pclk_info_.reset_reg, false);
            is_clock_enabled = true;
        }
    }

    SPI_Clock_Config SPI_pclk_info_;
    uint32_t base_address_;
    static bool is_clock_enabled;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

    SPI_Config default_config = {
        .operational_mode = Operational_Mode::SFD_MODE,
        .frame_format = Frame_Format::FF_8BIT,
        .nss_type = NSS_Type::HARDWARE_NSS,
        .pclk_divider = PCLK_Divider::PCLK_2,
        .msbf = Endian_Type::MSBF,
        .polarity_pull = Clock_Polarity::PULL_LOW,
        .clock_phase = Clock_Phase::PHASE_FIRST_EDGE,
    };
    SPI_Config& config_ = default_config;

#pragma GCC diagnostic pop

    template <SPI_Base Base>
    static SPI& get_instance_for_base() {
        static SPI instance(Base);
        return instance;
    }

    template<typename T>
    inline T read_register(SPI_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(SPI_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace spi
