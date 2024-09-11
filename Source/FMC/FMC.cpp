// gd32f30x FMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "FMC.hpp"

namespace fmc {

void FMC::unlock()
{
    if (read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK))) {
        write_register(*this, FMC_Regs::KEY0, Unlock_Key0);
        write_register(*this, FMC_Regs::KEY0, Unlock_Key1);
    }
    if (get_FMC_size() > Bank0_Size) {
        if (read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK))) {
            write_register(*this, FMC_Regs::KEY1, Unlock_Key0);
            write_register(*this, FMC_Regs::KEY1, Unlock_Key1);
        }
    }
}

void FMC::unlock_bank0() {
    if (read_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK))) {
        write_register(*this, FMC_Regs::KEY0, Unlock_Key0);
        write_register(*this, FMC_Regs::KEY0, Unlock_Key1);
    }
}

void FMC::unlock_bank1() {
    if (read_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK))) {
        write_register(*this, FMC_Regs::KEY1, Unlock_Key0);
        write_register(*this, FMC_Regs::KEY1, Unlock_Key1);
    }
}

void FMC::lock(void) {
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), Set);

    if (get_FMC_size() > Bank0_Size) {
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), Set);
    }
}

void FMC::lock_bank0(void) {
    write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::LK), Set);
}

void FMC::lock_bank1(void) {
    write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::LK), Set);
}

FMC_Error_Type FMC::mass_erase() {
    FMC_Error_Type state = ready_wait_bank0(Timeout_Count);

    if (state == FMC_Error_Type::READY) {
        write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Set,
                   static_cast<uint32_t>(CTL0_Bits::START), Set);
        // Wait until ready
        state = ready_wait_bank0(Timeout_Count);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Clear);
    }

    if (get_FMC_size() > Bank0_Size) {
        state = ready_wait_bank1(Timeout_Count);
        if (state == FMC_Error_Type::READY) {
            write_bits(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Set,
                       static_cast<uint32_t>(CTL1_Bits::START), Set);
            // Wait until ready
            state = ready_wait_bank1(Timeout_Count);
            write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Clear);
        }
    }

    return state;
}

FMC_Error_Type FMC::erase_page(uint32_t address) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    // Determine which bank to use based on the address and FMC size
    bool is_bank0 = (get_FMC_size() <= Bank0_Size || address < Bank0_End_Address);

    if (is_bank0) {
        state = erase_word_bank(address, Timeout_Count, FMC_Regs::CTL0, CTL0_Bits::PER,
                                CTL0_Bits::START, FMC_Regs::ADDR0);
    } else {
        state = erase_word_bank(address, Timeout_Count, FMC_Regs::CTL1, CTL1_Bits::PER,
                                CTL1_Bits::START, FMC_Regs::ADDR1);
    }

    return state;
}

FMC_Error_Type FMC::erase_bank0() {
    FMC_Error_Type state = FMC_Error_Type::READY;

    state = ready_wait_bank0(Timeout_Count);
    if (state == FMC_Error_Type::READY) {
        write_bits(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Set,
                   static_cast<uint32_t>(CTL0_Bits::START), Set);
        // Wait until ready
        state = ready_wait_bank0(Timeout_Count);
        write_bit(*this, FMC_Regs::CTL0, static_cast<uint32_t>(CTL0_Bits::MER), Clear);
    }

    return state;
}

FMC_Error_Type FMC::erase_bank1() {
    FMC_Error_Type state = FMC_Error_Type::READY;

    state = ready_wait_bank1(Timeout_Count);
    if (state == FMC_Error_Type::READY) {
        write_bits(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Set,
                   static_cast<uint32_t>(CTL1_Bits::START), Set);
        // Wait until ready
        state = ready_wait_bank1(Timeout_Count);
        write_bit(*this, FMC_Regs::CTL1, static_cast<uint32_t>(CTL1_Bits::MER), Clear);
    }

    return state;
}

