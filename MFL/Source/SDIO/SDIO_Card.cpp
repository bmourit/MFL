// gd32f30x SDIO Card in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#if !defined(DISABLE_SDIO_CARD_DRIVER)

#include "SDIO_Card.hpp"
#include "DMA.hpp"

namespace sdio {

static SDIO_Error_Type get_r1_error_type(uint32_t response);

// Initialize card and put in standby state
SDIO_Error_Type Card::init() {
    sdio_->reset();

    SDIO_Error_Type result = begin_startup_procedure();
    if (result != SDIO_Error_Type::OK) return result;

    result = card_init();
    if (result != SDIO_Error_Type::OK) return result;

    // Set clocks for initialization
    sdio_->clock_configure(Clock_Edge::RISING_EDGE, false, false, Regular_Clock_Divider);
    sdio_->set_bus_width(Bus_Width::WIDTH_1BIT);
    sdio_->set_hardware_clock_enable(false);

    return SDIO_Error_Type::OK;
}

// Startup command procedure according to SDIO specification
SDIO_Error_Type Card::begin_startup_procedure() {
    sdio_->set_power_mode(Power_Control::POWER_ON);
    sdio_->set_clock_enable(true);
    sdio_->init();

    // CMD0 (GO_IDLE_STATE)
    if (send_command_and_check(Command_Index::CMD0, 0, Command_Response::RSP_NONE, Wait_Type::WT_NONE, [this]() {
    return this->get_command_sent_result();
    }) != SDIO_Error_Type::OK) {
        return SDIO_Error_Type::CMD0_FAILED;
    }

    if (send_command_and_check(Command_Index::CMD8, Check_Pattern, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this]() {
    return this->get_r7_result();
    }) != SDIO_Error_Type::OK) {
        card_type_ = Card_Type::SDCARD_STANDARD_CAPACITY;
        handle_v1_card();
    } else {
        card_type_ = Card_Type::SDCARD_HIGH_CAPACITY;
    }

    return validate_voltage();
}

// V1.1 specification handler
SDIO_Error_Type Card::handle_v1_card() {
    if (send_command_and_check(Command_Index::CMD55, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD55]() {
        return get_r1_result(cmd);
    }) != SDIO_Error_Type::OK) {
        return SDIO_Error_Type::CMD55_FAILED;
    }
    return SDIO_Error_Type::OK;
}

// Voltage validation
SDIO_Error_Type Card::validate_voltage() {
    if (send_command_and_check(Command_Index::CMD55, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD55]() {
        return get_r1_result(cmd);
    }) != SDIO_Error_Type::OK) {
        return SDIO_Error_Type::CMD55_FAILED;
    }

    for (uint32_t i = 0; i < Max_Voltage_Checks; ++i) {
        if (send_command_and_check(Command_Index::CMD55, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this, cmd = Command_Index::CMD55]() {
            return get_r1_result(cmd);
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::CMD55_FAILED;
        }

        if (send_command_and_check(Command_Index::ACMD41, Voltage_Window |
                (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) ? SDCARD_HCS : SDCARD_Standard,
                Command_Response::RSP_SHORT, Wait_Type::WT_NONE, [this]() {
            return this->get_r2_r3_result();
        }) != SDIO_Error_Type::OK) {
            return SDIO_Error_Type::ACMD41_FAILED;
        }

        volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);
        if (response >> 31U) {
            return SDIO_Error_Type::OK;
        }
    }

    return SDIO_Error_Type::INVALID_VOLTAGE;
}

// Shutdown
void Card::begin_shutdown_procedure() {
    sdio_->set_power_mode(Power_Control::POWER_OFF);
}

