// gd32f30x SDIO Card in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#if !defined(DISABLE_SDIO_CARD_DRIVER)

#include "DMA.hpp"
#include "SDIO_Card.hpp"

namespace sdio {

static SDIO_Error_Type get_r1_error_type(uint32_t response);

//
// Initialize card and put in standby state
//
SDIO_Error_Type Card::init() {
    sdio_.reset();

    SDIO_Error_Type result = begin_startup_procedure();
    if (result != SDIO_Error_Type::OK) return result;

    result = card_init();
    if (result != SDIO_Error_Type::OK) return result;

    // Set clocks for initialization
    sdio_.clock_configure(Clock_Edge::RISING_EDGE, false, false, Init_Clock_Divider);
    sdio_.set_bus_width(Bus_Width::WIDTH_1BIT);
    sdio_.set_hardware_clock_enable(false);

    return SDIO_Error_Type::OK;
}

//
// Startup command procedure according to SDIO specification
//
SDIO_Error_Type Card::begin_startup_procedure() {
    sdio_.set_power_mode(Power_Control::POWER_ON);

    // CMD0 (GO_IDLE_STATE)
    if (send_command_and_check(Command_Index::CMD0, 0, Command_Response::RSP_NONE, Wait_Type::WT_NONE, [this]() {
    return this->get_command_sent_result();
    }) != SDIO_Error_Type::OK) {
        return SDIO_Error_Type::CMD0_FAILED;
    }

    if (send_command_and_check(Command_Index::CMD8, Check_Pattern, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this]() {
    return this->get_r7_result();
    }) != SDIO_Error_Type::OK) {
        return handle_v1_card();
    }

    return validate_voltage();
}

//
// V1.1 specification handler
//
SDIO_Error_Type Card::handle_v1_card() {
    if (send_command_and_check(Command_Index::CMD0, 0, Command_Response::RSP_NONE, Wait_Type::WT_NONE, [this]() {
    return this->get_command_sent_result();
    }) != SDIO_Error_Type::OK) {
        return SDIO_Error_Type::CMD0_FAILED;
    }

    return send_command_and_check(Command_Index::CMD55, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD55]() {
        return get_r1_result(cmd);
    });
}

