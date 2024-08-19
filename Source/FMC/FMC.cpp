// gd32f30x FMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "FMC.hpp"

namespace fmc {

void FMC::unlock()
{
    if (read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK))) {
        write_register(FMC_Regs::KEY0, UNLOCK_KEY0);
        write_register(FMC_Regs::KEY0, UNLOCK_KEY1);
    }
    if (BANK0_SIZE < *MAX_SIZE) {
        if (read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK))) {
            write_register(FMC_Regs::KEY1, UNLOCK_KEY0);
            write_register(FMC_Regs::KEY1, UNLOCK_KEY1);
        }
    }
}

void FMC::unlock_bank0()
{
    if (read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK))) {
        write_register(FMC_Regs::KEY0, UNLOCK_KEY0);
        write_register(FMC_Regs::KEY0, UNLOCK_KEY1);
    }
}

void FMC::unlock_bank1()
{
    if (read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK))) {
        write_register(FMC_Regs::KEY1, UNLOCK_KEY0);
        write_register(FMC_Regs::KEY1, UNLOCK_KEY1);
    }
}

void FMC::lock(void)
{
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), 1);

    if (BANK0_SIZE < *MAX_SIZE) {
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), 1);
    }
}

void FMC::lock_bank0(void)
{
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), 1);
}

void FMC::lock_bank1(void)
{
    write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), 1);
}

FMC_State FMC::mass_erase()
{
    FMC_State state;
    if (BANK0_SIZE < *MAX_SIZE) {
        // Wait until ready
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 1);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), 1);
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state != FMC_State::READY) {
                return state;
            }
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 0);
        }
        state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), 1);
            write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::START), 1);
            // Wait until ready
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), 0);
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 1);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), 1);
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 0);
        }
    }

    return state;
}

FMC_State FMC::erase_page(uint32_t address)
{
    FMC_State state;

    if (BANK0_SIZE < *MAX_SIZE) {
        if (address < BANK0_END_ADDRESS) {
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), 1);
                write_register(FMC_Regs::ADDR0, address);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), 1);
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), 0);
            }
        } else {
            // Wait until ready
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PER), 1);
                write_register(FMC_Regs::ADDR1, address);
                if (read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::SPC))) {
                    write_register(FMC_Regs::ADDR0, address);
                }
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::START), 1);
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>((CTL1_Bits::PER)), 0);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), 1);
            write_register(FMC_Regs::ADDR0, address);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), 1);
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), 0);
        }
    }

    return state;
}

FMC_State FMC::erase_bank0()
{
    FMC_State state = FMC_State::READY;

    state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
    if (state == FMC_State::READY) {
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 1);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), 1);
        // Wait until ready
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), 0);
    }

    return state;
}

FMC_State FMC::erase_bank1()
{
    FMC_State state = FMC_State::READY;

    state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
    if (state == FMC_State::READY) {
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), 1);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::START), 1);
        // Wait until ready
        state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), 0);
    }

    return state;
}

FMC_State FMC::program_word(uint32_t address, uint32_t data)
{
    FMC_State state = FMC_State::READY;

    if (BANK0_SIZE < *MAX_SIZE) {
        if (address < BANK0_END_ADDRESS) {
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 1);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 0);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
            (*(volatile uint32_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
        }
    }

    return state;
}

FMC_State FMC::program_halfword(uint32_t address, uint16_t data)
{
    FMC_State state = FMC_State::READY;

    if (BANK0_SIZE < *MAX_SIZE) {
        if (address < BANK0_END_ADDRESS) {
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
                (*(volatile uint16_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 1);
                (*(volatile uint16_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 0);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
            (*(volatile uint16_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
        }
    }

    return state;
}

FMC_State FMC::reprogram_word(uint32_t address, uint32_t data)
{
    FMC_State state = FMC_State::READY;

    if (BANK0_SIZE < *MAX_SIZE) {
        if (address < BANK0_END_ADDRESS) {
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), 1);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), 1);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 1);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), 0);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), 1);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 1);
            (*(volatile uint32_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), 0);
        }
    }

    return state;
}

void FMC::set_delay_count(Wait_State wait)
{
    write_bit(*this, FMC_Regs::WS, static_cast<uint32_t>(WS_Bits::WSCNT), static_cast<uint32_t>(wait));
}

FMC_State FMC::get_bank0_state()
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

FMC_State FMC::get_bank1_state()
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