// Initialize card
SDIO_Error_Type Card::card_init() {
    uint16_t r6_rca = 1U;
    if (sdio_->get_power_mode() == static_cast<uint32_t>(Power_Control::POWER_OFF)) {
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
    sdcard_class_ = (sdio_->get_response(Response_Type::RESPONSE1) >> 20);

    return select_deselect(sdcard_rca_);
}

SDIO_Error_Type Card::store_cid() {
    // Get the CID
    SDIO_Error_Type result = get_r2_r3_result();
    if (result != SDIO_Error_Type::OK) { return result; }

    // Store the CID register values
    sdcard_cid_[0] = sdio_->get_response(Response_Type::RESPONSE0);
    sdcard_cid_[1] = sdio_->get_response(Response_Type::RESPONSE1);
    sdcard_cid_[2] = sdio_->get_response(Response_Type::RESPONSE2);
    sdcard_cid_[3] = sdio_->get_response(Response_Type::RESPONSE3);

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::store_csd() {
    // Get the CSD
    SDIO_Error_Type result = get_r2_r3_result();
    if (result != SDIO_Error_Type::OK) { return result; }

    // Store the CSD register values
    sdcard_csd_[0] = sdio_->get_response(Response_Type::RESPONSE0);
    sdcard_csd_[1] = sdio_->get_response(Response_Type::RESPONSE1);
    sdcard_csd_[2] = sdio_->get_response(Response_Type::RESPONSE2);
    sdcard_csd_[3] = sdio_->get_response(Response_Type::RESPONSE3);

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::set_bus_width_config(Bus_Width width) {
    if (card_type_ == Card_Type::SD_MMC) return SDIO_Error_Type::UNSUPPORTED_FUNCTION;
    if (width == Bus_Width::WIDTH_8BIT) return SDIO_Error_Type::UNSUPPORTED_FUNCTION;

    SDIO_Error_Type result = set_interface_bus_width(width);
    if (result != SDIO_Error_Type::OK) { return result; }

    sdio_->clock_configure(Clock_Edge::RISING_EDGE, false, false, Regular_Clock_Divider);
    sdio_->set_bus_width(width);
    sdio_->set_hardware_clock_enable(false);

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::set_transfer_method(Transfer_Method method) {
    if ((method != Transfer_Method::METHOD_DMA) && (method != Transfer_Method::METHOD_POLLING)) {
        return SDIO_Error_Type::INVALID_PARAMETER;
    }

    if (method == Transfer_Method::METHOD_DMA) {
        initializeDMA();
    }
    transfer_method_ = method;

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::setup_transfer(uint32_t address, uint16_t size, bool is_write, bool multi_block) {
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        size = 512;
        address /= 512;
    }

    if (is_write && (sdio_->get_response(Response_Type::RESPONSE0) & Card_Locked)) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Set block size
    sdio_->set_command_config(Command_Index::CMD16, size, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    SDIO_Error_Type result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) return result;

    // Configure data path
    Block_Size block_size = get_data_block_size_index(size);
    stop_condition_ = multi_block ? 1 : 0;
    sdio_->data_configure(Data_Timeout, total_bytes_, block_size);
    sdio_->data_transfer_configure(Transfer_Mode::BLOCK, is_write ? Transfer_Direction::SDIO_TO_CARD : 
                                          Transfer_Direction::CARD_TO_SDIO);
    sdio_->set_data_state_machine_enable(true);

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::read_single_block(uint32_t* buf, uint32_t address, uint16_t size) {
    if (!validate_transfer_params(buf, size)) return SDIO_Error_Type::INVALID_PARAMETER;

    // Clear DSM and disable DMA
    clear_dsm(Transfer_Direction::CARD_TO_SDIO);
    total_bytes_ = size;

    SDIO_Error_Type result = setup_transfer(address, size, false, false);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD17 (READ_SINGLE_BLOCK)
    sdio_->set_command_config(Command_Index::CMD17, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD17);
    if (result != SDIO_Error_Type::OK) return result;

    // Execute transfer based on method
    return (transfer_method_ == Transfer_Method::METHOD_POLLING) ?
            read_single_block_polling(buf) : read_write_dma(buf, size, false);
}

SDIO_Error_Type Card::read_blocks(uint32_t* buf, uint32_t address, uint16_t size, uint32_t count) {
    if (!validate_transfer_params(buf, size) || count * size > Max_Data_Length)
        return SDIO_Error_Type::INVALID_PARAMETER;

    clear_dsm(Transfer_Direction::CARD_TO_SDIO);
    total_bytes_ = count * size;

    SDIO_Error_Type result = setup_transfer(address, size, false, true);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD18 (READ_MULTIPLE_BLOCK) send read command
    sdio_->set_command_config(Command_Index::CMD18, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD18);
    if (result != SDIO_Error_Type::OK) return result;

    // Execute transfer based on method
    return (transfer_method_ == Transfer_Method::METHOD_POLLING) ?
            read_blocks_polling(buf) : read_write_dma(buf, total_bytes_, false);
}

SDIO_Error_Type Card::write_single_block(uint32_t* buf, uint32_t address, uint16_t size) {
    if (!validate_transfer_params(buf, size)) return SDIO_Error_Type::INVALID_PARAMETER;

    clear_dsm(Transfer_Direction::SDIO_TO_CARD);
    total_bytes_ = size;

    SDIO_Error_Type result = setup_transfer(address, size, true, false);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD13 (SEND_STATUS)
    sdio_->set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift),
                            Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) return result;

    // Wait for card ready
    result = wait_for_card_ready();
    if (result != SDIO_Error_Type::OK) return result;

    // CMD24 (WRITE_BLOCK) send write command
    sdio_->set_command_config(Command_Index::CMD24, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD24);
    if (result != SDIO_Error_Type::OK) return result;

    // Execute transfer based on method
    result = (transfer_method_ == Transfer_Method::METHOD_POLLING) ?
            write_single_block_polling(buf) : read_write_dma(buf, size, true);

    Card_State card_state;
    do {
        result = get_card_state(&card_state);
    } while (result == SDIO_Error_Type::OK &&
            (card_state == Card_State::PROGRAMMING || card_state == Card_State::RECEIVE_DATA));

    return result;
}

SDIO_Error_Type Card::write_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count) {
    if (!validate_transfer_params(buf, size) || count * size > Max_Data_Length)
        return SDIO_Error_Type::INVALID_PARAMETER;

    // Clear DSM
    clear_dsm(Transfer_Direction::SDIO_TO_CARD);
    total_bytes_ = count * size;

    SDIO_Error_Type result = setup_transfer(address, size, true, true);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD13
    sdio_->set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) return result;

    // Pre-erase blocks for SD cards
    // CMD55
    if (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY || card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        sdio_->set_command_config(Command_Index::CMD55, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift),
                                Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);
        if ((result = get_r1_result(Command_Index::CMD55)) != SDIO_Error_Type::OK) return result;

        // ACMD23
        sdio_->set_command_config(Command_Index::ACMD23, count, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);
        if ((result = get_r1_result(Command_Index::ACMD23)) != SDIO_Error_Type::OK) return result;
    }

    // CMD25 (WRITE_MULTIPLE_BLOCK)
    sdio_->set_command_config(Command_Index::CMD25, address, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD25);
    if (result != SDIO_Error_Type::OK) return result;

    // Execute transfer based on method
    result = (transfer_method_ == Transfer_Method::METHOD_POLLING) ? 
                write_blocks_polling(buf) : read_write_dma(buf, total_bytes_, true);

    // Check card state
    Card_State card_state;
    do {
        result = get_card_state(&card_state);
    } while ((result == SDIO_Error_Type::OK) &&
            (card_state == Card_State::PROGRAMMING || card_state == Card_State::RECEIVE_DATA));

    return result;
}