//
// Voltage validation
//
SDIO_Error_Type Card::validate_voltage() {
    uint32_t voltage_valid = 0;
    volatile uint32_t response = 0;

    for (uint32_t i = 0; i < Max_Voltage_Checks; ++i) {
        if (send_command_and_check(Command_Index::CMD55, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD55]() {
        return get_r1_result(cmd);
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::CMD55_FAILED;
        }

        if (send_command_and_check(Command_Index::ACMD41, Voltage_Window | SDCARD_HCS | Switch_1_8V_Capacity, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this]() {
        return this->get_r2_r3_result();
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::ACMD41_FAILED;
        }

        response = sdio_.get_response(Response_Type::RESPONSE0);
        if (response >> 31) {
            voltage_valid = 1;
            break;
        }
    }

    return voltage_valid ? SDIO_Error_Type::OK : SDIO_Error_Type::INVALID_VOLTAGE;
}

//
// Shutdown
//
void Card::begin_shutdown_procedure() {
    sdio_.set_power_mode(Power_Control::POWER_OFF);
}

//
// Initialize card
//
SDIO_Error_Type Card::card_init() {
    uint16_t r6_rca = 1;
    if (sdio_.get_power_mode() == static_cast<uint32_t>(Power_Control::POWER_OFF)) {
        return SDIO_Error_Type::INVALID_OPERATION;
    }

    if (card_type_ != Card_Type::SD_IO_CARD) {
        if (send_command_and_check(Command_Index::CMD2, 0, Command_Response::RSP_LONG, Wait_Type::WT_NONE, [this]() {
        return this->get_r2_r3_result();
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::CMD2_FAILED;
        }

        store_cid();
        if (send_command_and_check(Command_Index::CMD3, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD3, rca = &r6_rca]() {
        return get_r6_result(cmd, rca);
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::CMD3_FAILED;
        }

        sdcard_rca_ = r6_rca;
        if (send_command_and_check(Command_Index::CMD9, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_LONG, Wait_Type::WT_NONE, [this]() {
        return this->get_r2_r3_result();
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::CMD9_FAILED;
        }

        store_csd();
    }

    sdcard_class_ = (sdio_.get_response(Response_Type::RESPONSE1) >> 20);

    return select_deselect(sdcard_rca_);
}

SDIO_Error_Type Card::store_cid() {
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    // Get the CID
    result = get_r2_r3_result();
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // Store the CID register values
    sdcard_cid_[0] = sdio_.get_response(Response_Type::RESPONSE0);
    sdcard_cid_[1] = sdio_.get_response(Response_Type::RESPONSE1);
    sdcard_cid_[2] = sdio_.get_response(Response_Type::RESPONSE2);
    sdcard_cid_[3] = sdio_.get_response(Response_Type::RESPONSE3);

    return result;
}

SDIO_Error_Type Card::store_csd() {
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    // Get the CSD
    result = get_r2_r3_result();
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // Store the CSD register values
    sdcard_csd_[0] = sdio_.get_response(Response_Type::RESPONSE0);
    sdcard_csd_[1] = sdio_.get_response(Response_Type::RESPONSE1);
    sdcard_csd_[2] = sdio_.get_response(Response_Type::RESPONSE2);
    sdcard_csd_[3] = sdio_.get_response(Response_Type::RESPONSE3);

    return result;
}

SDIO_Error_Type Card::set_bus_width_config(Bus_Width width) {
    if (card_type_ == Card_Type::SD_MMC) return SDIO_Error_Type::UNSUPPORTED_FUNCTION;
    if (width == Bus_Width::WIDTH_8BIT) return SDIO_Error_Type::UNSUPPORTED_FUNCTION;

    SDIO_Error_Type result = set_interface_bus_width(width);
    if (result != SDIO_Error_Type::OK) return result;

    sdio_.clock_configure(Clock_Edge::RISING_EDGE, false, false, Regular_Clock_Divider);
    sdio_.set_bus_width(width);
    sdio_.set_hardware_clock_enable(false);

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::set_transfer_method(Transfer_Method method) {
    if (method != Transfer_Method::METHOD_DMA && method != Transfer_Method::METHOD_POLLING) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    transfer_method_ = method;
    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::send_command_and_check(Command_Index cmd, uint32_t arg, Command_Response resp, Wait_Type wait, std::function<SDIO_Error_Type()> check_result) {
    sdio_.set_command_config(cmd, arg, resp, wait);
    sdio_.send_command(true);
    return check_result();
}

SDIO_Error_Type Card::read_single_block(uint32_t *buf, uint32_t address, uint16_t size) {
    if (buf == nullptr) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint32_t *temp_buf = buf;
    Block_Size block_size = Block_Size::BYTES_1;
    volatile uint32_t timeout = 100000;

    transfer_error_ = SDIO_Error_Type::OK;
    transfer_end_ = 0;
    total_bytes_ = 0;

    // Clear DSM and disable DMA
    sdio_.data_configure(0, 0, Block_Size::BYTES_1);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(false);
    sdio_.set_dma_enable(false);

    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Handle SDHC
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        size = 512;
        address /= 512;
    }

    if ((size > 0) && (size <= 2048) && ((size & (size - 1)) == 0)) {
        block_size = get_data_block_size_index(size);

        // CMD16 (SET_BLOCKLEN)
        sdio_.set_command_config(Command_Index::CMD16, size, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        uint32_t cmd16_response = sdio_.get_response(Response_Type::RESPONSE0);
        result = get_r1_error_type(cmd16_response);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
    } else {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    stop_condition_ = 0;
    total_bytes_ = size;

    // Configure data transfer
    sdio_.data_configure(Data_Timeout, total_bytes_, block_size);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
    sdio_.set_data_state_machine_enable(true);

    // CMD17 (READ_SINGLE_BLOCK)
    sdio_.set_command_config(Command_Index::CMD17, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    uint32_t cmd17_response = sdio_.get_response(Response_Type::RESPONSE0);
    result = get_r1_error_type(cmd17_response);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    if (transfer_method_ == Transfer_Method::METHOD_POLLING) {
        // Polling transfer method
        while (!sdio_.any_flag_set({
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE })) {

            if (sdio_.get_flag(Status_Flags::FLAG_RFH)) {
                // Read FIFO
                for (uint32_t count = 0; count < FIFO_Half_Words; ++count) {
                    *(temp_buf++) = sdio_.read_data_word();
                }
            }
        }

        // Error handling
        if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR)) {
            sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
            return SDIO_Error_Type::DATA_CRC_ERROR;
        }
        if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT)) {
            sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
            return SDIO_Error_Type::DATA_TIMEOUT;
        }
        if (sdio_.get_flag(Status_Flags::FLAG_RXORE)) {
            sdio_.clear_flag(Clear_Flags::FLAG_RXOREC);
            return SDIO_Error_Type::RX_FIFO_OVERRUN;
        }
        if (sdio_.get_flag(Status_Flags::FLAG_STBITE)) {
            sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
            return SDIO_Error_Type::START_BIT_ERROR;
        }

        while (sdio_.get_flag(Status_Flags::FLAG_RXDTVAL)) {
            *(temp_buf++) = sdio_.read_data_word();
        }
        sdio_.clear_all_flags();

    } else if (transfer_method_ == Transfer_Method::METHOD_DMA) {
        // DMA transfer method
        sdio_.set_dma_enable(true);
        dma_receive_configure(buf, size);

        auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
        if (dma_result.error() != dma::DMA_Error_Type::OK) {
            return SDIO_Error_Type::DMA_INSTANCE_ERROR;
        }

        dma::DMA& dma_instance = dma_result.value();
        while (!dma_instance.get_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF)) {
            if (timeout == 0) {
                return SDIO_Error_Type::DATA_TIMEOUT;
            }
            timeout = timeout - 1;
        }


    } else {
        result = SDIO_Error_Type::INVALID_PARAMETER;
    }

    return result;
}

SDIO_Error_Type Card::read_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count) {
    if (buf == nullptr) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    transfer_error_ = SDIO_Error_Type::OK;
    transfer_end_ = 0;
    total_bytes_ = 0;
    Block_Size block_size = Block_Size::BYTES_1;
    uint32_t align = size & (size - 1);

    // Clear DSM
    sdio_.data_configure(0, 0, Block_Size::BYTES_1);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(false);
    sdio_.set_dma_enable(false);

    // Check if the card is locked
    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Handle SDHC
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        size = 512;
        address /= 512;
    }

    if ((size > 0) && (size <= 2048) && (align == 0)) {
        block_size = get_data_block_size_index(size);

        // CMD16 (SET_BLOCKLEN)
        sdio_.set_command_config(Command_Index::CMD16, static_cast<uint32_t>(size), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        SDIO_Error_Type result = get_r1_result(Command_Index::CMD16);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
    } else {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    if (count > 1) {
        if (count * size > Max_Data_Length) {
            return SDIO_Error_Type::INVALID_PARAMETER;
        }

        stop_condition_ = 1;
        total_bytes_ = count * size;

        sdio_.data_configure(Data_Timeout, total_bytes_, block_size);
        sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
        sdio_.set_data_state_machine_enable(true);

        // CMD18 (READ_MULTIPLE_BLOCK)
        sdio_.set_command_config(Command_Index::CMD18, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        SDIO_Error_Type result = get_r1_result(Command_Index::CMD18);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }

        if (transfer_method_ == Transfer_Method::METHOD_POLLING) {
            // Polling
            while (!sdio_.get_flag(Status_Flags::FLAG_DTEND)) {
                if (sdio_.get_flag(Status_Flags::FLAG_RFH)) {
                    for (uint32_t i = 0; i < FIFO_Half_Words; ++i) {
                        *buf++ = sdio_.read_data_word();
                    }
                }
                if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR)) {
                    sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
                    return SDIO_Error_Type::DATA_CRC_ERROR;
                }

                if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT)) {
                    sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
                    return SDIO_Error_Type::DATA_TIMEOUT;
                }

                if (sdio_.get_flag(Status_Flags::FLAG_RXORE)) {
                    sdio_.clear_flag(Clear_Flags::FLAG_RXOREC);
                    return SDIO_Error_Type::RX_FIFO_OVERRUN;
                }

                if (sdio_.get_flag(Status_Flags::FLAG_STBITE)) {
                    sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
                    return SDIO_Error_Type::START_BIT_ERROR;
                }
            }

            // Read remaining data
            while (sdio_.get_flag(Status_Flags::FLAG_RXDTVAL)) {
                *buf++ = sdio_.read_data_word();
            }

            if (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY || card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
                // CMD12 (STOP_TRANSMISSION)
                sdio_.set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
                sdio_.send_command(true);

                result = get_r1_result(Command_Index::CMD12);
                if (result != SDIO_Error_Type::OK) {
                    return result;
                }
            }

            sdio_.clear_all_flags();

        } else if (transfer_method_ == Transfer_Method::METHOD_DMA) {
            // DMA
            sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, true);
            sdio_.set_dma_enable(true);
            dma_receive_configure(buf, total_bytes_);

            uint32_t timeout = 100000;
            auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
            if (dma_result.error() != dma::DMA_Error_Type::OK) {
                return SDIO_Error_Type::DMA_INSTANCE_ERROR;
            }

            dma::DMA& dma_instance = dma_result.value();
            while (!dma_instance.get_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF)) {
                if (timeout == 0) {
                    return SDIO_Error_Type::DATA_TIMEOUT;
                }
                timeout = timeout - 1;
            }

            while (transfer_end_ == 0 && transfer_error_ == SDIO_Error_Type::OK) {
                // Wait for transfer completion or error
            }

            if (transfer_error_ != SDIO_Error_Type::OK) {
                return transfer_error_;
            }

        } else {
            return SDIO_Error_Type::INVALID_PARAMETER;
        }
    }

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::write_single_block(uint32_t *buf, uint32_t address, uint16_t size) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint8_t card_state = 0;
    uint32_t count = 0;
    uint32_t align = 0;
    Block_Size block_size = Block_Size::BYTES_1;
    uint32_t *temp_buf = buf;
    uint32_t transfer_bytes = 0;
    uint32_t remaining = 0;
    volatile uint32_t response = 0;
    volatile uint32_t timeout = 0;

    if (buf == nullptr) {
        result = SDIO_Error_Type::INVALID_PARAMETER;
        return result;
    }

    transfer_error_ = SDIO_Error_Type::OK;
    transfer_end_ = 0;
    total_bytes_ = 0;

    // Clear DSM
    sdio_.data_configure(0, 0, Block_Size::BYTES_1);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(false);
    sdio_.set_dma_enable(false);

    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        result = SDIO_Error_Type::LOCK_UNLOCK_FAILED;
        return result;
    }

    // Size is fixed at 512 bytes for SDHC card
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        size = 512;
        address /= 512;
    }

    align = size & (size - 1);
    if ((size > 0) && (size <= 2048) && (align == 0)) {
        block_size = get_data_block_size_index(size);

        // CMD16 (SET_BLOCKLEN)
        sdio_.set_command_config(Command_Index::CMD16, static_cast<uint32_t>(size), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD16);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
    } else {
        result = SDIO_Error_Type::INVALID_PARAMETER;
        return result;
    }

    // CMD13 (SEND_STATUS)
    sdio_.set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    response = sdio_.get_response(Response_Type::RESPONSE0);
    timeout = 100000;

    while (((response & static_cast<uint32_t>(R1_Status::READY_FOR_DATA)) == 0) && (timeout > 0)) {
        // continue to send CMD13 to polling the state of card until buffer empty or timeout
        timeout = timeout - 1;

        // CMD13 (SEND_STATUS)
        sdio_.set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD13);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
        response = sdio_.get_response(Response_Type::RESPONSE0);
    }
    if (timeout == 0) {
        return SDIO_Error_Type::ERROR;
    }

    // CMD24 (WRITE_BLOCK)
    sdio_.set_command_config(Command_Index::CMD24, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD24);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    stop_condition_ = 0;
    total_bytes_ = size;

    sdio_.data_configure(Data_Timeout, total_bytes_, block_size);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(true);

    if (transfer_method_ == Transfer_Method::METHOD_POLLING) {
        // Polling
        // Polling transfer method
        while (!sdio_.any_flag_set({
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE })) {

            if (sdio_.get_flag(Status_Flags::FLAG_TFH) != false) {
                if ((total_bytes_ - transfer_bytes) < FIFO_Half_Bytes) {
                    remaining = (total_bytes_ - transfer_bytes) / 4 + (((total_bytes_ - transfer_bytes) % 4 == 0) ? 0 : 1);
                    for (count = 0; count < remaining; count++) {
                        sdio_.write_data_word(*temp_buf);
                        ++temp_buf;
                        transfer_bytes += 4;
                    }
                } else {
                    for (count = 0; count < FIFO_Half_Words; count++) {
                        sdio_.write_data_word(*(temp_buf + count));
                    }
                    // 32 bytes (8 words) have been transferred
                    temp_buf += FIFO_Half_Words;
                    transfer_bytes += FIFO_Half_Bytes;
                }
            }
        }

        if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
            result = SDIO_Error_Type::DATA_CRC_ERROR;
            sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
            return result;
        } else if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
            result = SDIO_Error_Type::DATA_TIMEOUT;
            sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
            return result;
        } else if (sdio_.get_flag(Status_Flags::FLAG_TXURE) != false) {
            result = SDIO_Error_Type::TX_FIFO_UNDERRUN;
            sdio_.clear_flag(Clear_Flags::FLAG_TXUREC);
            return result;
        } else if (sdio_.get_flag(Status_Flags::FLAG_STBITE) != false) {
            result = SDIO_Error_Type::START_BIT_ERROR;
            sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
            return result;
        }
    } else if (transfer_method_ == Transfer_Method::METHOD_DMA) {
        // DMA
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, true);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, true);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, true);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, true);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, true);
        dma_transfer_configure(buf, size);
        sdio_.set_dma_enable(true);

        auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
        if (dma_result.error() != dma::DMA_Error_Type::OK) {
            return SDIO_Error_Type::DMA_INSTANCE_ERROR;
        }
        dma::DMA& dma_instance = dma_result.value();

        timeout = 100000;
        while ((dma_instance.get_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF) == false) && (timeout > 0)) {
            timeout = timeout - 1;
            if (timeout == 0) {
                return SDIO_Error_Type::ERROR;
            }
        }
        while ((transfer_end_ == 0) && (transfer_error_ == SDIO_Error_Type::OK)) {
        }

        if (transfer_error_ != SDIO_Error_Type::OK) {
            return transfer_error_;
        }
    } else {
        result = SDIO_Error_Type::INVALID_PARAMETER;
        return result;
    }

    sdio_.clear_all_flags();

    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == PROGRAMMING) || (card_state == RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}


