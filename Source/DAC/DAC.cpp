// gd32f30x DAC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "DAC.hpp"

// Initialize the static member
bool dac::DAC::is_clock_enabled = false;

namespace dac {

void DAC::enable(Internal_Device instance) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DEN0), Set, true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DEN1), Set, true);
    }
}

void DAC::disable(Internal_Device instance) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DEN0), Clear, true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DEN1), Clear, true);
    }
}

void DAC::set_dma_enable(Internal_Device instance, bool enable) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DDMAEN0), enable ? Set : Clear, true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DDMAEN1), enable ? Set : Clear, true);
    }
}

void DAC::set_output_buffer_enable(Internal_Device instance, bool enable) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DBOFF0), enable ? Clear : Set, true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DBOFF1), enable ? Clear : Set, true);
    }
}

uint16_t DAC::get_output_value(Internal_Device instance) {
    if (instance == Internal_Device::DAC0) {
        return read_bit16(*this, DAC_Regs::DAC0_DO, static_cast<uint32_t>(DAC0_DO_Bits::DAC0_DO), true);
    } else {
        return read_bit16(*this, DAC_Regs::DAC1_DO, static_cast<uint32_t>(DAC1_DO_Bits::DAC1_DO), true);
    }
    return 0;
}

void DAC::set_data(Internal_Device instance, Data_Align align, uint16_t data)
{
    if (instance == Internal_Device::DAC0) {
        switch (align) {
        case Data_Align::RIGHT_12B:
            write_bit(*this, DAC_Regs::DAC0_R12DH, static_cast<uint32_t>(DAC0_R12DH_Bits::DAC0_DH), static_cast<uint32_t>(data), true);
            break;
        case Data_Align::LEFT_12B:
            write_bit(*this, DAC_Regs::DAC0_L12DH, static_cast<uint32_t>(DAC0_L12DH_Bits::DAC0_DH), static_cast<uint32_t>(data), true);
            break;
        case Data_Align::RIGHT_8B:
            write_bit(*this, DAC_Regs::DAC0_R8DH, static_cast<uint32_t>(DAC0_R8DH_Bits::DAC0_DH), static_cast<uint32_t>(data), true);
            break;
        default:
            break;
        }
    } else {
        switch (align) {
        case Data_Align::RIGHT_12B:
            write_bit(*this, DAC_Regs::DAC1_R12DH,static_cast<uint32_t>(DAC1_R12DH_Bits::DAC1_DH), static_cast<uint32_t>(data), true);
            break;
        case Data_Align::LEFT_12B:
            write_bit(*this, DAC_Regs::DAC1_L12DH, static_cast<uint32_t>(DAC1_L12DH_Bits::DAC1_DH), static_cast<uint32_t>(data), true);
            break;
        case Data_Align::RIGHT_8B:
            write_bit(*this, DAC_Regs::DAC1_R8DH, static_cast<uint32_t>(DAC1_R8DH_Bits::DAC1_DH), static_cast<uint32_t>(data), true);
            break;
        default:
            break;
        }
    }
}

void DAC::set_trigger_enable(Internal_Device instance, bool enable)
{
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DTEN0), enable ? Set : Clear, true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DTEN1), enable ? Set : Clear, true);
    }
}

void DAC::set_trigger_source(Internal_Device instance, Trigger_Source source) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DTSEL0), static_cast<uint32_t>(source), true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DTSEL1), static_cast<uint32_t>(source), true);
    }
}

void DAC::set_software_trigger_enable(Internal_Device instance, bool enable) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::SWT, static_cast<uint32_t>(SWT_Bits::SWTR0), enable ? Set : Clear, true);
    } else {
        write_bit(*this, DAC_Regs::SWT, static_cast<uint32_t>(SWT_Bits::SWTR1), enable ? Set : Clear, true);
    }
}

void DAC::set_wave_mode(Internal_Device instance, Wave_Type type) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWM0), static_cast<uint32_t>(type), true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWM1), static_cast<uint32_t>(type), true);
    }
}

void DAC::set_wave_bit_width(Internal_Device instance, Bit_Width width) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW0), static_cast<uint32_t>(width), true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW1), static_cast<uint32_t>(width), true);
    }
}

void DAC::set_lfsr_noise(Internal_Device instance, LFSR_Noise noise) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW0), static_cast<uint32_t>(noise), true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW1), static_cast<uint32_t>(noise), true);
    }
}

void DAC::set_triangle_amplitude(Internal_Device instance, Triangle_Amplitude amplitude) {
    if (instance == Internal_Device::DAC0) {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW0), static_cast<uint32_t>(amplitude), true);
    } else {
        write_bit(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DWBW1), static_cast<uint32_t>(amplitude), true);
    }
}

void DAC::set_dual_mode_enable(bool enable) {
    write_bits(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DEN0), enable ? Set : Clear,
               static_cast<uint32_t>(CTL_Bits::DEN1), enable ? Set : Clear);
}

void DAC::set_dual_software_trigger_enable(bool enable)
{
    write_bits(*this, DAC_Regs::SWT, static_cast<uint32_t>(SWT_Bits::SWTR0), enable ? Set : Clear,
               static_cast<uint32_t>(SWT_Bits::SWTR1), enable ? Set : Clear);
}

void DAC::set_dual_output_buffer_enable(bool enable)
{
    write_bits(*this, DAC_Regs::CTL, static_cast<uint32_t>(CTL_Bits::DBOFF0), enable ? Clear : Set,
               static_cast<uint32_t>(CTL_Bits::DBOFF1), enable ? Clear : Set);
}

void DAC::set_dual_data(Data_Align align, uint16_t data0, uint16_t data1)
{
    switch (align) {
    case Data_Align::RIGHT_12B:
        write_bits(*this, DAC_Regs::DACC_R12DH, static_cast<uint32_t>(DACC_R12DH_Bits::DAC0_DH), static_cast<uint32_t>(data0),
                   static_cast<uint32_t>(DACC_R12DH_Bits::DAC1_DH), static_cast<uint32_t>(data1));
        break;
    case Data_Align::LEFT_12B:
        write_bits(*this, DAC_Regs::DACC_L12DH, static_cast<uint32_t>(DACC_L12DH_Bits::DAC0_DH), static_cast<uint32_t>(data0),
                   static_cast<uint32_t>(DACC_L12DH_Bits::DAC1_DH), static_cast<uint32_t>(data1));
        break;
    case Data_Align::RIGHT_8B:
        write_bits(*this, DAC_Regs::DACC_R8DH, static_cast<uint32_t>(DACC_R8DH_Bits::DAC0_DH), static_cast<uint32_t>(data0),
                   static_cast<uint32_t>(DACC_R8DH_Bits::DAC1_DH), static_cast<uint32_t>(data1));
        break;
    default:
        break;
    }
}

} // namespace dac

dac::DAC DAC_DEVICE;
