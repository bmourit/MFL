// gd32f30x SDIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdint>

#include "BitRW.hpp"
#include "ErrorTypes.hpp"
#include "RCU.hpp"
#include "GPIO.hpp"
#include "sdio_config.hpp"

namespace sdio {

class SDIO {
public:
    SDIO() {}

    // Initialize
    void init();
    // Reset
    void reset();
    // Configure
    void configure(SDIO_Config *config) {
        if (config) {
            config_ = *config;
        }
        init();
    }
    // Clock configure
    void clock_configure(Clock_Edge edge, bool bypass, bool low_power, uint16_t divider);
    // Hardware clock
    void set_hardware_clock_enable(bool enable);
    // Bus width
    void set_bus_width(Bus_Width width);
    // Power
    uint32_t get_power_mode();
    void set_power_mode(Power_Control power);
    // Clock
    void set_clock_enable(bool enable);
    // Command
    void set_command_config(Command_Index command, uint32_t argument, Command_Response response, Wait_Type type);
    // Command state machine
    void send_command(bool enable);
    // Index and last response
    uint8_t get_command_index();
    uint32_t get_response(Response_Type type);
    // Configure
    void data_configure(uint32_t timeout, uint32_t length, Block_Size size);
    void data_transfer_configure(Transfer_Mode mode, Transfer_Direction direction);
    // Data state machine
    void set_data_state_machine_enable(bool enable);
    // Data read/write
    uint32_t read_data_word();
    void write_data_word(uint32_t data);
    // Count
    uint32_t get_data_count();
    uint32_t get_fifo_count();
    // DMA
    void set_dma_enable(bool enable);
    // Flags
    bool get_flag(Status_Flags flag) const;
    void clear_flag(Clear_Flags flag);
    void clear_all_flags();
    // Helper
    bool any_flag_set(const std::initializer_list<Status_Flags>& flags) const {
        for (const auto& flag : flags) {
            if (get_flag(flag)) {
                return true;
            }
        }
        return false;
    }
    // Interrupt flags
    bool get_interrupt_flag(Interrupt_Flags int_flag);
    void clear_interrupt_flag(Clear_Flags int_flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Type type, bool enable);
    // Read wait
    void set_read_wait_enable(bool enable);
    void set_stop_read_wait_enable(bool enable);
    void set_read_wait_type(Read_Wait type);
    // IO
    void set_IO_operation_enable(bool enable);
    void set_IO_suspend_enable(bool enable);
    // CE-ATA
    void set_CE_ATA_command_enable(bool enable);
    void set_CE_ATA_interrupt_enable(bool enable);
    void set_CE_ATA_command_completion_enable(bool enable);

    static constexpr uint32_t SDIO_baseAddress = 0x40018000;

    inline volatile uint32_t *reg_address(SDIO_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(SDIO_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    // Default dummy pin config
    SDIO_Config default_config = {};
    SDIO_Config& config_ = default_config;

    template<typename T>
    inline T read_register(SDIO_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(SDIO_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace sdio

extern sdio::SDIO SDIO_DEVICE;