SDIO_Error_Type Card::write_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count) {
    if (buf == nullptr) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint8_t card_state = 0;
    Block_Size block_size = Block_Size::BYTES_1;
    uint32_t *temp_buf = buf;
    uint32_t transfer_bytes = 0;
    uint32_t remaining = 0;
    volatile uint32_t timeout = 100000;
    total_bytes_ = 0;

    // Clear DSM and disable DMA
    sdio_.data_configure(0, 0, Block_Size::BYTES_1);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(false);
    sdio_.set_dma_enable(false);

    // Check if the card is locked
    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Adjust size for SDHC cards
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        size = 512;
        address /= 512;
    }

    // Validate and set block size
    if ((size > 0) && (size <= 2048) && ((size & (size - 1)) == 0)) {
        block_size = get_data_block_size_index(size);
        sdio_.set_command_config(Command_Index::CMD16, static_cast<uint32_t>(size), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD16);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
    } else {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    // CMD13
    sdio_.set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);


    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    if (count > 1) {
        if (count * size > Max_Data_Length) {
            return SDIO_Error_Type::INVALID_PARAMETER;
        }

        // CMD55
        if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) || (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY)) {
            sdio_.set_command_config(Command_Index::CMD55, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);

            result = get_r1_result(Command_Index::CMD55);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }

            // ACMD23
            sdio_.set_command_config(Command_Index::ACMD23, count, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);

            result = get_r1_result(Command_Index::ACMD23);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }
        }

        // CMD25 (WRITE_MULTIPLE_BLOCK)
        sdio_.set_command_config(Command_Index::CMD25, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD25);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }

        stop_condition_ = 1;
        total_bytes_ = count * size;

        sdio_.data_configure(Data_Timeout, total_bytes_, block_size);
        sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
        sdio_.set_data_state_machine_enable(true);

        if (transfer_method_ == Transfer_Method::METHOD_POLLING) {
            // Polling
            while (!sdio_.any_flag_set({
            Status_Flags::FLAG_DTCRCERR,
            Status_Flags::FLAG_DTTMOUT,
            Status_Flags::FLAG_RXORE,
            Status_Flags::FLAG_DTBLKEND,
            Status_Flags::FLAG_STBITE })) {

                if (sdio_.get_flag(Status_Flags::FLAG_TFH)) {
                    // Write data to the FIFO
                    if (total_bytes_ - transfer_bytes >= FIFO_Half_Bytes) {
                        for (uint32_t i = 0; i < FIFO_Half_Words; ++i) {
                            sdio_.write_data_word(*(temp_buf + i));
                        }
                        temp_buf += FIFO_Half_Words;
                        transfer_bytes += FIFO_Half_Bytes;
                    } else {
                        remaining = (total_bytes_ - transfer_bytes) / 4 + ((total_bytes_ - transfer_bytes) % 4 != 0);
                        for (uint32_t i = 0; i < remaining; ++i) {
                            sdio_.write_data_word(*temp_buf++);
                            transfer_bytes += 4;
                        }
                    }
                }
            }

            // Handle errors
            if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR)) {
                result = SDIO_Error_Type::DATA_CRC_ERROR;
                sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
            } else if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT)) {
                result = SDIO_Error_Type::DATA_TIMEOUT;
                sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
            } else if (sdio_.get_flag(Status_Flags::FLAG_TXURE)) {
                result = SDIO_Error_Type::TX_FIFO_UNDERRUN;
                sdio_.clear_flag(Clear_Flags::FLAG_TXUREC);
            } else if (sdio_.get_flag(Status_Flags::FLAG_STBITE)) {
                result = SDIO_Error_Type::START_BIT_ERROR;
                sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
            }

            if (result != SDIO_Error_Type::OK) {
                return result;
            }

            // CMD12 (STOP_TRANSMISSION)
            if (sdio_.get_flag(Status_Flags::FLAG_DTEND)) {
                if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) || (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY)) {
                    sdio_.set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
                    sdio_.send_command(true);

                    result = get_r1_result(Command_Index::CMD12);
                    if (result != SDIO_Error_Type::OK) {
                        return result;
                    }
                }
            }
            sdio_.clear_all_flags();

        } else if (transfer_method_ == Transfer_Method::METHOD_DMA) {
            // DMA
            sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, true);
            sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, true);
            sdio_.set_dma_enable(true);
            dma_transfer_configure(buf, total_bytes_);

            auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
            if (dma_result.error() != dma::DMA_Error_Type::OK) {
                return SDIO_Error_Type::DMA_INSTANCE_ERROR;
            }
            dma::DMA& dma_instance = dma_result.value();

            while ((dma_instance.get_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF)) && (timeout > 0)) {
                timeout = timeout - 1;
                if (timeout == 0) {
                    return SDIO_Error_Type::DATA_TIMEOUT;
                }
            }

            while ((transfer_end_ == 0) && (transfer_error_ == SDIO_Error_Type::OK)) {
                // Wait for the transfer to complete
            }

            if (transfer_error_ != SDIO_Error_Type::OK) {
                return transfer_error_;
            }

        } else {
            return SDIO_Error_Type::INVALID_PARAMETER;
        }
    }

    sdio_.clear_all_flags();

    // Check card state
    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == PROGRAMMING) || (card_state == RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}