SDIO_Error_Type Card::read_single_block_polling(uint32_t* buf) {
    Status_Flags flags[] = {
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE
    };

    while (!sdio_->any_flag_set(flags, sizeof(flags) / sizeof(Status_Flags))) {
        if (sdio_->get_flag(Status_Flags::FLAG_RFH)) {
            for (uint32_t count = 0; count < FIFO_Half_Words; ++count) {
                *(buf++) = sdio_->read_data_word();
            }
        }
    }

    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR)) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return SDIO_Error_Type::DATA_CRC_ERROR;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT)) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return SDIO_Error_Type::DATA_TIMEOUT;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_RXORE)) {
        sdio_->clear_flag(Clear_Flags::FLAG_RXOREC);
        return SDIO_Error_Type::RX_FIFO_OVERRUN;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_STBITE)) {
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
        return SDIO_Error_Type::START_BIT_ERROR;
    }

    while (sdio_->get_flag(Status_Flags::FLAG_RXDTVAL)) {
        *(buf++) = sdio_->read_data_word();
    }

    sdio_->clear_all_flags();
    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::write_single_block_polling(uint32_t* buf) {
    uint32_t transfer_bytes = 0;

    Status_Flags flags[] = {
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE
    };

    while (!sdio_->any_flag_set(flags, sizeof(flags) / sizeof(Status_Flags))) {
        if (sdio_->get_flag(Status_Flags::FLAG_TFH)) {
            if ((total_bytes_ - transfer_bytes) < FIFO_Half_Bytes) {
                uint32_t remaining = (total_bytes_ - transfer_bytes) / 4 + 
                           (((total_bytes_ - transfer_bytes) % 4 == 0) ? Clear : Set);
                for (uint32_t count = 0; count < remaining; count++) {
                    sdio_->write_data_word(*buf++);
                    transfer_bytes += 4;
                }
            } else {
                for (uint32_t count = 0; count < FIFO_Half_Words; count++) {
                    sdio_->write_data_word(*(buf + count));
                }
                buf += FIFO_Half_Words;
                transfer_bytes += FIFO_Half_Bytes;
            }
        }
    }

    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR)) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return SDIO_Error_Type::DATA_CRC_ERROR;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT)) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return SDIO_Error_Type::DATA_TIMEOUT;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_TXURE)) {
        sdio_->clear_flag(Clear_Flags::FLAG_TXUREC);
        return SDIO_Error_Type::TX_FIFO_UNDERRUN;
    }
    if (sdio_->get_flag(Status_Flags::FLAG_STBITE)) {
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
        return SDIO_Error_Type::START_BIT_ERROR;
    }

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::read_blocks_polling(uint32_t* buf) {
    while (!sdio_->get_flag(Status_Flags::FLAG_DTEND)) {
        if (sdio_->get_flag(Status_Flags::FLAG_RFH)) {
            for (uint32_t i = 0; i < FIFO_Half_Words; ++i) {
                *buf++ = sdio_->read_data_word();
            }
        }

        if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR)) {
            sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
            return SDIO_Error_Type::DATA_CRC_ERROR;
        }
        if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT)) {
            sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
            return SDIO_Error_Type::DATA_TIMEOUT;
        }
        if (sdio_->get_flag(Status_Flags::FLAG_RXORE)) {
            sdio_->clear_flag(Clear_Flags::FLAG_RXOREC);
            return SDIO_Error_Type::RX_FIFO_OVERRUN;
        }
        if (sdio_->get_flag(Status_Flags::FLAG_STBITE)) {
            sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
            return SDIO_Error_Type::START_BIT_ERROR;
        }
    }

    // Read remaining data
    while (sdio_->get_flag(Status_Flags::FLAG_RXDTVAL)) {
        *buf++ = sdio_->read_data_word();
    }

    if (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY ||
        card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        // CMD12 (STOP_TRANSMISSION)
        sdio_->set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);
        return get_r1_result(Command_Index::CMD12);
    }

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::write_blocks_polling(uint32_t* buf) {
    uint32_t transfer_bytes = 0;

    Status_Flags flags[] = {
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE
    };

    while (!sdio_->any_flag_set(flags, sizeof(flags) / sizeof(Status_Flags))) {
        if (sdio_->get_flag(Status_Flags::FLAG_TFH)) {
            if (total_bytes_ - transfer_bytes >= FIFO_Half_Bytes) {
                for (uint32_t i = 0; i < FIFO_Half_Words; ++i) {
                    sdio_->write_data_word(*(buf + i));
                }
                buf += FIFO_Half_Words;
                transfer_bytes += FIFO_Half_Bytes;
            } else {
                uint32_t remaining = (total_bytes_ - transfer_bytes) / 4 + ((total_bytes_ - transfer_bytes) % 4 != 0);
                for (uint32_t i = 0; i < remaining; ++i) {
                    sdio_->write_data_word(*buf++);
                    transfer_bytes += 4;
                }
            }
        }
    }

    // Handle errors
    SDIO_Error_Type result = SDIO_Error_Type::OK;
    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR)) {
        result = SDIO_Error_Type::DATA_CRC_ERROR;
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
    } else if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT)) {
        result = SDIO_Error_Type::DATA_TIMEOUT;
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
    } else if (sdio_->get_flag(Status_Flags::FLAG_TXURE)) {
        result = SDIO_Error_Type::TX_FIFO_UNDERRUN;
        sdio_->clear_flag(Clear_Flags::FLAG_TXUREC);
    } else if (sdio_->get_flag(Status_Flags::FLAG_STBITE)) {
        result = SDIO_Error_Type::START_BIT_ERROR;
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
    }
    if (result != SDIO_Error_Type::OK) return result;

    // CMD12 (STOP_TRANSMISSION) send stop command for SD cards
    if (sdio_->get_flag(Status_Flags::FLAG_DTEND)) {
        if (stop_condition_ && (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY ||
                                card_type_ == Card_Type::SDCARD_HIGH_CAPACITY)) {
            sdio_->set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
            sdio_->send_command(true);
            return get_r1_result(Command_Index::CMD12);
        }
    }

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::read_write_dma(uint32_t* buf, uint32_t size, bool is_write) {
    sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, true);
    sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, true);
    sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, true);
    sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, true);
    sdio_->set_interrupt_enable(is_write ? Interrupt_Type::TXUREIE : Interrupt_Type::RXOREIE, true);

    // Configure DMA
    dma_configure(buf, size, is_write);
    sdio_->set_dma_enable(true);

    // Wait for DMA completion
    uint32_t timeout = 100000;
    while (!dma_->get_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF)) {
        if (timeout-- == 0) return SDIO_Error_Type::DATA_TIMEOUT;
    }

    while (transfer_end_ == 0 && transfer_error_ == SDIO_Error_Type::OK) {
        // Wait for transfer completion or error
    }

    return transfer_error_;
}

SDIO_Error_Type Card::erase(uint32_t address_start, uint32_t address_end) {
    SDIO_Error_Type result = SDIO_Error_Type::OK;

    // Card command classes CSD
    uint8_t temp_byte = static_cast<uint8_t>((sdcard_csd_[1] & (0xFFU << 24)) >> 24U);
    uint16_t classes = static_cast<uint16_t>(temp_byte << 4);
    temp_byte = static_cast<uint8_t>((sdcard_csd_[1] & (0xFFU << 16)) >> 16U);
    classes |= static_cast<uint16_t>((temp_byte & 0xF0) >> 4U);

    if ((classes & (1 << static_cast<uint32_t>(Card_Command_Class::ERASE))) == 0) {
        return SDIO_Error_Type::UNSUPPORTED_FUNCTION;
    }

    uint32_t divider = read_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV));
    divider += (read_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV8)) * 256);
    divider += 2;
    uint32_t delay = 120000 / divider;

    if (sdio_->get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Size is fixed at 512 bytes for SDHC
    if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        address_start /= 512;
        address_end /= 512;
    }

    if ((card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) || (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY)) {
        // CMD32 (ERASE_WR_BLK_START)
        sdio_->set_command_config(Command_Index::CMD32, address_start, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);

        result = get_r1_result(Command_Index::CMD32);
        if (result != SDIO_Error_Type::OK) return result;

        // CMD33 (ERASE_WR_BLK_END)
        sdio_->set_command_config(Command_Index::CMD33, address_end, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);

        result = get_r1_result(Command_Index::CMD33);
        if (result != SDIO_Error_Type::OK) return result;
    }

    // CMD38 (ERASE)
    sdio_->set_command_config(Command_Index::CMD38, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD38);
    if (result != SDIO_Error_Type::OK) return result;

    // loop until the counter reaches the calculated time
    for (uint32_t count = 0; count < delay; count++) {
    }

    Card_State card_state;
    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == Card_State::PROGRAMMING) || (card_state == Card_State::RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}

