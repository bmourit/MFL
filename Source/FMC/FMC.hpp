// gd32f30x FMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "BitRW.hpp"
#include "RCU.hpp"
#include "fmc_config.hpp"

namespace fmc {

class FMC {
public:
    FMC() {}

    // Lock
    void lock();
    void lock_bank0();
    void lock_bank1();
    // Unlock
    void unlock();
    void unlock_bank0();
    void unlock_bank1();
    // Erase
    FMC_State mass_erase();
    FMC_State erase_page(uint32_t address);
    FMC_State erase_bank0();
    FMC_State erase_bank1();
    // Program
    FMC_State program_word(uint32_t address, uint32_t data);
    FMC_State program_halfword(uint32_t address, uint16_t data);
    FMC_State reprogram_word(uint32_t address, uint32_t data);
    // Delay
    void set_delay_count(Wait_State wait);
    // State
    FMC_State get_bank0_state();
    FMC_State get_bank1_state();
    // Wait
    FMC_State ready_wait_bank0(uint32_t timeout);
    FMC_State ready_wait_bank1(uint32_t timeout);
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    // Interruppt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Types type, bool enable);

    // Base address
    static constexpr uint32_t FMC_baseAddress = 0x40022000;

    // Register address
    inline volatile uint32_t *reg_address(FMC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(FMC_baseAddress + static_cast<uint32_t>(reg));
    }

private:
    template<typename T>
    inline T read_register(FMC_Regs reg) const {
        return *reinterpret_cast<volatile T *>(reg_address(reg));
    }

    template<typename T>
    inline void write_register(FMC_Regs reg, T value) {
        *reinterpret_cast<volatile T *>(reg_address(reg)) = value;
    }
};

} // namespace fmc

extern fmc::FMC FMC_DEVICE;