SDIO_Error_Type Card::erase(uint32_t address_start, uint32_t address_end) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint32_t count = 0;
    uint32_t divider = 0;
    volatile uint32_t delay = 0;
    uint8_t card_state = 0;
    uint8_t temp_byte = 0;
    uint16_t classes = 0;

    // Card command classes CSD
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 24)) >> 24);
    classes = (uint16_t)((uint16_t)temp_byte << 4);
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 16)) >> 16);
    classes |= (uint16_t)((uint16_t)(temp_byte & 0xF0) >> 4);
    if (0 == (classes & (1 << static_cast<uint32_t>(Card_Command_Class::ERASE)))) {
        return SDIO_Error_Type::UNSUPPORTED_FUNCTION;
    }
    divider = read_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV));
    divider += (read_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV8)) * 256);
    divider += 2;
    delay = 120000 / divider;

    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        result = SDIO_Error_Type::LOCK_UNLOCK_FAILED;
        return result;
    }

    // Size is fixed at 512 bytes for SDHC
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        address_start /= 512;
        address_end /= 512;
    }

    if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) ||
            (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY)) {
        // CMD32 (ERASE_WR_BLK_START)
        sdio_.set_command_config(Command_Index::CMD32, address_start, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD32);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }

        // CMD33 (ERASE_WR_BLK_END)
        sdio_.set_command_config(Command_Index::CMD33, address_end, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD33);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
    }

    // CMD38 (ERASE)
    sdio_.set_command_config(Command_Index::CMD38, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD38);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // loop until the counter reaches the calculated time
    for (count = 0; count < delay; count++) {
    }

    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == PROGRAMMING) || (card_state == RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}

SDIO_Error_Type Card::handle_interrupts() {
    transfer_error_ = SDIO_Error_Type::OK;
    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTEND) != false) {
        if (stop_condition_ == 1) {
            transfer_error_ = stop_transfer();
        } else {
            transfer_error_ = SDIO_Error_Type::OK;
        }
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_DTENDC);
        // Disable all interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        transfer_end_ = 1;
        count_ = 0;
        return transfer_error_;
    }

    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTCRCERR) != false) {
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_DTCRCERRC);
        // Disable all interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::DATA_CRC_ERROR;
        return transfer_error_;
    }

    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTTMOUT) != false) {
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_DTTMOUTC);
        // Disable all interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::DATA_TIMEOUT;
        return transfer_error_;
    }

    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_STBITE) != false) {
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_STBITEC);
        // Disable all the interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::START_BIT_ERROR;
        return transfer_error_;
    }

    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_TXURE) != false) {
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_TXUREC);
        // Disable all interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::TX_FIFO_UNDERRUN;
        return transfer_error_;
    }

    if (sdio_.get_interrupt_flag(Interrupt_Flags::FLAG_INTR_RXORE) != false) {
        sdio_.clear_interrupt_flag(Clear_Flags::FLAG_RXOREC);
        // Disable all interrupts
        sdio_.set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_.set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::RX_FIFO_OVERRUN;
        return transfer_error_;
    }

    return transfer_error_;
}

SDIO_Error_Type Card::select_deselect(uint16_t rca) {
    // CMD7 (SELECT/DESELECT_CARD)
    sdio_.set_command_config(Command_Index::CMD7, static_cast<uint32_t>(rca << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    return get_r1_result(Command_Index::CMD7);
}


SDIO_Error_Type Card::get_card_status(uint32_t *status) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    if (status == nullptr) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    // CMD13 (SEND_STATUS)
    sdio_.set_command_config(Command_Index::CMD13, (sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    *status = sdio_.get_response(Response_Type::RESPONSE0);

    return result;
}

SDIO_Error_Type Card::get_sdcard_status(uint32_t *status) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint32_t count = 0;

    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // CMD16 (SET_BLOCKLEN)
    sdio_.set_command_config(Command_Index::CMD16, 64, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // CMD55 (APP_CMD)
    sdio_.set_command_config(Command_Index::CMD55, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD55);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    sdio_.data_configure(Data_Timeout, 64, Block_Size::BYTES_64);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
    sdio_.set_data_state_machine_enable(true);

    // ACMD13 (SD_STATUS)
    sdio_.set_command_config(Command_Index::ACMD13, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    /* error check */
    result = get_r1_result(Command_Index::ACMD13);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }
    while (!sdio_.any_flag_set({
    Status_Flags::FLAG_DTCRCERR,
    Status_Flags::FLAG_DTTMOUT,
    Status_Flags::FLAG_RXORE,
    Status_Flags::FLAG_DTBLKEND,
    Status_Flags::FLAG_STBITE })) {

        if (sdio_.get_flag(Status_Flags::FLAG_RFH) != false) {
            for (count = 0; count < FIFO_Half_Words; count++) {
                *(status + count) = sdio_.read_data_word();
            }
            status += FIFO_Half_Words;
        }
    }

    if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        result = SDIO_Error_Type::DATA_CRC_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        result = SDIO_Error_Type::DATA_TIMEOUT;
        sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_RXORE) != false) {
        result = SDIO_Error_Type::RX_FIFO_OVERRUN;
        sdio_.clear_flag(Clear_Flags::FLAG_RXOREC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_STBITE) != false) {
        result = SDIO_Error_Type::START_BIT_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
        return result;
    }
    while (sdio_.get_flag(Status_Flags::FLAG_RXDTVAL) != false) {
        *status = sdio_.read_data_word();
        ++status;
    }

    sdio_.clear_all_flags();

    status -= 16;
    for (count = 0; count < 16; count++) {
        status[count] = ((status[count] & 0xFF) << 24) | ((status[count] & (0xFF << 8)) << 8) |
                        ((status[count] & (0xFF << 16)) >> 8) | ((status[count] & (0xFF << 24)) >> 24);
    }

    return result;
}

SDIO_Error_Type Card::stop_transfer(void)
{
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    // CMD12 (STOP_TRANSMISSION)
    sdio_.set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD12);

    return result;
}

