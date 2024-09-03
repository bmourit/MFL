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
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), Set);

    if (BANK0_SIZE < *MAX_SIZE) {
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), Set);
    }
}

void FMC::lock_bank0(void)
{
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), Set);
}

void FMC::lock_bank1(void)
{
    write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), Set);
}

FMC_State FMC::mass_erase()
{
    FMC_State state;
    if (BANK0_SIZE < *MAX_SIZE) {
        // Wait until ready
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Set,
                       static_cast<uint32_t>(CTL0_Bits::START), Set);
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            if (state != FMC_State::READY) {
                return state;
            }
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Clear);
        }
        state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bits(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Set,
                       static_cast<uint32_t>(CTL1_Bits::START), Set);
            // Wait until ready
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Clear);
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Set,
                       static_cast<uint32_t>(CTL0_Bits::START), Set);
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Clear);
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
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), Set);
                write_register(FMC_Regs::ADDR0, address);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::START), Set);
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), Clear);
            }
        } else {
            // Wait until ready
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PER), Set);
                write_register(FMC_Regs::ADDR1, address);
                if (read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::SPC))) {
                    write_register(FMC_Regs::ADDR0, address);
                }
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::START), Set);
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>((CTL1_Bits::PER)), Clear);
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
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PER), Clear);
        }
    }

    return state;
}

FMC_State FMC::erase_bank0()
{
    FMC_State state = FMC_State::READY;

    state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
    if (state == FMC_State::READY) {
        write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Set,
                   static_cast<uint32_t>(CTL0_Bits::START), Set);
        // Wait until ready
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Clear);
    }

    return state;
}

FMC_State FMC::erase_bank1()
{
    FMC_State state = FMC_State::READY;

    state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
    if (state == FMC_State::READY) {
        write_bits(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Set,
                   static_cast<uint32_t>(CTL1_Bits::START), Set);
        // Wait until ready
        state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Clear);
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
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Set);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Clear);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
            (*(volatile uint32_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
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
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
                (*(volatile uint16_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Set);
                (*(volatile uint16_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Clear);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
            (*(volatile uint16_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
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
            write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), Set);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
            }
        } else {
            state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), Set);
            if (state == FMC_State::READY) {
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Set);
                (*(volatile uint32_t *)(uint32_t)address) = data;
                // Wait until ready
                state = ready_wait_bank1(FMC_TIMEOUT_COUNT);
                write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::PG), Clear);
            }
        }
    } else {
        state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
        write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), Set);
        if (state == FMC_State::READY) {
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Set);
            (*(volatile uint32_t *)(uint32_t)address) = data;
            // Wait until ready
            state = ready_wait_bank0(FMC_TIMEOUT_COUNT);
            write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::PG), Clear);
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

bool FMC::get_flag(Status_Flags flag)
{
    uint32_t value = 0;

    switch (flag) {
    case Status_Flags::FLAG_BANK0_BUSY:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY));
        break;
    case Status_Flags::FLAG_BANK0_PGERR:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR));
        break;
    case Status_Flags::FLAG_BANK0_WPERR:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR));
        break;
    case Status_Flags::FLAG_BANK0_END:
        value = read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF));
        break;
    case Status_Flags::FLAG_OBERR:
        value = read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::OBERR));
        break;
    case Status_Flags::FLAG_BANK1_BUSY:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY));
        break;
    case Status_Flags::FLAG_BANK1_PGERR:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR));
        break;
    case Status_Flags::FLAG_BANK1_WPERR:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR));
        break;
    case Status_Flags::FLAG_BANK1_END:
        value = read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF));
        break;
    default:
        break;
    }

    return (value != 0);

}

void FMC::clear_flag(Status_Flags flag)
{
    switch (flag) {
    case Status_Flags::FLAG_BANK0_BUSY:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY), Set);
        break;
    case Status_Flags::FLAG_BANK0_PGERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR), Set);
        break;
    case Status_Flags::FLAG_BANK0_WPERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR), Set);
        break;
    case Status_Flags::FLAG_BANK0_END:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF), Set);
        break;
    case Status_Flags::FLAG_OBERR:
        write_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::OBERR), Set);
        break;
    case Status_Flags::FLAG_BANK1_BUSY:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY), Set);
        break;
    case Status_Flags::FLAG_BANK1_PGERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR), Set);
        break;
    case Status_Flags::FLAG_BANK1_WPERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR), Set);
        break;
    case Status_Flags::FLAG_BANK1_END:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF), Set);
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

    return (value1 && value2);

}

void FMC::clear_interrupt_flag(Interrupt_Flags flag)
{
    switch (flag) {
    case Interrupt_Flags::INTR_FLAG_BANK0_PGERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR), Set);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), Set);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_WPERR:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR), Set);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), Set);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK0_END:
        write_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::ENDF), Set);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE), Set);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_PGERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR), Set);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), Set);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_WPERR:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR), Set);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), Set);
        break;
    case Interrupt_Flags::INTR_FLAG_BANK1_END:
        write_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::ENDF), Set);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE), Set);
        break;
    }
}

void FMC::set_interrupt_enable(Interrupt_Types type, bool enable)
{
    switch (type) {
    case Interrupt_Types::INTR_BANK0_END:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ENDIE), enable ? Set : Clear);
        break;
    case Interrupt_Types::INTR_BANK0_ERR:
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::ERRIE), enable ? Set : Clear);
        break;
    case Interrupt_Types::INTR_BANK1_END:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ENDIE), enable ? Set : Clear);
        break;
    case Interrupt_Types::INTR_BANK1_ERR:
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::ERRIE), enable ? Set : Clear);
        break;
    default:
        break;
    }
}

} // namespace fmc

// Instantiate class of global device
fmc::FMC FMC_DEVICE;
