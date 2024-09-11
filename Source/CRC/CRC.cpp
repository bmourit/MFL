// gd32f30x CRC functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "CRC.hpp"

// Initialize the static member
bool crc::CRC::is_clock_enabled = false;

namespace crc {

constexpr uint32_t DataResetValue = 0xFFFFFFFF;
constexpr uint8_t FreeDataResetValue = 0;

void CRC::reset() {
    write_register(*this, CRC_Regs::DATA, DataResetValue, true);
    write_bit(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA), FreeDataResetValue, true);
    write_bit(*this, CRC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RST), Set, true);
}

void CRC::reset_data() {
    write_bit(*this, CRC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RST), Set, true);
}

uint32_t CRC::get_data() {
    return read_register<uint32_t>(*this, CRC_Regs::DATA, true);
}

uint8_t CRC::get_free_data(void) {
    return read_bit8(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA), true);
}

void CRC::set_free_data(uint8_t data) {
    write_bit(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA), static_cast<uint32_t>(data), true);
}

uint32_t CRC::calculate_data(uint32_t data) {
    write_register(*this, CRC_Regs::DATA, data, true);
    return read_register<uint32_t>(*this, CRC_Regs::DATA, true);
}

uint32_t CRC::calculate_mulitple_data(const uint32_t *data, uint32_t size) {
    for (auto i = 0U; i < size; ++i) {
        write_register(*this, CRC_Regs::DATA, data[i], true);
    }
    return read_register<uint32_t>(*this, CRC_Regs::DATA, true);
}

} // namespace crc

crc::CRC CRC_DEVICE;