SDIO_Error_Type Card::handle_interrupts() {
    transfer_error_ = SDIO_Error_Type::OK;
    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTEND) != false) {
        if (stop_condition_ == 1) {
            transfer_error_ = stop_transfer();
        } else {
            transfer_error_ = SDIO_Error_Type::OK;
        }
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_DTENDC);
        // Disable all interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        transfer_end_ = 1;
        count_ = 0;
        return transfer_error_;
    }

    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTCRCERR) != false) {
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_DTCRCERRC);
        // Disable all interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::DATA_CRC_ERROR;
        return transfer_error_;
    }

    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_DTTMOUT) != false) {
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_DTTMOUTC);
        // Disable all interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::DATA_TIMEOUT;
        return transfer_error_;
    }

    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_STBITE) != false) {
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_STBITEC);
        // Disable all the interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::START_BIT_ERROR;
        return transfer_error_;
    }

    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_TXURE) != false) {
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_TXUREC);
        // Disable all interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::TX_FIFO_UNDERRUN;
        return transfer_error_;
    }

    if (sdio_->get_interrupt_flag(Interrupt_Flags::FLAG_INTR_RXORE) != false) {
        sdio_->clear_interrupt_flag(Clear_Flags::FLAG_RXOREC);
        // Disable all interrupts
        sdio_->set_interrupt_enable(Interrupt_Type::DTCRCERRIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTTMOUTIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::DTENDIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::STBITEIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RFHIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::TXUREIE, false);
        sdio_->set_interrupt_enable(Interrupt_Type::RXOREIE, false);
        count_ = 0;
        transfer_error_ = SDIO_Error_Type::RX_FIFO_OVERRUN;
        return transfer_error_;
    }

    return transfer_error_;
}

SDIO_Error_Type Card::select_deselect(uint16_t rca) {
    // CMD7 (SELECT/DESELECT_CARD)
    sdio_->set_command_config(Command_Index::CMD7, static_cast<uint32_t>(rca << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    return get_r1_result(Command_Index::CMD7);
}

SDIO_Error_Type Card::get_card_status(uint32_t *status) {
    if (status == nullptr) return SDIO_Error_Type::INVALID_PARAMETER;

    // CMD13 (SEND_STATUS)
    sdio_->set_command_config(Command_Index::CMD13, (sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    SDIO_Error_Type result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) return result;

    *status = sdio_->get_response(Response_Type::RESPONSE0);

    return result;
}

SDIO_Error_Type Card::get_sdcard_status(uint32_t* status) {
    uint32_t count = 0;

    if (sdio_->get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // CMD16 (SET_BLOCKLEN)
    sdio_->set_command_config(Command_Index::CMD16, 64, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    SDIO_Error_Type result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD55 (APP_CMD)
    sdio_->set_command_config(Command_Index::CMD55, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD55);
    if (result != SDIO_Error_Type::OK) return result;

    sdio_->data_configure(Data_Timeout, 64, Block_Size::BYTES_64);
    sdio_->data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
    sdio_->set_data_state_machine_enable(true);

    // ACMD13 (SD_STATUS)
    sdio_->set_command_config(Command_Index::ACMD13, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::ACMD13);
    if (result != SDIO_Error_Type::OK) return result;

    Status_Flags flags[] = {
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE
    };

    while (!sdio_->any_flag_set(flags, sizeof(flags) / sizeof(Status_Flags))) {
        if (sdio_->get_flag(Status_Flags::FLAG_RFH) != false) {
            for (count = 0; count < FIFO_Half_Words; count++) {
                *(status + count) = sdio_->read_data_word();
            }
            status += FIFO_Half_Words;
        }
    }

    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        result = SDIO_Error_Type::DATA_CRC_ERROR;
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return result;
    } else if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        result = SDIO_Error_Type::DATA_TIMEOUT;
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return result;
    } else if (sdio_->get_flag(Status_Flags::FLAG_RXORE) != false) {
        result = SDIO_Error_Type::RX_FIFO_OVERRUN;
        sdio_->clear_flag(Clear_Flags::FLAG_RXOREC);
        return result;
    } else if (sdio_->get_flag(Status_Flags::FLAG_STBITE) != false) {
        result = SDIO_Error_Type::START_BIT_ERROR;
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
        return result;
    }
    while (sdio_->get_flag(Status_Flags::FLAG_RXDTVAL) != false) {
        *status = sdio_->read_data_word();
        ++status;
    }

    sdio_->clear_all_flags();

    status -= 16U;
    for (count = 0; count < 16U; count++) {
        status[count] = ((status[count] & 0xFFU) << 24) |
                        ((status[count] & (0xFFU << 8)) << 8) |
                        ((status[count] & (0xFFU << 16)) >> 8U) |
                        ((status[count] & (0xFFU << 24)) >> 24U);
    }

    return result;
}

SDIO_Error_Type Card::stop_transfer(void) {
    // CMD12 (STOP_TRANSMISSION)
    sdio_->set_command_config(Command_Index::CMD12, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    return get_r1_result(Command_Index::CMD12);
}

SDIO_Error_Type Card::set_lock_unlock(Lock_State state) {
    uint32_t temp_byte = static_cast<uint8_t>((sdcard_csd_[1] & (0xFFU << 24)) >> 24U);
    uint16_t classes = static_cast<uint16_t>(temp_byte << 4);
    temp_byte = static_cast<uint8_t>((sdcard_csd_[1] & (0xFFU << 16)) >> 16U);
    classes |= static_cast<uint16_t>((temp_byte & 0xF0U) >> 4U);

    if ((classes & (1 << static_cast<uint32_t>(Card_Command_Class::LOCK_CARD))) == 0) {
        return SDIO_Error_Type::UNSUPPORTED_FUNCTION;
    }

    uint32_t key1 = (PasswordKey1 | static_cast<uint32_t>(state));
    uint32_t key2 = PasswordKey2;

    clear_dsm(Transfer_Direction::SDIO_TO_CARD);

    // CMD16 (SET_BLOCKLEN)
    sdio_->set_command_config(Command_Index::CMD16, 8, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    SDIO_Error_Type result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD13 (SEND_STATUS)
    sdio_->set_command_config(Command_Index::CMD13, (sdcard_rca_ << RCA_Shift),
                            Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD13);
    if (result != SDIO_Error_Type::OK) return result;

    volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);
    uint32_t timeout = 100000;

    while (((response & (1 << static_cast<uint32_t>(R1_Status::READY_FOR_DATA))) == 0) && (timeout != 0)) {
        // Continue to send CMD13 to poll the state of card until buffer is empty or timeout
        --timeout;
        // CMD13 (SEND_STATUS)
        sdio_->set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift),
                                Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);

        result = get_r1_result(Command_Index::CMD13);
        if (result != SDIO_Error_Type::OK) return result;

        response = sdio_->get_response(Response_Type::RESPONSE0);
    }
    if (timeout == 0) return SDIO_Error_Type::ERROR;

    // CMD42 (LOCK_UNLOCK)
    sdio_->set_command_config(Command_Index::CMD42, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD42);
    if (result != SDIO_Error_Type::OK) return result;

    response = sdio_->get_response(Response_Type::RESPONSE0);

    sdio_->data_configure(Data_Timeout, 8, Block_Size::BYTES_8);
    sdio_->data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::SDIO_TO_CARD);
    sdio_->set_data_state_machine_enable(true);

    sdio_->write_data_word(key1);
    sdio_->write_data_word(key2);

    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return SDIO_Error_Type::DATA_CRC_ERROR;
    } else if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return SDIO_Error_Type::DATA_TIMEOUT;
    } else if (sdio_->get_flag(Status_Flags::FLAG_TXURE) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_TXUREC);
        return SDIO_Error_Type::TX_FIFO_UNDERRUN;
    } else if (sdio_->get_flag(Status_Flags::FLAG_STBITE) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
        return SDIO_Error_Type::START_BIT_ERROR;
    }

    sdio_->clear_all_flags();

    Card_State card_state;
    result = get_card_state(&card_state);
    while ((result == SDIO_Error_Type::OK) && ((card_state == Card_State::PROGRAMMING) ||
                (card_state == Card_State::RECEIVE_DATA))) {
        result = get_card_state(&card_state);
    }

    return result;
}