FMC_Error_Type FMC::program_word(uint32_t address, uint32_t data) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    // Determine which bank to use based on the address and FMC size
    bool is_bank0 = (get_FMC_size() <= Bank0_Size || address < Bank0_End_Address);

    if (is_bank0) {
        state = program_word_to_bank(address, data, Timeout_Count,
                                     FMC_Regs::CTL0, CTL0_Bits::PG);
    } else {
        state = program_word_to_bank(address, data, Timeout_Count,
                                     FMC_Regs::CTL1, CTL1_Bits::PG);
    }

    return state;
}

FMC_Error_Type FMC::program_halfword(uint32_t address, uint16_t data) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    // Determine which bank to use based on the address and FMC size
    bool is_bank0 = (get_FMC_size() <= Bank0_Size || address < Bank0_End_Address);

    if (is_bank0) {
        state = program_halfword_to_bank(address, data, Timeout_Count,
                                         FMC_Regs::CTL0, CTL0_Bits::PG);
    } else {
        state = program_halfword_to_bank(address, data, Timeout_Count,
                                         FMC_Regs::CTL1, CTL1_Bits::PG);
    }

    return state;
}

FMC_Error_Type FMC::reprogram_word(uint32_t address, uint32_t data) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    // Determine which bank to use based on the address and FMC size
    bool is_bank0 = (get_FMC_size() <= Bank0_Size || address < Bank0_End_Address);

    if (is_bank0) {
        write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), Set);
        state = program_word_to_bank(address, data, Timeout_Count,
                                     FMC_Regs::CTL0, CTL0_Bits::PG);
    } else {
        write_bit(*this, FMC_Regs::WSEN, static_cast<uint32_t>(WSEN_Bits::BPEN), Set);
        state = program_word_to_bank(address, data, Timeout_Count,
                                     FMC_Regs::CTL1, CTL1_Bits::PG);
    }

    return state;
}

void FMC::set_wait_state(Wait_State wait) {
    write_bit(*this, FMC_Regs::WS, static_cast<uint32_t>(WS_Bits::WSCNT), static_cast<uint32_t>(wait));
}

FMC_Error_Type FMC::get_bank0_state() {
    FMC_Error_Type state = FMC_Error_Type::READY;

    if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::BUSY))) {
        state = FMC_Error_Type::BUSY;
    } else if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::WPERR))) {
        state = FMC_Error_Type::WP_ERROR;
    } else if (read_bit(*this, FMC_Regs::STAT0, static_cast<uint32_t>(STAT0_Bits::PGERR))) {
        state = FMC_Error_Type::PG_ERROR;
    }

    return state;
}

FMC_Error_Type FMC::get_bank1_state() {
    FMC_Error_Type state = FMC_Error_Type::READY;

    if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::BUSY))) {
        state = FMC_Error_Type::BUSY;
    } else if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::WPERR))) {
        state = FMC_Error_Type::WP_ERROR;
    } else if (read_bit(*this, FMC_Regs::STAT1, static_cast<uint32_t>(STAT1_Bits::PGERR))) {
        state = FMC_Error_Type::PG_ERROR;
    }

    return state;
}

FMC_Error_Type FMC::ready_wait_bank0(uint32_t timeout) {
    FMC_Error_Type state = FMC_Error_Type::BUSY;

    do {
        state = get_bank0_state();
        timeout--;
    } while ((state == FMC_Error_Type::BUSY) && (timeout != 0));

    if (state == FMC_Error_Type::BUSY) {
        state = FMC_Error_Type::TIMEOUT;
    }

    return state;
}

FMC_Error_Type FMC::ready_wait_bank1(uint32_t timeout) {
    FMC_Error_Type state = FMC_Error_Type::BUSY;

    do {
        state = get_bank1_state();
        timeout--;
    } while ((state == FMC_Error_Type::BUSY) && (timeout != 0));

    if (state == FMC_Error_Type::BUSY) {
        state = FMC_Error_Type::TIMEOUT;
    }

    return state;
}

