// gd32f30x SPI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
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
    void configure(SPI_Config new_config) {
        config_ = new_config;
        init();
    }
    // Initialize pins
    void pin_config_init();
    void pins_configure(SPI_Pins pin_config) {
        pin_config_ = pin_config;
        pin_config_init();
    }
    // Enable
    void enable();
    void disable();
    // NSS
    void set_nss_output_enable(bool enabled);
    void nss_internal_high();
    void nss_internal_low();
    // DMA
    void set_dma_enable(DMA_Direction dma, bool enabled);
    // Configuration
    void data_frame_format_config(Frame_Format frame_format);
    void bidirectional_transfer_config(Direction_Mode transfer_direction);
    // Data
    void data_transmit(uint16_t data);
    uint16_t data_receive();
    // CRC
    void set_crc_enable(bool enabled);
    void set_crc_next();
    uint16_t get_crc(CRC_Direction crc);
    void clear_crc_error();
    void set_crc_polynomial(uint16_t crc_poly);
    uint16_t get_crc_polynomial();
    // NSSP
    void set_nssp_mode_enable(bool enabled);
    // QUAD
    void set_quad_mode_enable(bool enabled);
    void quad_write_enable();
    void quad_read_enable();
    void set_quad_io23_output_enable(bool enabled);
    // Interrupts and flags
    bool get_flag(Status_Flags flag);
    bool get_interrupt_flag(Interrupt_Flags flag);
    void set_interrupt_enable(Interrupt_Type type, bool enabled);

    inline volatile uint32_t *reg_address(SPI_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(base_address_ + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}

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

    SPI_Config default_config = {
        .operational_mode = Operational_Mode::SFD_MODE,
        .frame_format = Frame_Format::FF_8BIT,
        .nss_type = NSS_Type::HARDWARE_NSS,
        .pclk_divider = PCLK_Divider::PCLK_2,
        .msbf = Endian_Type::MSBF,
        .polarity_pull = Clock_Polarity::PULL_LOW,
        .clock_phase = Clock_Phase::PHASE_FIRST_EDGE,
    };

    SPI_Config config_ = default_config;
    SPI_Pins pin_config_;

    template <SPI_Base Base>
    static SPI& get_instance_for_base() {
        static SPI instance(Base);
        return instance;
    }
};

} // namespace spi