Transfer_State Card::get_transfer_state() {
    Transfer_State transfer_state = Transfer_State::IDLE;
    if ((sdio_->get_flag(Status_Flags::FLAG_TXRUN) != false) && (sdio_->get_flag(Status_Flags::FLAG_RXRUN) != false)) {
        transfer_state = Transfer_State::BUSY;
    }

    return transfer_state;
}

uint32_t Card::get_card_capacity() const {
    auto extract_bits = [](uint32_t value, uint8_t start_bit, uint8_t length) -> uint32_t {
        return (value >> start_bit) & ((1 << length) - 1);
    };

    uint32_t capacity = 0;
    uint32_t device_size = 0;

    if (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) {
        // Extract fields from CSD data using bit manipulation
        uint8_t device_size_high = static_cast<uint8_t>(extract_bits(sdcard_csd_[1], 8, 2)); // Bits [69:68]
        uint8_t device_size_mid = static_cast<uint8_t>(extract_bits(sdcard_csd_[1], 0, 8));  // Bits [67:60]
        uint8_t device_size_low = static_cast<uint8_t>(extract_bits(sdcard_csd_[2], 24, 2)); // Bits [73:72]

        device_size = static_cast<uint32_t>((device_size_high) << 10) |
                      static_cast<uint32_t>((device_size_mid) << 2) |
                      static_cast<uint32_t>((device_size_low));

        uint8_t device_size_multiplier_high = static_cast<uint8_t>(extract_bits(sdcard_csd_[2], 16, 2)); // Bits [49:48]
        uint8_t device_size_multiplier_low = static_cast<uint8_t>(extract_bits(sdcard_csd_[2], 8, 1));    // Bit [47]
        uint8_t device_size_multiplier = static_cast<uint8_t>((device_size_multiplier_high << 1) | device_size_multiplier_low);
        uint8_t read_block_length = static_cast<uint8_t>(extract_bits(sdcard_csd_[1], 16, 4)); // Bits [83:80]

        // Capacity = (device_size + 1) * MULT * BLOCK_LEN
        uint32_t mult = static_cast<uint32_t>(1U << (device_size_multiplier + 2)); // MULT = 2 ^ (C_SIZE_MULT + 2)
        uint32_t block_length = static_cast<uint32_t>(1U << read_block_length);    // BLOCK_LEN = 2 ^ READ_BL_LEN

        capacity = (device_size + 1) * mult * block_length;
        capacity /= KILOBYTE; // Convert capacity to kilobytes

    } else if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        // High-capacity card calculation
        uint8_t device_size_high = static_cast<uint8_t>(extract_bits(sdcard_csd_[1], 0, 6));  // Bits [69:64]
        uint8_t device_size_mid = static_cast<uint8_t>(extract_bits(sdcard_csd_[2], 24, 8));  // Bits [55:48]
        uint8_t device_size_low = static_cast<uint8_t>(extract_bits(sdcard_csd_[2], 16, 8));  // Bits [47:40]

        device_size = static_cast<uint32_t>((device_size_high) << 16) |
                      static_cast<uint32_t>((device_size_mid) << 8) |
                      static_cast<uint32_t>(device_size_low);

        // Capacity in kilobytes
        capacity = (device_size + 1) * BLOCK_SIZE;
    }

    return capacity;
}