SDIO_Error_Type Card::set_lock_unlock(Lock_State state) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint8_t card_state = 0;
    uint32_t temp_byte = 0;
    uint32_t key1 = 0;
    uint32_t key2 = 0;
    uint32_t response = 0;
    volatile uint32_t timeout = 0;
    uint16_t classes = 0;

    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 24)) >> 24);
    classes = (uint16_t)((uint16_t)temp_byte << 4);
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 16)) >> 16);
    classes |= (uint16_t)((uint16_t)(temp_byte & 0xF0) >> 4);

    if (0 == (classes & (1 << static_cast<uint32_t>(Card_Command_Class::LOCK_CARD)))) {
        result = SDIO_Error_Type::UNSUPPORTED_FUNCTION;
        return result;
    }

    key1 = (PasswordKey1 | static_cast<uint32_t>(state));
    key2 = PasswordKey2;

    // clear DSM
    sdio_.data_configure(0, 0, Block_Size::BYTES_1);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(false);
    sdio_.set_dma_enable(false);

    // CMD16 (SET_BLOCKLEN)
    sdio_.set_command_config(Command_Index::CMD16, 8, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // CMD13 (SEND_STATUS)
    sdio_.set_command_config(Command_Index::CMD13, (sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    response = sdio_.get_response(Response_Type::RESPONSE0);
    timeout = 100000;
    while (((response & (1 << static_cast<uint32_t>(R1_Status::READY_FOR_DATA))) == 0) && (timeout > 0)) {
        // Continue to send CMD13 to poll the state of card until buffer is empty or timeout
        timeout = timeout - 1;
        // CMD13 (SEND_STATUS)
        sdio_.set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_.send_command(true);

        result = get_r1_result(Command_Index::CMD13);
        if (result != SDIO_Error_Type::OK) {
            return result;
        }
        response = sdio_.get_response(Response_Type::RESPONSE0);
    }
    if (timeout == 0) {
        return SDIO_Error_Type::ERROR;
    }

    // CMD42 (LOCK_UNLOCK)
    sdio_.set_command_config(Command_Index::CMD42, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD42);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    response = sdio_.get_response(Response_Type::RESPONSE0);

    sdio_.data_configure(Data_Timeout, 8, Block_Size::BYTES_8);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_.set_data_state_machine_enable(true);

    sdio_.write_data_word(key1);
    sdio_.write_data_word(key2);

    if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        result = SDIO_Error_Type::DATA_CRC_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        result = SDIO_Error_Type::DATA_TIMEOUT;
        sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_TXURE) != false) {
        result = SDIO_Error_Type::TX_FIFO_UNDERRUN;
        sdio_.clear_flag(Clear_Flags::FLAG_TXUREC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_STBITE) != false) {
        result = SDIO_Error_Type::START_BIT_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
        return result;
    }

    sdio_.clear_all_flags();

    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == PROGRAMMING) || (card_state == RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}

Transfer_State Card::get_transfer_state() {
    Transfer_State transfer_state = Transfer_State::IDLE;
    if ((sdio_.get_flag(Status_Flags::FLAG_TXRUN) != false) && (sdio_.get_flag(Status_Flags::FLAG_RXRUN) != false)) {
        transfer_state = Transfer_State::BUSY;
    }

    return transfer_state;
}

uint32_t Card::get_card_capacity() {
    uint8_t temp_byte = 0;
    uint8_t device_size_multiplier = 0;
    uint8_t read_block_length = 0;
    uint32_t capacity = 0;
    uint32_t device_size = 0;

    if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY)) {
        // calculate the device size
        temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 8)) >> 8);
        device_size |= (uint32_t)((uint32_t)(temp_byte & 0x03) << 10);
        temp_byte = (uint8_t)(sdcard_csd_[1] & 0xFF);
        device_size |= (uint32_t)((uint32_t)temp_byte << 2);
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 24)) >> 24);
        device_size |= (uint32_t)((uint32_t)(temp_byte & 0xC0) >> 6);

        // calculate the device size multiplier
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 16)) >> 16);
        device_size_multiplier = (temp_byte & 0x03) << 1;
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 8)) >> 8);
        device_size_multiplier |= (temp_byte & 0x80) >> 7;

        // calculate the read block length
        temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 16)) >> 16);
        read_block_length = temp_byte & 0x0F;

        // capacity = BLOCKNR * BLOCK_LEN, BLOCKNR = (C_SIZE + 1) * MULT, MULT = 2 ^ (C_SIZE_MULT + 2), BLOCK_LEN = 2 ^ READ_BL_LEN
        capacity = (device_size + 1) * (1 << (device_size_multiplier + 2));
        capacity *= (1 << read_block_length);

        // change the unit of capacity to KByte
        capacity /= 1024;
    } else if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        // Calculate the device size
        temp_byte = (uint8_t)(sdcard_csd_[1] & 0xFF);
        device_size = (uint32_t)((uint32_t)(temp_byte & 0x3F) << 16);
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 24)) >> 24);
        device_size |= (uint32_t)((uint32_t)temp_byte << 8);
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 16)) >> 16);
        device_size |= (uint32_t)temp_byte;

        // Capacity = (device_size + 1) * 512 KByte
        capacity = (device_size + 1) * 512;
    }

    return capacity;
}