FMC_State FMC::ready_wait_bank0(uint32_t timeout)
{
    FMC_State state = FMC_State::BUSY;

    // Wait until ready
    do {
        state = get_bank0_state();
        timeout--;
    } while ((state == FMC_State::BUSY) && (timeout != 0));

    if (state == FMC_State::BUSY) {
        state = FMC_State::TIMEOUT;
    }

    return state;
}

FMC_State FMC::ready_wait_bank1(uint32_t timeout)
{
    FMC_State state = FMC_State::BUSY;

    // Wait until ready
    do {
        state = get_bank1_state();
        timeout--;
    } while ((state == FMC_State::BUSY) && (timeout != 0));

    if (state == FMC_State::BUSY) {
        state = FMC_State::TIMEOUT;
    }

    return state;
}

bool FMC::get_flag(Flag_Types flag)
{
    uint32_t value = 0;

    switch (flag) {
    case Flag_Types::FLAG_BANK0_BUSY:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY));
        break;
    case Flag_Types::FLAG_BANK0_PGERR:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR));
        break;
    case Flag_Types::FLAG_BANK0_WPERR:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR));
        break;
    case Flag_Types::FLAG_BANK0_END:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF));
        break;
    case Flag_Types::FLAG_OBERR:
        value = read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::OBERR));
        break;
    case Flag_Types::FLAG_BANK1_BUSY:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY));
        break;
    case Flag_Types::FLAG_BANK1_PGERR:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR));
        break;
    case Flag_Types::FLAG_BANK1_WPERR:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR));
        break;
    case Flag_Types::FLAG_BANK1_END:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF));
        break;
    default:
        break;
    }

    return (value != 0) ? true : false;

}

void FMC::clear_flag(Flag_Types flag)
{
    switch (flag) {
    case Flag_Types::FLAG_BANK0_BUSY:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY), 1);
        break;
    case Flag_Types::FLAG_BANK0_PGERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR), 1);
        break;
    case Flag_Types::FLAG_BANK0_WPERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR), 1);
        break;
    case Flag_Types::FLAG_BANK0_END:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF), 1);
        break;
    case Flag_Types::FLAG_OBERR:
        write_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::OBERR), 1);
        break;
    case Flag_Types::FLAG_BANK1_BUSY:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY), 1);
        break;
    case Flag_Types::FLAG_BANK1_PGERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR), 1);
        break;
    case Flag_Types::FLAG_BANK1_WPERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR), 1);
        break;
    case Flag_Types::FLAG_BANK1_END:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF), 1);
        break;
    default:
        break;
    }
}

bool FMC::get_interrupt_flag(Interrupt_Flags flag)
{
    uint32_t value1 = 0;
    uint32_t value2 = 0;

    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_BANK0_PGERR:
        value1 = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR));
        value2 = read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_WPERR:
        value1 = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR));
        value2 = read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_END:
        value1 = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF));
        value2 = read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE));
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_PGERR:
        value1 = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR));
        value2 = read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_WPERR:
        value1 = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR));
        value2 = read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE));
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_END:
        value1 = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF));
        value2 = read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE));
        break;
    default:
        break;
    }

    return (value1 && value2) ? true : false;

}

void FMC::clear_interrupt_flag(Interrupt_Flags flag)
{
    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_BANK0_PGERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR), 1);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), 1);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_WPERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR), 1);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), 1);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_END:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF), 1);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE), 1);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_PGERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR), 1);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 1);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_WPERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR), 1);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 1);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_END:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF), 1);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE), 1);
        break;
    }
}

void FMC::interrupt_enable(Interrupt_Types type)
{
    switch (type) {
    case Interrupt_Types::INTR_BANK0_END:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE), 1);
        break;
    case Interrupt_Types::INTR_BANK0_ERR:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), 1);
        break;
    case Interrupt_Types::INTR_BANK1_END:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE), 1);
        break;
    case Interrupt_Types::INTR_BANK1_ERR:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 1);
        break;
    default:
        break;
    }
}

void FMC::interrupt_disable(Interrupt_Types type)
{
    switch (type) {
    case Interrupt_Types::INTR_BANK0_END:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE), 0);
        break;
    case Interrupt_Types::INTR_BANK0_ERR:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), 0);
        break;
    case Interrupt_Types::INTR_BANK1_END:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE), 0);
        break;
    case Interrupt_Types::INTR_BANK1_ERR:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), 0);
        break;
    default:
        break;
    }
}

} // namespace fmc

// Instantiate class of global device
fmc::FMC FMC_DEVICE;