SDIO_Error_Type Card::get_card_specific_data(Card_Info* info) {
    if (info == nullptr) return SDIO_Error_Type::INVALID_PARAMETER;

    // Store card type and relative address
    info->type = card_type_;
    info->relative_address = sdcard_rca_;

    // Extract CID fields
    uint8_t cid_bytes[16];
    for (int i = 0; i < 4; ++i) {
        cid_bytes[i * 4]     = (sdcard_cid_[i] >> 24U) & 0xFF;
        cid_bytes[i * 4 + 1] = (sdcard_cid_[i] >> 16U) & 0xFF;
        cid_bytes[i * 4 + 2] = (sdcard_cid_[i] >> 8U) & 0xFF;
        cid_bytes[i * 4 + 3] = sdcard_cid_[i] & 0xFF;
    }

    info->cid.manufacture_id = static_cast<uint8_t>(cid_bytes[0]);
    info->cid.oem_id = static_cast<uint16_t>((cid_bytes[1]) << 8) | static_cast<uint16_t>(cid_bytes[2]);
    info->cid.name0 = (static_cast<uint32_t>(cid_bytes[3]) << 24) | (static_cast<uint32_t>(cid_bytes[4]) << 16) |
                      (static_cast<uint32_t>(cid_bytes[5]) << 8) | static_cast<uint32_t>(cid_bytes[6]);
    info->cid.name1 = static_cast<uint8_t>(cid_bytes[7]);
    info->cid.revision = static_cast<uint8_t>(cid_bytes[8]);
    info->cid.serial_number = (static_cast<uint32_t>(cid_bytes[9]) << 24) | (static_cast<uint32_t>(cid_bytes[10]) << 16) |
                              (static_cast<uint32_t>(cid_bytes[11]) << 8) | static_cast<uint32_t>(cid_bytes[12]);
    info->cid.manufacture_date = (static_cast<uint16_t>(cid_bytes[13] & 0x0F) << 8) | static_cast<uint32_t>(cid_bytes[14]);
    info->cid.checksum = static_cast<uint16_t>((cid_bytes[15] & 0xFE) >> 1U);

    // Extract CSD fields
    uint8_t csd_bytes[16];
    for (int i = 0; i < 4; ++i) {
        csd_bytes[i * 4]     = (sdcard_csd_[i] >> 24U) & 0xFF;
        csd_bytes[i * 4 + 1] = (sdcard_csd_[i] >> 16U) & 0xFF;
        csd_bytes[i * 4 + 2] = (sdcard_csd_[i] >> 8U) & 0xFF;
        csd_bytes[i * 4 + 3] = sdcard_csd_[i] & 0xFF;
    }

    info->csd.csd_structure = static_cast<uint8_t>((csd_bytes[0] & 0xC0) >> 6U);
    info->csd.read_access_time = static_cast<uint8_t>(csd_bytes[1]);
    info->csd.read_access_clock_cycles = static_cast<uint8_t>(csd_bytes[2]);
    info->csd.transfer_speed = static_cast<uint8_t>(csd_bytes[3]);
    info->csd.card_command_class = static_cast<uint16_t>((csd_bytes[4] << 4) | ((csd_bytes[5] & 0xF0) >> 4U));
    info->csd.read_block_length = static_cast<uint8_t>(csd_bytes[5] & 0x0F);
    info->csd.read_partial_block = static_cast<uint8_t>((csd_bytes[6] & 0x80) >> 7U);
    info->csd.write_unaligned_block = static_cast<uint8_t>((csd_bytes[6] & 0x40) >> 6U);
    info->csd.read_unaligned_block = static_cast<uint8_t>((csd_bytes[6] & 0x20) >> 5U);
    info->csd.dsr_available = static_cast<uint8_t>((csd_bytes[6] & 0x10) >> 4U);

    if (card_type_ == Card_Type::SDCARD_STANDARD_CAPACITY) {
        // SDSC card CSD version 1.0
        info->csd.device_size = (static_cast<uint32_t>(csd_bytes[6] & 0x03) << 10) |
                                (static_cast<uint32_t>(csd_bytes[7]) << 2) |
                                (static_cast<uint32_t>((csd_bytes[8] & 0xC0) >> 6U));
        info->csd.min_vdd_read = static_cast<uint8_t>((csd_bytes[8] & 0x38) >> 3U);
        info->csd.max_vdd_read = static_cast<uint8_t>(csd_bytes[8] & 0x07);
        info->csd.min_vdd_write = static_cast<uint8_t>((csd_bytes[9] & 0xE0) >> 5U);
        info->csd.max_vdd_write = static_cast<uint8_t>((csd_bytes[9] & 0x1C) >> 2U);
        info->csd.device_size_multiplier = static_cast<uint8_t>((csd_bytes[9] & 0x03) << 1 | (csd_bytes[10] & 0x80) >> 7U);
        info->block_size = static_cast<uint32_t>(1 << info->csd.read_block_length);
        info->capacity = static_cast<uint32_t>((info->csd.device_size + 1) * (1 << (info->csd.device_size_multiplier + 2)) * info->block_size);
    } else if (card_type_ == Card_Type::SDCARD_HIGH_CAPACITY) {
        // SDHC card CSD version 2.0
        info->csd.device_size = static_cast<uint32_t>((csd_bytes[7] & 0x3F) << 16) |
                                static_cast<uint32_t>((csd_bytes[8]) << 8) |
                                static_cast<uint32_t>(csd_bytes[9]);
        info->block_size = BLOCK_SIZE;
        info->capacity = static_cast<uint32_t>((info->csd.device_size + 1) * BLOCK_SIZE * KILOBYTE);
    }

    info->csd.erase_block_enable = static_cast<uint8_t>((csd_bytes[9] & 0x40) >> 6U);
    info->csd.sector_size = static_cast<uint8_t>(((csd_bytes[9] & 0x3F) << 1) | (csd_bytes[10] & 0x80) >> 7U);
    info->csd.write_protect_group_size = static_cast<uint8_t>(csd_bytes[10] & 0x7F);
    info->csd.write_protect_group_enable = static_cast<uint8_t>((csd_bytes[11] & 0x80) >> 7U);
    info->csd.speed_factor = static_cast<uint8_t>((csd_bytes[11] & 0x1C) >> 2U);
    info->csd.write_block_length = static_cast<uint8_t>(((csd_bytes[11] & 0x03) << 2) | ((csd_bytes[12] & 0xC0U) >> 6U));
    info->csd.write_partial_block = static_cast<uint8_t>((csd_bytes[12] & 0x20) >> 5U);
    info->csd.file_format_group = static_cast<uint8_t>((csd_bytes[13] & 0x80) >> 7U);
    info->csd.copy_flag = static_cast<uint8_t>((csd_bytes[13] & 0x40) >> 6U);
    info->csd.permanent_write_protect = static_cast<uint8_t>((csd_bytes[13] & 0x20) >> 5U);
    info->csd.temporary_write_protect = static_cast<uint8_t>((csd_bytes[13] & 0x10) >> 4U);
    info->csd.file_format = static_cast<uint8_t>((csd_bytes[13] & 0x0C) >> 2U);
    info->csd.checksum = static_cast<uint8_t>((csd_bytes[15] & 0xFE) >> 1U);

    return SDIO_Error_Type::OK;
}

Block_Size Card::get_data_block_size_index(uint16_t size) {
    uint8_t index = 0;

    while (size != 1) {
        size >>= 1;
        ++index;
    }
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ), static_cast<uint32_t>(index));

    return static_cast<Block_Size>(read_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ)));
}

