// gd32f30x SDCARD common functions using SDIO in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#ifndef DISABLE_SDIO_CARD_DRIVER

#include <stdint.h>

#include "RegRW.hpp"
#include "SDIO.hpp"
#include "DMA.hpp"
#include "sdio_config.hpp"

namespace sdio {

class Card {
public:
    Card() : sdio_(&SDIO_DEVICE()), dma_(nullptr), dma_config_(nullptr), sdcard_scr_{0}, sdcard_csd_{0},
             sdcard_cid_{0}, sdcard_rca_{0}, sdcard_class_{0}, card_type_{Card_Type::UNKNOWN},
             transfer_method_{Transfer_Method::METHOD_POLLING}, transfer_error_{SDIO_Error_Type::OK},
             total_bytes_{0}, stop_condition_{0}, transfer_end_{0}, count_{0} {
                auto result = dma::DMA::get_instance(dma::DMA_Base::DMA1_BASE);
                if (result.error() == dma::DMA_Error_Type::OK) {
                    dma_ = &result.value();
                    dma_config_ = &dma_->get_config();
                }
            }

    SDIO_Error_Type init();
    SDIO_Error_Type card_init();
    SDIO_Error_Type begin_startup_procedure();
    void begin_shutdown_procedure();
    // Configuration
    SDIO_Error_Type set_bus_width_config(Bus_Width width);
    SDIO_Error_Type set_transfer_method(Transfer_Method method);
    // Main read functions
    SDIO_Error_Type read_single_block(uint32_t *buf, uint32_t address, uint16_t size);
    SDIO_Error_Type read_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count);
    // Main write functions
    SDIO_Error_Type write_single_block(uint32_t *buf, uint32_t address, uint16_t size);
    SDIO_Error_Type write_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count);

    // Polling single block transfers
    SDIO_Error_Type read_single_block_polling(uint32_t* buf);
    SDIO_Error_Type write_single_block_polling(uint32_t* buf);
    // Polling multiblock transfers
    SDIO_Error_Type read_blocks_polling(uint32_t* buf);
    SDIO_Error_Type write_blocks_polling(uint32_t* buf);
    // DMA transfers
    SDIO_Error_Type read_write_dma(uint32_t* buf, uint32_t size, bool is_write);

    // Other card functions
    SDIO_Error_Type erase(uint32_t address_start, uint32_t address_end);
    SDIO_Error_Type handle_interrupts();
    SDIO_Error_Type select_deselect(uint16_t card);
    SDIO_Error_Type get_card_status(uint32_t *status);
    SDIO_Error_Type get_sdcard_status(uint32_t *status);
    SDIO_Error_Type stop_transfer();
    SDIO_Error_Type set_lock_unlock(Lock_State state);
    Transfer_State get_transfer_state();
    uint32_t get_card_capacity() const;
    SDIO_Error_Type set_interface_bus_width(Bus_Width width);
    SDIO_Error_Type send_bus_width_command(uint32_t width_value);
    SDIO_Error_Type get_card_specific_data(Card_Info *info);
    Block_Size get_data_block_size_index(uint16_t size);
    SDIO_Error_Type handle_v1_card();
    SDIO_Error_Type validate_voltage();
    SDIO_Error_Type get_card_state(Card_State *card_state);
    SDIO_Error_Type check_sdio_status(Command_Index index = Command_Index::INVALID, bool check_index = false);
    SDIO_Error_Type get_r1_result(Command_Index index);
    SDIO_Error_Type get_r2_r3_result();
    SDIO_Error_Type get_r6_result(Command_Index index, uint16_t *rca);
    SDIO_Error_Type get_r7_result();
    SDIO_Error_Type get_scr(uint16_t rca, uint32_t *scr);
    SDIO_Error_Type store_cid();
    SDIO_Error_Type store_csd();

    // DMA
    void initializeDMA() {
        dma_->init(dma::DMA_Channel::CHANNEL3);
    }
    void dma_configure(uint32_t *buf, uint32_t size, bool is_write);

    // Helpers
    SDIO_Error_Type setup_transfer(uint32_t address, uint16_t size, bool is_write, bool multi_block);
    SDIO_Error_Type wait_for_card_ready();

    template <typename CheckFunc>
    SDIO_Error_Type send_command_and_check(Command_Index command, uint32_t argument, Command_Response response, Wait_Type type, CheckFunc check_result) {
        sdio_->set_command_config(command, argument, response, type);
        sdio_->send_command(true);
        return check_result();
    }
    SDIO_Error_Type get_command_sent_result();

    static constexpr uintptr_t SDIO_baseAddress = 0x40018000U;

    inline volatile uint32_t* reg_address(SDIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t*>(SDIO_baseAddress + static_cast<uint32_t>(reg));
    }

    void sdio_configure(SDIO_Config* config) {
        sdio_->configure(*config);
    }

    void sdio_pin_configure(SDIO_Pins* pin_config) {
        sdio_->pin_configure(*pin_config);
    }

    SDIO_Pins& get_sdio_pin_config() {
        return sdio_->get_pin_config();
    }

    inline void clear_dsm(Transfer_Direction direction) {
        sdio_->data_configure(0, 0, Block_Size::BYTES_1);
        sdio_->data_transfer_configure(Transfer_Mode::BLOCK, direction);
        sdio_->set_data_state_machine_enable(false);
        sdio_->set_dma_enable(false);
    }

    inline bool validate_transfer_params(uint32_t* buf, uint16_t size) {
        if (buf == nullptr) return false;
        // Size must be > 0, <= 2048 and power of 2
        if ((size == 0) || (size > 2048) || (size & (size - 1))) {
            return false;
        }
        return true;
    }

private:
    SDIO* sdio_;
    dma::DMA* dma_;
    dma::DMA_Config* dma_config_;
    uint32_t sdcard_scr_[2];
    uint32_t sdcard_csd_[4];
    uint32_t sdcard_cid_[4];
    uint16_t sdcard_rca_;
    uint16_t sdcard_class_;

    Card_Type card_type_;
    Transfer_Method transfer_method_;
    SDIO_Error_Type transfer_error_;

    uint32_t total_bytes_;
    uint32_t stop_condition_;

    volatile uint32_t transfer_end_;
    volatile uint32_t count_;
};

Card& SDCard_Handle();

} // namespace sdio

#endif // DISABLE_SDIO_CARD_DRIVER

