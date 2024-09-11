// gd32f30x OB peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
#include "RCU.hpp"
#include "fmc_config.hpp"

namespace fmc {

class OB {
public:
    explicit OB() {}

    // Lock
    void ob_lock();
    void ob_unlock();
    // Erase
    FMC_Error_Type ob_erase();
    // Write protection
    FMC_Error_Type set_ob_write_protection(WP_Sector sector);
    uint32_t get_ob_write_protection();
    // Security protection
    FMC_Error_Type set_ob_security_protection(OB_Security_Type type);
    bool get_ob_security_protection();
    // User
    FMC_Error_Type set_ob_user(OB_Watchdog_Type type, OB_Deep_Sleep deepsleep, OB_Standby standby, OB_Boot_Bank bank);
    uint8_t get_ob_user();
    // Data
    FMC_Error_Type set_ob_data(uint32_t address, uint8_t data);
    uint16_t get_ob_data();
    // State
    FMC_Error_Type ob_get_bank0_state();
    FMC_Error_Type ob_get_bank1_state();
    // Wait state
    FMC_Error_Type ob_ready_wait_bank0(uint32_t timeout);
    FMC_Error_Type ob_ready_wait_bank1(uint32_t timeout);

    static constexpr uintptr_t FMC_baseAddress = 0x40022000;
    static constexpr uintptr_t OB_baseAddress = 0x1FFFF800;

    inline volatile uint32_t *reg_address(OB_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(OB_baseAddress + static_cast<uint32_t>(reg));
    }

    inline volatile uint32_t *reg_address(FMC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(FMC_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}
};

} // namespace fmc

extern fmc::OB OB_DEVICE;
