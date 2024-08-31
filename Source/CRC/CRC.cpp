// gd32f30x CRC functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "CRC.hpp"

namespace crc {

constexpr uint32_t DataResetValue = 0xFFFFFFFF;
constexpr uint8_t FreeDataResetValue = 0;

void CRC::reset()
{
    write_register(CRC_Regs::DATA, DataResetValue);
    write_bit(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA), FreeDataResetValue);
    write_bit(*this, CRC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RST), Set);
}

void CRC::reset_data()
{
    write_bit(*this, CRC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::RST), Set);
}

uint32_t CRC::get_data()
{
    return read_register<uint32_t>(CRC_Regs::DATA);
}

uint8_t CRC::get_free_data(void)
{
    return read_bit8(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA));
}

void CRC::set_free_data(uint8_t data)
{
    write_bit(*this, CRC_Regs::FDATA, static_cast<uint32_t>(FDATA_Bits::FDATA), static_cast<uint32_t>(data));
}

uint32_t CRC::calculate_data(uint32_t data)
{
    write_register(CRC_Regs::DATA, data);
    return read_register<uint32_t>(CRC_Regs::DATA);
}

uint32_t CRC::calculate_mulitple_data(const uint32_t *data, uint32_t size)
{
    for (auto i = 0U; i < size; ++i) {
        write_register(CRC_Regs::DATA, data[i]);
    }
    return read_register<uint32_t>(CRC_Regs::DATA);
}

} // namespace crc