bool FMC::get_flag(Status_Flags flag) {
    return get_value(flag);
}

void FMC::clear_flag(Status_Flags flag) {
    set_value(flag, Set);
}

bool FMC::get_interrupt_flag(Interrupt_Flags flag) {
    return get_value(flag);
}

void FMC::clear_interrupt_flag(Interrupt_Flags flag) {
    set_value(flag, Set);
}

void FMC::set_interrupt_enable(Interrupt_Types type, bool enable) {
    set_value(type, enable ? Set : Clear);
}

template<typename T>
FMC_Error_Type FMC::program_word_to_bank(uint32_t address, uint32_t data,
        uint32_t timeout, FMC_Regs control_reg, T program_bit) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    if (control_reg == FMC_Regs::CTL0) {
        state = ready_wait_bank0(timeout);
    } else {
        state = ready_wait_bank1(timeout);
    }

    if (state == FMC_Error_Type::READY) {
        // Set the programming bit
        write_bit(*this, control_reg, static_cast<uint32_t>(program_bit), Set);

        // Write the data to the specified address
        *reinterpret_cast<volatile uint32_t*>(address) = data;

        // Wait until programming completes
        if (control_reg == FMC_Regs::CTL0) {
            state = ready_wait_bank0(timeout);
        } else {
            state = ready_wait_bank1(timeout);
        }

        // Clear the programming bit
        write_bit(*this, control_reg, static_cast<uint32_t>(program_bit), Clear);
    }

    return state;
}

template<typename T>
FMC_Error_Type FMC::program_halfword_to_bank(uint32_t address, uint16_t data,
        uint32_t timeout, FMC_Regs control_reg, T program_bit) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    if (control_reg == FMC_Regs::CTL0) {
        state = ready_wait_bank0(timeout);
    } else {
        state = ready_wait_bank1(timeout);
    }

    if (state == FMC_Error_Type::READY) {
        // Set the programming bit
        write_bit(*this, control_reg, static_cast<uint32_t>(program_bit), Set);

        // Write the data to the specified address
        *reinterpret_cast<volatile uint16_t*>(address) = data;

        // Wait until programming completes
        if (control_reg == FMC_Regs::CTL0) {
            state = ready_wait_bank0(timeout);
        } else {
            state = ready_wait_bank1(timeout);
        }

        // Clear the programming bit
        write_bit(*this, control_reg, static_cast<uint32_t>(program_bit), Clear);
    }

    return state;
}

template<typename T>
FMC_Error_Type FMC::erase_word_bank(uint32_t address, uint32_t timeout,
                                    FMC_Regs control_reg, T erase_bit, T start_bit, FMC_Regs address_reg) {
    FMC_Error_Type state = FMC_Error_Type::READY;

    if (control_reg == FMC_Regs::CTL0) {
        state = ready_wait_bank0(timeout);
    } else {
        state = ready_wait_bank1(timeout);
    }

    if (state == FMC_Error_Type::READY) {
        // Set the programming bit
        write_bit(*this, control_reg, static_cast<uint32_t>(erase_bit), Set);
        write_register(*this, address_reg, address);
        if (control_reg == FMC_Regs::CTL1) {
            if (read_bit(*this, FMC_Regs::OBSTAT, static_cast<uint32_t>(OBSTAT_Bits::SPC))) {
                write_register(*this, FMC_Regs::ADDR0, address);
            }
        }
        // Write the start bit
        write_bit(*this, control_reg, static_cast<uint32_t>(start_bit), Set);

        // Wait until erase completes
        if (control_reg == FMC_Regs::CTL0) {
            state = ready_wait_bank0(timeout);
        } else {
            state = ready_wait_bank1(timeout);
        }

        // Clear the erase bit
        write_bit(*this, control_reg, static_cast<uint32_t>(erase_bit), Clear);
    }

    return state;
}

} // namespace fmc

// Instantiate class of global device
fmc::FMC FMC_DEVICE;