SDIO_Error_Type Card::get_card_state(Card_State* card_state) {
    // CMD13 (SEND_STATUS)
    sdio_->set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR) |
                      read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) |
                      read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))))) {
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))) {
        sdio_->clear_all_flags();
        return SDIO_Error_Type::OK;
    } else if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR))) {
        sdio_->clear_all_flags();
        return SDIO_Error_Type::COMMAND_CRC_ERROR;
    } else if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT))) {
        sdio_->clear_all_flags();
        return SDIO_Error_Type::RESPONSE_TIMEOUT;
    }

    if (sdio_->get_command_index() != static_cast<uint8_t>(Command_Index::CMD13)) {
        return SDIO_Error_Type::ILLEGAL_COMMAND;
    }

    sdio_->clear_all_flags();

    volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);

    uint8_t response_state = static_cast<uint8_t>((response >> 9) & CardStateMask);
    if (!(response & Card_Locked)) {
        *card_state = static_cast<Card_State>(response_state);
    } else {
        *card_state = Card_State::LOCKED;
    }

    if ((response & All_R1_Error_Bits) == 0) {
        return SDIO_Error_Type::OK;
    }

    return get_r1_error_type(response);
}

SDIO_Error_Type Card::get_command_sent_result() {
    uint32_t timeout = 100000;

    while ((sdio_->get_flag(Status_Flags::FLAG_CMDSEND) == false) && (timeout != 0)) {
        --timeout;
    }

    if (timeout == 0) return SDIO_Error_Type::RESPONSE_TIMEOUT;

    sdio_->clear_all_flags();

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::check_sdio_status(Command_Index index, bool check_index) {
    // Wait until one of the relevant flags is set
    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV)))) {
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))) {
        sdio_->clear_all_flags();
        return SDIO_Error_Type::OK;
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR))) {
        sdio_->clear_flag(Clear_Flags::FLAG_CCRCERRC);
        return SDIO_Error_Type::COMMAND_CRC_ERROR;
    }

    if (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT))) {
        sdio_->clear_flag(Clear_Flags::FLAG_CMDTMOUTC);
        return SDIO_Error_Type::RESPONSE_TIMEOUT;
    }

    if (check_index && sdio_->get_command_index() != static_cast<uint8_t>(index)) {
        return SDIO_Error_Type::ILLEGAL_COMMAND;
    }

    // Clear all flags before returning
    sdio_->clear_all_flags();

    return SDIO_Error_Type::OK;
}

SDIO_Error_Type Card::get_r1_result(Command_Index index) {
    SDIO_Error_Type result = check_sdio_status(index, true);
    if (result != SDIO_Error_Type::OK) return result;

    // Get the R1 response and check for errors
    volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);
    if ((response & All_R1_Error_Bits) == 0) return SDIO_Error_Type::OK;

    return get_r1_error_type(response);
}

SDIO_Error_Type Card::get_r2_r3_result() {
    return check_sdio_status();
}

SDIO_Error_Type Card::get_r6_result(Command_Index index, uint16_t *rca) {
    SDIO_Error_Type result = check_sdio_status(index, true);
    if (result != SDIO_Error_Type::OK) return result;

    volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);

    if ((response & (static_cast<uint32_t>(R6_Status::COM_CRC_ERROR) |
                     static_cast<uint32_t>(R6_Status::ILLEGAL_COMMAND) |
                     static_cast<uint32_t>(R6_Status::ERROR))) == 0) {
        *rca = static_cast<uint16_t>(response >> 16U);
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
    uint32_t timeout = 100000;

    while (!(read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CCRCERR)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDTMOUT)) ||
             read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(Status_Flags::FLAG_CMDRECV))) && (timeout != 0)) {
        --timeout;
    }

    if (timeout == 0) return SDIO_Error_Type::RESPONSE_TIMEOUT;

    SDIO_Error_Type result = check_sdio_status();
    if (result != SDIO_Error_Type::OK) return result;

    return SDIO_Error_Type::OK;
}

static SDIO_Error_Type get_r1_error_type(uint32_t response) {
    switch (static_cast<R1_Status>(response)) {
    case R1_Status::OUT_OF_RANGE: return SDIO_Error_Type::COMMAND_OUT_OF_RANGE;
    case R1_Status::ADDRESS_ERROR: return SDIO_Error_Type::INVALID_ADDRESS;
    case R1_Status::BLOCK_LEN_ERROR: return SDIO_Error_Type::INVALID_BLOCK_LENGTH;
    case R1_Status::ERASE_SEQ_ERROR: return SDIO_Error_Type::ERASE_SEQUENCE_ERROR;
    case R1_Status::ERASE_PARAM: return SDIO_Error_Type::INVALID_ERASE_BLOCKS;
    case R1_Status::WP_VIOLATION: return SDIO_Error_Type::WRITE_PROTECT_VIOLATION;
    case R1_Status::LOCK_UNLOCK_FAILED: return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    case R1_Status::COM_CRC_ERROR: return SDIO_Error_Type::COMMAND_CRC_ERROR;
    case R1_Status::ILLEGAL_COMMAND: return SDIO_Error_Type::ILLEGAL_COMMAND;
    case R1_Status::CARD_ECC_FAILED: return SDIO_Error_Type::ECC_FAILED;
    case R1_Status::CC_ERROR: return SDIO_Error_Type::CARD_CONTROLLER_ERROR;
    case R1_Status::ERROR: return SDIO_Error_Type::ERROR;
    case R1_Status::CID_CSD_OVERWRITE: return SDIO_Error_Type::CSD_OVERWRITE;
    case R1_Status::WP_ERASE_SKIP: return SDIO_Error_Type::ERASE_SKIP;
    case R1_Status::CARD_ECC_DISABLED: return SDIO_Error_Type::ECC_DISABLED;
    case R1_Status::ERASE_RESET: return SDIO_Error_Type::ERASE_SEQUENCE_RESET;
    case R1_Status::AKE_SEQ_ERROR: return SDIO_Error_Type::AUTHENTICATION_ERROR;
    default: return SDIO_Error_Type::ERROR;
    }
}

SDIO_Error_Type Card::set_interface_bus_width(Bus_Width width) {
    // Check if card is locked
    if (sdio_->get_response(Response_Type::RESPONSE0) & Card_Locked) {
        return SDIO_Error_Type::LOCK_UNLOCK_FAILED;
    }

    // Retrieve SCR
    SDIO_Error_Type result = get_scr(sdcard_rca_, sdcard_scr_);
    if (result != SDIO_Error_Type::OK) return result;

    // Check and set bus width
    if (width == Bus_Width::WIDTH_1BIT && (sdcard_scr_[1] & SCR_Width_1)) {
        return send_bus_width_command(0);
    } 
    if (width == Bus_Width::WIDTH_4BIT && (sdcard_scr_[1] & SCR_Width_4)) {
        return send_bus_width_command(2);
    }

    return (width == Bus_Width::WIDTH_1BIT || width == Bus_Width::WIDTH_4BIT) ? 
           SDIO_Error_Type::INVALID_OPERATION : SDIO_Error_Type::INVALID_PARAMETER;
}

