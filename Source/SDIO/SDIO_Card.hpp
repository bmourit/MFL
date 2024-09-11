// gd32f30x SDCARD common functions using SDIO in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#if !defined(DISABLE_SDIO_CARD_DRIVER)

#include <cstdint>
#include <functional>

#include "RegRW.hpp"
#include "SDIO.hpp"
#include "DMA.hpp"
#include "sdio_config.hpp"

namespace sdio {

class Card {
public:
    Card() : sdio_(SDIO_DEVICE) {}

    SDIO_Error_Type init();
    SDIO_Error_Type card_init();
    SDIO_Error_Type begin_startup_procedure();
    void begin_shutdown_procedure();

    SDIO_Error_Type set_bus_width_config(Bus_Width width);
    SDIO_Error_Type set_transfer_method(Transfer_Method method);

    SDIO_Error_Type read_single_block(uint32_t *buf, uint32_t address, uint16_t size);
    SDIO_Error_Type read_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count);

    SDIO_Error_Type write_single_block(uint32_t *buf, uint32_t address, uint16_t size);
    SDIO_Error_Type write_blocks(uint32_t *buf, uint32_t address, uint16_t size, uint32_t count);

    SDIO_Error_Type erase(uint32_t address_start, uint32_t address_end);
    SDIO_Error_Type handle_interrupts();

    SDIO_Error_Type select_deselect(uint16_t card);
    SDIO_Error_Type get_card_status(uint32_t *status);
    SDIO_Error_Type get_sdcard_status(uint32_t *status);
    SDIO_Error_Type stop_transfer();
    SDIO_Error_Type set_lock_unlock(Lock_State state);
    Transfer_State get_transfer_state();
    uint32_t get_card_capacity();
    SDIO_Error_Type set_interface_bus_width(Bus_Width width);
    SDIO_Error_Type get_card_specific_data(Card_Info *info);
    Block_Size get_data_block_size_index(uint16_t size);
    SDIO_Error_Type handle_v1_card();
    SDIO_Error_Type validate_voltage();
    SDIO_Error_Type get_card_state(uint8_t *card_state);
    SDIO_Error_Type check_sdio_status(Command_Index index = Command_Index::INVALID, bool check_index = false);
    SDIO_Error_Type send_command_and_check(Command_Index cmd, uint32_t arg, Command_Response resp, Wait_Type wait, std::function<SDIO_Error_Type()> check_result);
    SDIO_Error_Type get_command_sent_result();
    SDIO_Error_Type get_r1_result(Command_Index index);
    SDIO_Error_Type get_r2_r3_result();
    SDIO_Error_Type get_r6_result(Command_Index index, uint16_t *rca);
    SDIO_Error_Type get_r7_result();
    SDIO_Error_Type get_scr(uint16_t rca, uint32_t *scr);
    SDIO_Error_Type store_cid();
    SDIO_Error_Type store_csd();
    void dma_transfer_configure(uint32_t *buf, uint32_t size);
    void dma_receive_configure(uint32_t *buf, uint32_t size);

    static constexpr uintptr_t SDIO_baseAddress = 0x40018000;

    inline volatile uint32_t *reg_address(SDIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(SDIO_baseAddress + static_cast<uint32_t>(reg));
    }

    inline void ensure_clock_enabled() const {}

private:
    SDIO& sdio_;
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

} // namespace sdio

extern sdio::Card SDCard_Handle;

#endif // DISABLE_SDIO_CARD_DRIVER