SDIO_Error_Type Card::get_card_specific_data(Card_Info *info)
{
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint8_t temp_byte = 0;

    if (info == nullptr) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    // store RCA and card type
    info->type = card_type_;
    info->relative_address = sdcard_rca_;

    // CID byte 0
    temp_byte = (uint8_t)((sdcard_cid_[0] & (0xFF << 24)) >> 24);
    info->cid.manufacture_id = temp_byte;

    // CID byte 1
    temp_byte = (uint8_t)((sdcard_cid_[0] & (0xFF << 16)) >> 16);
    info->cid.oem_id = (uint16_t)((uint16_t)temp_byte << 8);

    // CID byte 2
    temp_byte = (uint8_t)((sdcard_cid_[0] & (0xFF << 8)) >> 8);
    info->cid.oem_id |= (uint16_t)temp_byte;

    // CID byte 3
    temp_byte = (uint8_t)(sdcard_cid_[0] & 0xFF);
    info->cid.name0 = (uint32_t)((uint32_t)temp_byte << 24);

    // CID byte 4
    temp_byte = (uint8_t)((sdcard_cid_[1] & (0xFF << 24)) >> 24);
    info->cid.name0 |= (uint32_t)((uint32_t)temp_byte << 16);

    // CID byte 5
    temp_byte = (uint8_t)((sdcard_cid_[1] & (0xFF << 16)) >> 16);
    info->cid.name0 |= (uint32_t)((uint32_t)temp_byte << 8);

    // CID byte 6
    temp_byte = (uint8_t)((sdcard_cid_[1] & (0xFF << 8)) >> 8);
    info->cid.name0 |= (uint32_t)(temp_byte);

    // CID byte 7
    temp_byte = (uint8_t)(sdcard_cid_[1] & 0xFF);
    info->cid.name1 = temp_byte;

    // CID byte 8
    temp_byte = (uint8_t)((sdcard_cid_[2] & (0xFF << 24)) >> 24);
    info->cid.revision = temp_byte;

    // CID byte 9
    temp_byte = (uint8_t)((sdcard_cid_[2] & (0xFF << 16)) >> 16);
    info->cid.serial_number = (uint32_t)((uint32_t)temp_byte << 24);

    // CID byte 10
    temp_byte = (uint8_t)((sdcard_cid_[2] & (0xFF << 8)) >> 8);
    info->cid.serial_number |= (uint32_t)((uint32_t)temp_byte << 16);

    // CID byte 11
    temp_byte = (uint8_t)(sdcard_cid_[2] & 0xFF);
    info->cid.serial_number |= (uint32_t)temp_byte;

    // CID byte 12
    temp_byte = (uint8_t)((sdcard_cid_[3] & (0xFF << 24)) >> 24);
    info->cid.serial_number |= (uint32_t)temp_byte;

    // CID byte 13
    temp_byte = (uint8_t)((sdcard_cid_[3] & (0xFF << 16)) >> 16);
    info->cid.manufacture_date = (uint16_t)((uint16_t)(temp_byte & 0x0F) << 8);

    // CID byte 14
    temp_byte = (uint8_t)((sdcard_cid_[3] & (0xFF << 8)) >> 8);
    info->cid.manufacture_date |= (uint16_t)temp_byte;

    // CID byte 15
    temp_byte = (uint8_t)(sdcard_cid_[3] & 0xFF);
    info->cid.checksum = (temp_byte & 0xFE) >> 1;

    // CSD byte 0
    temp_byte = (uint8_t)((sdcard_csd_[0] & (0xFF << 24)) >> 24);
    info->csd.csd_structure = (temp_byte & 0xC0) >> 6;

    // CSD byte 1
    temp_byte = (uint8_t)((sdcard_csd_[0] & (0xFF << 16)) >> 16);
    info->csd.read_access_time = temp_byte;

    // CSD byte 2
    temp_byte = (uint8_t)((sdcard_csd_[0] & (0xFF << 8)) >> 8);
    info->csd.read_access_clock_cycles = temp_byte;

    // CSD byte 3
    temp_byte = (uint8_t)(sdcard_csd_[0] & 0xFF);
    info->csd.transfer_speed = temp_byte;

    // CSD byte 4
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 24)) >> 24);
    info->csd.card_command_class = (uint16_t)((uint16_t)temp_byte << 4);

    // CSD byte 5
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 16)) >> 16);
    info->csd.card_command_class |= (uint16_t)((uint16_t)(temp_byte & 0xF0) >> 4);
    info->csd.read_block_length = temp_byte & 0x0F;

    // CSD byte 6
    temp_byte = (uint8_t)((sdcard_csd_[1] & (0xFF << 8)) >> 8);
    info->csd.read_partial_block = (temp_byte & 0x80) >> 7;
    info->csd.write_unaligned_block = (temp_byte & 0x40) >> 6;
    info->csd.read_unaligned_block = (temp_byte & 0x20) >> 5;
    info->csd.dsr_available = (temp_byte & 0x10) >> 4;

    if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY)) {
        // card is SDSC card CSD version 1.0
        info->csd.device_size = (uint32_t)((uint32_t)(temp_byte & 0x03) << 10);

        // CSD byte 7
        temp_byte = (uint8_t)(sdcard_csd_[1] & 0xFF);
        info->csd.device_size |= (uint32_t)((uint32_t)temp_byte << 2);

        // CSD byte 8
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 24)) >> 24);
        info->csd.device_size |= (uint32_t)((uint32_t)(temp_byte & 0xC0) >> 6);
        info->csd.min_vdd_read = (temp_byte & 0x38) >> 3;
        info->csd.max_vdd_read = temp_byte & 0x07;

        // CSD byte 9
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 16)) >> 16);
        info->csd.min_vdd_write = (temp_byte & 0xE0) >> 5;
        info->csd.max_vdd_write = (temp_byte & 0x1C) >> 2;
        info->csd.device_size_multiplier = (temp_byte & 0x03) << 1;

        // CSD byte 10
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 8)) >> 8);
        info->csd.device_size_multiplier |= (temp_byte & 0x80) >> 7;

        // Bblock size and capacity
        info->block_size = 1 << (info->csd.read_block_length);
        info->capacity = info->csd.device_size + 1;
        info->capacity *= (1 << (info->csd.device_size_multiplier + 2));
        info->capacity *= info->block_size;
    } else if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        // Card is SDHC card CSD version 2.0
        // CSD byte 7
        temp_byte = (uint8_t)(sdcard_csd_[1] & 0xFF);
        info->csd.device_size = (uint32_t)((uint32_t)(temp_byte & 0x3F) << 16);

        // CSD byte 8
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 24)) >> 24);
        info->csd.device_size |= (uint32_t)((uint32_t)temp_byte << 8);

        // CSD byte 9
        temp_byte = (uint8_t)((sdcard_csd_[2] & (0xFF << 16)) >> 16);
        info->csd.device_size |= (uint32_t)temp_byte;

        // Block size and capacity
        info->block_size = 512;
        info->capacity = (info->csd.device_size + 1) * 512 * 1024;
    }

    info->csd.erase_block_enable = (temp_byte & 0x40) >> 6;
    info->csd.sector_size = (temp_byte & 0x3F) << 1;

    // CSD byte 11 */
    temp_byte = (uint8_t)(sdcard_csd_[2] & 0xFF);
    info->csd.sector_size |= (temp_byte & 0x80) >> 7;
    info->csd.write_protect_group_size = (temp_byte & 0x7F);

    // CSD byte 12 */
    temp_byte = (uint8_t)((sdcard_csd_[3] & (0xFF << 24)) >> 24);
    info->csd.write_protect_group_enable = (temp_byte & 0x80) >> 7;
    info->csd.speed_factor = (temp_byte & 0x1C) >> 2;
    info->csd.write_block_length = (temp_byte & 0x03) << 2;

    // CSD byte 13 */
    temp_byte = (uint8_t)((sdcard_csd_[3] & (0xFF << 16)) >> 16);
    info->csd.write_block_length |= (temp_byte & 0xC0) >> 6;
    info->csd.write_partial_block = (temp_byte & 0x20) >> 5;

    // CSD byte 14 */
    temp_byte = (uint8_t)((sdcard_csd_[3] & (0xFF << 8)) >> 8);
    info->csd.file_format_group = (temp_byte & 0x80) >> 7;
    info->csd.copy_flag = (temp_byte & 0x40) >> 6;
    info->csd.permanent_write_protect = (temp_byte & 0x20) >> 5;
    info->csd.temporary_write_protect = (temp_byte & 0x10) >> 4;
    info->csd.file_format = (temp_byte & 0x0C) >> 2;

    // CSD byte 15 */
    temp_byte = (uint8_t)(sdcard_csd_[3] & 0xFF);
    info->csd.checksum = (temp_byte & 0xFE) >> 1;

    return result;
}

