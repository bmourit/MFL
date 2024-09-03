// gd32f30x OB peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "OB.hpp"

namespace fmc {

void OB::ob_lock()
{
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBWEN), Set);
}

void OB::ob_unlock()
{
    if (read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBWEN)) == Clear) {
        write_register(FMC_Regs::OBKEY, static_cast<uint32_t>(UNLOCK_KEY0));
        write_register(FMC_Regs::OBKEY, static_cast<uint32_t>(UNLOCK_KEY1));
    }
}

FMC_State OB::ob_erase()
{
    uint16_t value = 0xA5;
    FMC_State state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);

    if (get_ob_security_protection() == true) {
        value = 0xBB;
    }

    if (state == FMC_State::READY) {
        write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBER), Set,
                static_cast<uint32_t>(CTL0_Bits::START), Set);
        // Wait until ready
        state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBER), Clear,
                    static_cast<uint32_t>(CTL0_Bits::OBPG), Set);
            write_bit(*this, OB_Regs::SPC, static_cast<uint32_t>(SPC_Bits::SPC), value);
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state != FMC_State::TIMEOUT) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
            }
        } else {
            if (state != FMC_State::TIMEOUT) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
            }
        }
    }

    return state;
}

FMC_State OB::set_ob_write_protection(WP_Sector sector)
{
    uint16_t wp_sector_value;
    FMC_State state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
    uint32_t wp_sector = static_cast<uint32_t>(sector);

    wp_sector = ~wp_sector;
    if (state == FMC_State::READY) {
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Set);
        wp_sector_value = (wp_sector & 0x000000FF);
        if (wp_sector_value != 0xFF) {
            write_register(OB_Regs::WP0, wp_sector_value);
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        }
        wp_sector_value = ((wp_sector & 0x0000FF00) >> 8);
        if ((state == FMC_State::READY) && (wp_sector_value != 0xFF)) {
            write_register(OB_Regs::WP1, wp_sector_value);
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        }
        wp_sector_value = ((wp_sector & 0x00FF0000) >> 16);
        if ((state == FMC_State::READY) && (wp_sector_value != 0xFF)) {
            write_register(OB_Regs::WP2, wp_sector_value);
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        }
        wp_sector_value = ((wp_sector & 0xFF000000) >> 24);
        if ((state == FMC_State::READY) && (wp_sector_value != 0xFF)) {
            write_register(OB_Regs::WP3, wp_sector_value);
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        }
        if (state != FMC_State::TIMEOUT) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
        }
    }

    return state;
}

uint32_t OB::get_ob_write_protection()
{
    return read_register<uint32_t>(FMC_Regs::WP);
}

FMC_State OB::set_ob_security_protection(OB_Security_Type type)
{
    FMC_State state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);

    if (state == FMC_State::READY) {
        write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBER), Set,
                static_cast<uint32_t>(CTL0_Bits::START), Set);
        // Wait until ready
        state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBER), Clear,
                    static_cast<uint32_t>(CTL0_Bits::OBPG), Set);
            write_bit(*this, OB_Regs::SPC, static_cast<uint32_t>(SPC_Bits::SPC), static_cast<uint32_t>(type));
            // Wait until ready
            state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state != FMC_State::TIMEOUT) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
            }
        } else {
            if (state != FMC_State::TIMEOUT) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBER), Clear);
            }
        }
    }

    return state;
}

bool OB::get_ob_security_protection()
{
    return (read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::SPC)) != Clear);
}

FMC_State OB::set_ob_user(OB_Watchdog_Type type, OB_Deep_Sleep deepsleep, OB_Standby standby, OB_Boot_Bank bank)
{
    FMC_State state = FMC_State::READY;
    uint32_t user_op;

    state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
    if (state == FMC_State::READY) {
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Set);
        user_op = static_cast<uint32_t>(bank) | static_cast<uint32_t>(type) | static_cast<uint32_t>(deepsleep) | static_cast<uint32_t>(standby) | 0x000000F0;
        write_bit(*this, OB_Regs::USER, static_cast<uint32_t>(USER_Bits::USER), user_op);
        // Wait until ready
        state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);

        if (state != FMC_State::TIMEOUT) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
        }
    }

    return state;
}

uint8_t OB::get_ob_user()
{
    uint32_t value = read_register<uint32_t>(FMC_Regs::OBSTAT);
    value = (value >> 2);
    return static_cast<uint8_t>(value);
}

FMC_State OB::set_ob_data(uint32_t address, uint8_t data)
{
    FMC_State state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);

    if (state == FMC_State::READY) {
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Set);
        (*(volatile uint16_t *)(uint32_t)(address)) = data;
        // Wait until ready
        state = ob_ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state != FMC_State::TIMEOUT) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::OBPG), Clear);
        }
    }

    return state;
}

uint16_t OB::get_ob_data()
{
    uint32_t value = read_register<uint32_t>(FMC_Regs::OBSTAT);
    value = (value >> 10);
    return static_cast<uint16_t>(value);
}

FMC_State OB::ob_get_bank0_state()
{
    FMC_State state = FMC_State::READY;

    if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY))) {
        state = FMC_State::BUSY;
    } else if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR))) {
        state = FMC_State::WP_ERROR;
    } else if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR))) {
        state = FMC_State::PG_ERROR;
    }

    return state;
}

FMC_State OB::ob_get_bank1_state()
{
    FMC_State state = FMC_State::READY;

    if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY))) {
        state = FMC_State::BUSY;
    } else if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR))) {
        state = FMC_State::WP_ERROR;
    } else if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR))) {
        state = FMC_State::PG_ERROR;
    }

    return state;
}

FMC_State OB::ob_ready_wait_bank0(uint32_t timeout)
{
    FMC_State state = FMC_State::BUSY;

    // Wait until ready
    do {
        state = ob_get_bank0_state();
        timeout--;
    } while ((state == FMC_State::BUSY) && (timeout != Clear));

    if (state == FMC_State::BUSY) {
        state = FMC_State::TIMEOUT;
    }

    return state;
}

FMC_State OB::ob_ready_wait_bank1(uint32_t timeout)
{
    FMC_State state = FMC_State::BUSY;

    // Wait until ready
    do {
        state = ob_get_bank1_state();
        timeout--;
    } while ((state == FMC_State::BUSY) && (timeout != Clear));

    if (state == FMC_State::BUSY) {
        state = FMC_State::TIMEOUT;
    }

    return state;
}

} // namespace fmc

// Instantiate class of global device
fmc::OB OB_DEVICE;