SDIO_Error_Type Card::send_bus_width_command(uint32_t width_value) {
    // Send CMD55 (APP_CMD)
    sdio_->set_command_config(Command_Index::CMD55, sdcard_rca_ << RCA_Shift, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);
    SDIO_Error_Type result = get_r1_result(Command_Index::CMD55);
    if (result != SDIO_Error_Type::OK) return result;

    // Send ACMD6 (SET_BUS_WIDTH)
    sdio_->set_command_config(Command_Index::ACMD6, width_value, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    return get_r1_result(Command_Index::ACMD6);
}

SDIO_Error_Type Card::get_scr(uint16_t rca, uint32_t *scr) {
    uint32_t temp_scr[2] = {0, 0};
    uint32_t index_scr = 0;

    // CMD16 (SET_BLOCKLEN)
    sdio_->set_command_config(Command_Index::CMD16, 8, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    SDIO_Error_Type result = get_r1_result(Command_Index::CMD16);
    if (result != SDIO_Error_Type::OK) return result;

    // CMD55 (APP_CMD)
    sdio_->set_command_config(Command_Index::CMD55, static_cast<uint32_t>(rca << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::CMD55);
    if (result != SDIO_Error_Type::OK) return result;

    sdio_->data_configure(Data_Timeout, 8, Block_Size::BYTES_8);
    sdio_->data_transfer_configure(Transfer_Mode::BLOCK, Transfer_Direction::CARD_TO_SDIO);
    sdio_->set_data_state_machine_enable(true);

    // ACMD51 (SEND_SCR)
    sdio_->set_command_config(Command_Index::ACMD51, 0, Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
    sdio_->send_command(true);

    result = get_r1_result(Command_Index::ACMD51);
    if (result != SDIO_Error_Type::OK) return result;

    // Store SCR
    Status_Flags flags[] = {
        Status_Flags::FLAG_DTCRCERR,
        Status_Flags::FLAG_DTTMOUT,
        Status_Flags::FLAG_RXORE,
        Status_Flags::FLAG_DTBLKEND,
        Status_Flags::FLAG_STBITE
    };

    while (!sdio_->any_flag_set(flags, sizeof(flags) / sizeof(Status_Flags))) {
        if (sdio_->get_flag(Status_Flags::FLAG_RXDTVAL) != false) {
            *(temp_scr + index_scr) = sdio_->read_data_word();
            ++index_scr;
        }
    }

    if (sdio_->get_flag(Status_Flags::FLAG_DTCRCERR) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTCRCERRC);
        return SDIO_Error_Type::DATA_CRC_ERROR;
    } else if (sdio_->get_flag(Status_Flags::FLAG_DTTMOUT) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_DTTMOUTC);
        return SDIO_Error_Type::DATA_TIMEOUT;
    } else if (sdio_->get_flag(Status_Flags::FLAG_RXORE) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_RXOREC);
        return SDIO_Error_Type::RX_FIFO_OVERRUN;
    } else if (sdio_->get_flag(Status_Flags::FLAG_STBITE) != false) {
        sdio_->clear_flag(Clear_Flags::FLAG_STBITEC);
        return SDIO_Error_Type::START_BIT_ERROR;
    }

    sdio_->clear_all_flags();

    // adjust SCR value
    *(scr) = ((temp_scr[1] & 0xFF) << 24) | ((temp_scr[1] & (0xFFU << 8)) << 8) |
             ((temp_scr[1] & (0xFFU << 16)) >> 8U) | ((temp_scr[1] & (0xFFU << 24)) >> 24U);
    *(scr + 1) = ((temp_scr[0] & 0xFF) << 24) | ((temp_scr[0] & (0xFFU << 8)) << 8) |
                 ((temp_scr[0] & (0xFFU << 16)) >> 8U) | ((temp_scr[0] & (0xFFU << 24)) >> 24U);

    return result;
}

// DMA for rx/tx is always DMA1 channel 3
void Card::dma_configure(uint32_t* buf, uint32_t size, bool is_write) {
    dma_->clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_GIF);
    dma_->clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_FTFIF);
    dma_->clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_HTFIF);
    dma_->clear_flag(dma::DMA_Channel::CHANNEL3, dma::Status_Flags::FLAG_ERRIF);
    dma_->set_channel_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_->reset(dma::DMA_Channel::CHANNEL3);

    dma_config_ = &dma_->get_config();
    // DMA_Config receive parameters
    dma_config_->peripheral_address = reinterpret_cast<uint32_t>(sdio_->reg_address(SDIO_Regs::FIFO));
    dma_config_->memory_address = reinterpret_cast<uint32_t>(buf);
    dma_config_->count = size / 4;
    dma_config_->peripheral_bit_width = dma::Bit_Width::WIDTH_32BIT;
    dma_config_->memory_bit_width = dma::Bit_Width::WIDTH_32BIT;
    dma_config_->peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    dma_config_->memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    dma_config_->channel_priority = dma::Channel_Priority::MEDIUM_PRIORITY;
    dma_config_->direction = is_write ? dma::Transfer_Direction::M2P : dma::Transfer_Direction::P2M;

    dma_->configure(dma::DMA_Channel::CHANNEL3, *dma_config_);
    dma_->set_circulation_mode_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_->set_channel_enable(dma::DMA_Channel::CHANNEL3, true);
}

SDIO_Error_Type Card::wait_for_card_ready() {
    uint32_t timeout = 100000;
    volatile uint32_t response = sdio_->get_response(Response_Type::RESPONSE0);

    while (((response & static_cast<uint32_t>(R1_Status::READY_FOR_DATA)) == 0) && (timeout != 0)) {
        // continue to send CMD13 to polling the state of card until buffer empty or timeout
        --timeout;
        // CMD13 (SEND_STATUS)
        sdio_->set_command_config(Command_Index::CMD13, static_cast<uint32_t>(sdcard_rca_ << RCA_Shift), Command_Response::RSP_SHORT, Wait_Type::WT_NONE);
        sdio_->send_command(true);
        SDIO_Error_Type result = get_r1_result(Command_Index::CMD13);
        if (result != SDIO_Error_Type::OK) return result;

        response = sdio_->get_response(Response_Type::RESPONSE0);
    }

    return (timeout == 0) ? SDIO_Error_Type::ERROR : SDIO_Error_Type::OK;
}

Card& SDCard_Handle() {
    static Card instance;
    return instance;
}

} // namespace sdio

#endif // DISABLE_SDIO_CARD_DRIVER