Block_Size Card::get_data_block_size_index(uint16_t size) {
    uint8_t index = 0;

    while (size != 1) {
        size >>= 1;
        ++index;
    }
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ), static_cast<uint32_t>(index));

    uint32_t result = read_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ));
    switch (result) {
    case 0:
        return Block_Size::BYTES_1;
    case 1:
        return Block_Size::BYTES_2;
    case 2:
        return Block_Size::BYTES_4;
    case 3:
        return Block_Size::BYTES_8;
    case 4:
        return Block_Size::BYTES_16;
    case 5:
        return Block_Size::BYTES_32;
    case 6:
        return Block_Size::BYTES_64;
    case 7:
        return Block_Size::BYTES_128;
    case 8:
        return Block_Size::BYTES_256;
    case 9:
        return Block_Size::BYTES_512;
    case 10:
        return Block_Size::BYTES_1024;
    case 11:
        return Block_Size::BYTES_2048;
    case 12:
        return Block_Size::BYTES_4096;
    case 13:
        return Block_Size::BYTES_8192;
    case 14:
        return Block_Size::BYTES_16384;
    default:
        return Block_Size::BYTES_1;
    }
}

SDIO_Error_Type Card::get_card_state(uint8_t *card_state) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    volatile uint32_t response = 0;

    // CMD13 (SEND_STATUS)
    sdio_.set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR) |
                      read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) |
                      read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))))) {
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR))) {
        result = SDIO_Error_Type::COMMAND_CRC_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_CCRCERRC);
        return result;
    } else if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT))) {
        result = SDIO_Error_Type::RESPONSE_TIMEOUT;
        sdio_.clear_flag(Clear_Flags::FLAG_CMDTMOUTC);
        return result;
    }

    if (sdio_.get_command_index() != static_cast<uint8_t>(Command_Index::CMD13)) {
        return SDIO_Error_Type::ILLEGAL_COMMAND;
    }

    sdio_.clear_all_flags();

    response = sdio_.get_response(Response_Type::RESPONSE0);
    *card_state = static_cast<uint8_t>((response >> 9) & CardStateMask);

    if ((response & All_R1_Error_Bits) == 0) {
        return SDIO_Error_Type::OK;
    }

    result = get_r1_error_type(response);

    return result;
}

SDIO_Error_Type Card::get_command_sent_result() {
    volatile uint32_t timeout = 100000;
    while ((sdio_.get_flag(Status_Flags::FLAG_CMDSEND) == false) && (timeout > 0)) {
        timeout = timeout - 1;
    }

    if (timeout == 0) {
        return SDIO_Error_Type::RESPONSE_TIMEOUT;
    }
    sdio_.clear_all_flags();

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::check_sdio_status(Command_Index index, bool check_index) {
    // Wait until one of the relevant flags is set
    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV)))) {
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR))) {
        sdio_.clear_flag(Clear_Flags::FLAG_CCRCERRC);
        return SDIO_Error_Type::COMMAND_CRC_ERROR;
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT))) {
        sdio_.clear_flag(Clear_Flags::FLAG_CMDTMOUTC);
        return SDIO_Error_Type::RESPONSE_TIMEOUT;
    }

    if (check_index && sdio_.get_command_index() != static_cast<uint8_t>(index)) {
        return SDIO_Error_Type::ILLEGAL_COMMAND;
    }

    // Clear all flags before returning
    sdio_.clear_all_flags();

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::get_r1_result(Command_Index index) {
    SDIO_Error_Type result = check_sdio_status(index, true);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // Get the R1 response and check for errors
    uint32_t r1_response = sdio_.get_response(Response_Type::RESPONSE0);
    if ((r1_response & All_R1_Error_Bits) == 0) {
        return SDIO_Error_Type::OK;
    }

    return get_r1_error_type(r1_response);
}

SDIO_Error_Type Card::get_r2_r3_result() {
    return check_sdio_status();
}

SDIO_Error_Type Card::get_r6_result(Command_Index index, uint16_t *rca) {
    SDIO_Error_Type result = check_sdio_status(index, true);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    volatile uint32_t response = sdio_.get_response(Response_Type::RESPONSE0);

    if ((response & (static_cast<uint32_t>(R6_Status::COM_CRC_ERROR) |
                     static_cast<uint32_t>(R6_Status::ILLEGAL_COMMAND) |
                     static_cast<uint32_t>(R6_Status::ERROR))) == 0) {
        *rca = static_cast<uint16_t>(response >> 16);
        return result;
    }

    if (response & static_cast<uint32_t>(R6_Status::COM_CRC_ERROR)) {
        result = SDIO_Error_Type::COMMAND_CRC_ERROR;
    } else if (static_cast<uint32_t>(R6_Status::ILLEGAL_COMMAND)) {
        result = SDIO_Error_Type::ILLEGAL_COMMAND;
    } else if (static_cast<uint32_t>(R6_Status::ERROR)) {
        result = SDIO_Error_Type::ERROR;
    }

    return result;
}

SDIO_Error_Type Card::get_r7_result() {
    volatile uint32_t timeout = 100000;

    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))) && (timeout > 0)) {
        timeout = timeout - 1;
    }

    if (timeout == 0) {
        return SDIO_Error_Type::RESPONSE_TIMEOUT;
    }

    SDIO_Error_Type result = check_sdio_status();
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    return SDIO_Error_Type::OK;
}

static SDIO_Error_Type get_r1_error_type(uint32_t response) {
    switch (response) {
    case static_cast<uint32_t>(R1_Status::OUT_OF_RANGE):
        return SDIO_Error_Type::COMMAND_OUT_OF_RANGE;
    case static_cast<uint32_t>(R1_Status::ADDRESS_ERROR):
        return SDIO_Error_Type::INVALID_ADDRESS;
    case static_cast<uint32_t>(R1_Status::BLOCK_LEN_ERROR):
        return SDIO_Error_Type::INVALID_BLOCK_LENGTH;
    case static_cast<uint32_t>(R1_Status::ERASE_SEQ_ERROR):
        return SDIO_Error_Type::ERASE_SEQUENCE_ERROR;
    case static_cast<uint32_t>(R1_Status::ERASE_PARAM):
        return SDIO_Error_Type::INVALID_ERASE_BLOCKS;
    case static_cast<uint32_t>(R1_Status::WP_VIOLATION):
        return SDIO_Error_Type::WRITE_PROTECT_VIOLATION;
    case static_cast<uint32_t>(R1_Status::LOCK_UNLOCK_FAILED):
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    case static_cast<uint32_t>(R1_Status::COM_CRC_ERROR):
        return SDIO_Error_Type::COMMAND_CRC_ERROR;
    case static_cast<uint32_t>(R1_Status::ILLEGAL_COMMAND):
        return SDIO_Error_Type::ILLEGAL_COMMAND;
    case static_cast<uint32_t>(R1_Status::CARD_ECC_FAILED):
        return SDIO_Error_Type::ECC_FAILED;
    case static_cast<uint32_t>(R1_Status::CC_ERROR):
        return SDIO_Error_Type::CARD_CONTROLLER_ERROR;
    case static_cast<uint32_t>(R1_Status::ERROR):
        return SDIO_Error_Type::ERROR;
    case static_cast<uint32_t>(R1_Status::CID_CSD_OVERWRITE):
        return SDIO_Error_Type::CSD_OVERWRITE;
    case static_cast<uint32_t>(R1_Status::WP_ERASE_SKIP):
        return SDIO_Error_Type::ERASE_SKIP;
    case static_cast<uint32_t>(R1_Status::CARD_ECC_DISABLED):
        return SDIO_Error_Type::ECC_DISABLED;
    case static_cast<uint32_t>(R1_Status::ERASE_RESET):
        return SDIO_Error_Type::ERASE_SEQUENCE_RESET;
    case static_cast<uint32_t>(R1_Status::AKE_SEQ_ERROR):
        return SDIO_Error_Type::AUTHENTICATION_ERROR;
    default:
        return SDIO_Error_Type::ERROR;
    }
}

