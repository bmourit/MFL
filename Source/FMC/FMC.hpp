// gd32f30x FMC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#include "RegRW.hpp"
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
    FMC_Error_Type mass_erase();
    FMC_Error_Type erase_page(uint32_t address);
    FMC_Error_Type erase_bank0();
    FMC_Error_Type erase_bank1();
    // Program
    FMC_Error_Type program_word(uint32_t address, uint32_t data);
    FMC_Error_Type program_halfword(uint32_t address, uint16_t data);
    FMC_Error_Type reprogram_word(uint32_t address, uint32_t data);
    // Delay
    void set_wait_state(Wait_State wait);
    // State
    FMC_Error_Type get_bank0_state();
    FMC_Error_Type get_bank1_state();
    // Wait
    FMC_Error_Type ready_wait_bank0(uint32_t timeout);
    FMC_Error_Type ready_wait_bank1(uint32_t timeout);
    // Flags
    bool get_flag(Status_Flags flag);
    void clear_flag(Status_Flags flag);
    // Interruppt flags
    bool get_interrupt_flag(Interrupt_Flags flag);
    void clear_interrupt_flag(Interrupt_Flags flag);
    // Interrupts
    void set_interrupt_enable(Interrupt_Types type, bool enable);

    // Base address
    static constexpr uintptr_t FMC_baseAddress = 0x40022000;

    // Register address
    inline volatile uint32_t *reg_address(FMC_Regs reg) const {
        return reinterpret_cast<volatile uint32_t *>(FMC_baseAddress + static_cast<uint32_t>(reg));
    }

    // Function to keep compiler happy
    inline void ensure_clock_enabled() const {}

private:
    template<typename T>
    FMC_Error_Type program_word_to_bank(uint32_t address, uint32_t data,
                                        uint32_t timeout, FMC_Regs control_reg, T program_bit);

    template<typename T>
    FMC_Error_Type program_halfword_to_bank(uint32_t address, uint16_t data,
                                            uint32_t timeout, FMC_Regs control_reg, T program_bit);

    template<typename T>
    FMC_Error_Type erase_word_bank(uint32_t address, uint32_t timeout,
                                   FMC_Regs control_reg, T erase_bit, T start_bit, FMC_Regs address_reg);

    inline uint16_t get_FMC_size() {
        return *reinterpret_cast<const uint16_t*>(Flash_Size_Addess);
    }

    bool get_value(Interrupt_Flags flag) const {
        const auto& info = interrupt_flag_index[static_cast<int>(flag)];
        uint32_t flag_value = read_register<uint32_t>(*this, info.flag_register_offset);
        uint32_t interrupt_value = read_register<uint32_t>(*this, info.interrupt_register_offset);

        const uint32_t flag_width = info.flag_bit_info & 0xFF;
        const uint32_t flag_bitno = info.flag_bit_info >> 16;
        const uint32_t interrupt_width = info.interrupt_bit_info & 0xFF;
        const uint32_t interrupt_bitno = info.interrupt_bit_info >> 16;

        flag_value >>= flag_bitno;
        flag_value &= ((1 << flag_width) - 1);
        interrupt_value >>= interrupt_bitno;
        interrupt_value &= ((1 << interrupt_width) - 1);

        return (flag_value && interrupt_value);
    }

    bool get_value(Status_Flags flag) const {
        const auto& info = status_flag_index[static_cast<int>(flag)];
        uint32_t flag_value = read_register<uint32_t>(*this, info.register_offset);

        const uint32_t flag_width = info.bit_info & 0xFF;
        const uint32_t flag_bitno = info.bit_info >> 16;

        flag_value >>= flag_bitno;
        flag_value &= ((1 << flag_width) - 1);

        return flag_value;
    }

    void set_value(Interrupt_Flags flag, uint32_t value) {
        const auto& info = interrupt_flag_index[static_cast<int>(flag)];
        uint32_t flag_value = read_register<uint32_t>(*this, info.flag_register_offset);
        uint32_t interrupt_value = read_register<uint32_t>(*this, info.interrupt_register_offset);

        const uint32_t flag_width = info.flag_bit_info & 0xFF;
        const uint32_t flag_bitno = info.flag_bit_info >> 16;
        const uint32_t interrupt_width = info.interrupt_bit_info & 0xFF;
        const uint32_t interrupt_bitno = info.interrupt_bit_info >> 16;

        flag_value &= ~(((1 << flag_width) - 1) << flag_bitno);
        flag_value |= value << flag_bitno;
        interrupt_value &= ~(((1 << interrupt_width) - 1) << interrupt_bitno);
        interrupt_value |= value << interrupt_bitno;

        write_register(*this, info.flag_register_offset, flag_value);
        write_register(*this, info.interrupt_register_offset, interrupt_value);
    }

    void set_value(Status_Flags flag, uint32_t value) {
        const auto& info = status_flag_index[static_cast<int>(flag)];
        uint32_t flag_value = read_register<uint32_t>(*this, info.register_offset);

        const uint32_t flag_width = info.bit_info & 0xFF;
        const uint32_t flag_bitno = info.bit_info >> 16;

        flag_value &= ~(((1 << flag_width) - 1) << flag_bitno);
        flag_value |= value << flag_bitno;

        write_register(*this, info.register_offset, flag_value);
    }

    void set_value(Interrupt_Types type, uint32_t value) {
        const auto& info = interrupt_type_index[static_cast<int>(type)];
        uint32_t type_value = read_register<uint32_t>(*this, info.register_offset);

        const uint32_t type_width = info.bit_info & 0xFF;
        const uint32_t type_bitno = info.bit_info >> 16;

        type_value &= ~(((1 << type_width) - 1) << type_bitno);
        type_value |= value << type_bitno;

        write_register(*this, info.register_offset, type_value);
    }
};

} // namespace fmc

extern fmc::FMC FMC_DEVICE;