SDIO_Error_Type Card::set_interface_bus_width(Bus_Width width)
{
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    if (sdio_.get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Get SCR
    result = get_scr(sdcard_rca_, sdcard_scr_);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    if (width == Bus_Width::WIDTH_1BIT) {
        if ((sdcard_scr_[1] & SCR_Width_1) != 0) {
            // CMD55 (APP_CMD)
            sdio_.set_command_config(Command_Index::CMD55, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);
            result = get_r1_result(Command_Index::CMD55);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }
            // ACMD6 (SET_BUS_WIDTH)
            sdio_.set_command_config(Command_Index::ACMD6, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);

            result = get_r1_result(Command_Index::ACMD6);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }
        } else {
            result = SDIO_Error_Type::INVALID_OPERATION;
        }
        return result;
    } else if (width == Bus_Width::WIDTH_4BIT) {
        if ((sdcard_scr_[1] & SCR_Width_4) != 0) {
            // CMD55 (APP_CMD)
            sdio_.set_command_config(Command_Index::CMD55, (sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);

            result = get_r1_result(Command_Index::CMD55);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }
            // ACMD6 (SET_BUS_WIDTH)
            sdio_.set_command_config(Command_Index::ACMD6, 2, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_.send_command(true);

            result = get_r1_result(Command_Index::ACMD6);
            if (result != SDIO_Error_Type::OK) {
                return result;
            }
        } else {
            result = SDIO_Error_Type::INVALID_OPERATION;
        }
        return result;
    } else {
        result = SDIO_Error_Type::INVALID_PARAMETER;

        return result;
    }
}

SDIO_Error_Type Card::get_scr(uint16_t rca, uint32_t *scr)
{
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    uint32_t temp_scr[2] = {0, 0};
    uint32_t index_scr = 0;

    // CMD16 (SET_BLOCKLEN)
    sdio_.set_command_config(Command_Index::CMD16, 8, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // CMD55 (APP_CMD)
    sdio_.set_command_config(Command_Index::CMD55, static_cast<uint32_t>(rca << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::CMD55);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    sdio_.data_configure(Data_Timeout, 8, Block_Size::BYTES_8);
    sdio_.data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
    sdio_.set_data_state_machine_enable(true);

    // ACMD51 (SEND_SCR)
    sdio_.set_command_config(Command_Index::ACMD51, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_.send_command(true);

    result = get_r1_result(Command_Index::ACMD51);
    if (result != SDIO_Error_Type::OK) {
        return result;
    }

    // Store SCR
    while (!sdio_.any_flag_set({
    Status_Flags::FLAG_DTCRCERR,
    Status_Flags::FLAG_DTTMOUT,
    Status_Flags::FLAG_RXORE,
    Status_Flags::FLAG_DTBLKEND,
    Status_Flags::FLAG_STBITE })) {

        if (sdio_.get_flag(Status_Flags::FLAG_RXDTVAL) != false) {
            *(temp_scr + index_scr) = sdio_.read_data_word();
            ++index_scr;
        }
    }

    if (sdio_.get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        result = SDIO_Error_Type::DATA_CRC_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        result = SDIO_Error_Type::DATA_TIMEOUT;
        sdio_.clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_RXORE) != false) {
        result = SDIO_Error_Type::RX_FIFO_OVERRUN;
        sdio_.clear_flag(Clear_Flags::FLAG_RXOREC);
        return result;
    } else if (sdio_.get_flag(Status_Flags::FLAG_STBITE) != false) {
        result = SDIO_Error_Type::START_BIT_ERROR;
        sdio_.clear_flag(Clear_Flags::FLAG_STBITEC);
        return result;
    }

    sdio_.clear_all_flags();

    // adjust SCR value
    *(scr) = ((temp_scr[1] & 0xFF) << 24) | ((temp_scr[1] & (0xFF << 8)) << 8) |
             ((temp_scr[1] & (0xFF << 16)) >> 8) | ((temp_scr[1] & (0xFF << 24)) >> 24);
    *(scr + 1) = ((temp_scr[0] & 0xFF) << 24) | ((temp_scr[0] & (0xFF << 8)) << 8) |
                 ((temp_scr[0] & (0xFF << 16)) >> 8) | ((temp_scr[0] & (0xFF << 24)) >> 24);

    return result;
}

//
// Transfer config for DMA is always DMA1 channel 3
//
void Card::dma_transfer_configure(uint32_t *buf, uint32_t size)
{
    auto result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
    if (result.error() != dma::DMA_Error_Type::OK) {
        return;
    }
    dma::DMA& dma_instance = result.value();

    dma::DMA_Config config = {};

    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_GIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_HTFIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_ERRIF);
    dma_instance.set_channel_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_instance.reset(dma::DMA_Channel::CHANNEL3);

    // DMA_Config transfer parameters
    config.peripheral_address = reinterpret_cast<uint32_t>(reg_address(SDIO_Regs::FIFO));
    config.peripheral_bit_width = dma::Bit_Width::WIDTH_32BIT;
    config.memory_address = reinterpret_cast<uint32_t>(buf);
    config.memory_bit_width = dma::Bit_Width::WIDTH_32BIT;
    config.count = size / 4;
    config.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    config.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    config.channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY;
    config.direction = dma::Transfer_Direction::M2P;

    dma_instance.configure(dma::DMA_Channel::CHANNEL3, &config);
    dma_instance.set_circulation_mode_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_instance.set_channel_enable(dma::DMA_Channel::CHANNEL3, true);
}

//
// Receive config for DMA is always DMA1 channel 3
//
void Card::dma_receive_configure(uint32_t *buf, uint32_t size)
{
    auto result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
    if (result.error() != dma::DMA_Error_Type::OK) {
        return;
    }
    dma::DMA& dma_instance = result.value();

    dma::DMA_Config config = {};

    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_GIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_HTFIF);
    dma_instance.clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_ERRIF);
    dma_instance.set_channel_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_instance.reset(dma::DMA_Channel::CHANNEL3);

    // DMA_Config receive parameters
    config.peripheral_address = reinterpret_cast<uint32_t>(reg_address(SDIO_Regs::FIFO));
    config.peripheral_bit_width = dma::Bit_Width::WIDTH_32BIT;
    config.memory_address = reinterpret_cast<uint32_t>(buf);
    config.memory_bit_width = dma::Bit_Width::WIDTH_32BIT;
    config.count = size / 4;
    config.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    config.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    config.channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY;
    config.direction = dma::Transfer_Direction::P2M;

    dma_instance.configure(dma::DMA_Channel::CHANNEL3, &config);
    dma_instance.set_circulation_mode_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_instance.set_channel_enable(dma::DMA_Channel::CHANNEL3, true);
}

} // namespace sdio

// Instantiate class of global device
sdio::Card SDCard_Handle;

#endif // DISABLE_SDIO_CARD_DRIVER
