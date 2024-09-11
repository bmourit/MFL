// gd32f30x RCU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "RCU.hpp"

namespace rcu {

constexpr uint32_t CoreClockFrequency = 72'000'000;

void RCU::reset() {
    // Enable IRC8M
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::IRC8MEN), Set);
    while (is_osci_stable(OSCI_Select::IRC8M) == false) {
    }
    // Clear system clk source
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::SCS), Clear);
    // Reset CTL register
    write_bits(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HXTALEN), Clear,
               static_cast<uint32_t>(CTL_Bits::CKMEN), Clear,
               static_cast<uint32_t>(CTL_Bits::PLLEN), Clear,
               static_cast<uint32_t>(CTL_Bits::HXTALBPS), Clear);
    // Reset CFG0 register
    write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::SCS), Clear,
               static_cast<uint32_t>(CFG0_Bits::AHBPSC), Clear,
               static_cast<uint32_t>(CFG0_Bits::APB1PSC), Clear,
               static_cast<uint32_t>(CFG0_Bits::APB2PSC), Clear,
               static_cast<uint32_t>(CFG0_Bits::ADCPSC), Clear,
               static_cast<uint32_t>(CFG0_Bits::PLLSEL), Clear,
               static_cast<uint32_t>(CFG0_Bits::PREDV0), Clear,
               static_cast<uint32_t>(CFG0_Bits::PLLMF), Clear,
               static_cast<uint32_t>(CFG0_Bits::USBDPSC), Clear,
               static_cast<uint32_t>(CFG0_Bits::CKOUT0SEL), Clear,
               static_cast<uint32_t>(CFG0_Bits::PLLMF_4), Clear,
               static_cast<uint32_t>(CFG0_Bits::ADCPSC_2), Clear,
               static_cast<uint32_t>(CFG0_Bits::PLLMF_5), Clear,
               static_cast<uint32_t>(CFG0_Bits::USBDPSC_2), Clear);
    write_bits(*this, RCU_Regs::INTR, static_cast<uint32_t>(INTR_Bits::CKMIC), Set,
               static_cast<uint32_t>(INTR_Bits::CLEAR_ALL), Set);
    write_bits(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Clear,
               static_cast<uint32_t>(CFG1_Bits::PLLPRESEL), Clear);
}

// Enable or disable peripheral clock
void RCU::set_pclk_enable(RCU_PCLK pclk, bool enable) {
    const auto &info = pclk_index[static_cast<int>(pclk)];
    uint32_t reg_value = read_register<uint32_t>(*this, info.register_offset);

    // Decode bit information
    uint32_t start_bit = static_cast<uint32_t>(info.bit_info) >> 16;
    uint32_t bit_width = static_cast<uint32_t>(info.bit_info) & 0xFFFF;

    // Create bit mask
    uint32_t mask = ((1 << bit_width) - 1) << start_bit;
    reg_value = enable ? (reg_value | mask) : (reg_value & ~mask);

    write_register<uint32_t>(*this, info.register_offset, reg_value);
}

// Enable or disable peripheral clock sleep
void RCU::set_pclk_sleep_enable(RCU_PCLK_Sleep pclk, bool enable) {
    const auto &info = pclk_sleep_index[static_cast<size_t>(pclk)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

// Enable or disable peripheral clock reset
void RCU::set_pclk_reset_enable(RCU_PCLK_Reset pclk, bool enable) {
    const auto &info = pclk_reset_index[static_cast<size_t>(pclk)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

// Enable or disable backup clock reset
void RCU::set_backup_reset_enable(bool enable) {
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::BKPRST), enable ? Set : Clear);
}

void RCU::set_system_source(System_Clock_Source source) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::SCS), static_cast<uint32_t>(source));
}

System_Clock_Source RCU::get_system_source() {
    uint32_t clock = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::SCSS));
    // Find the corresponding enum value using the lookup table
    for (const auto &mapping : source_mapping) {
        if (mapping.value == clock) {
            return mapping.source;
        }
    }
    // Handle invalid selection
    return System_Clock_Source::SOURCE_INVALID;
}

void RCU::set_ahb_prescaler(AHB_Prescaler prescaler) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_apb1_prescaler(APB_Prescaler prescaler) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB1PSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_apb2_prescaler(APB_Prescaler prescaler) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB2PSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_ckout0_source(CKOUT0_Source source) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::CKOUT0SEL), static_cast<uint32_t>(source));
}

void RCU::set_pll_config(PLL_Source source, PLLMF_Select multiplier) {
    // Set PLL source
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLSEL), static_cast<uint32_t>(source));

    if (multiplier <= PLLMF_Select::PLL_MUL16) {
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF), static_cast<uint32_t>(multiplier));
    } else if (multiplier <= PLLMF_Select::PLL_MUL31) {
        // Set the PLLMF_4 bit and adjust PLLMF value based on multiplier
        uint32_t pllmf_val = static_cast<uint32_t>(multiplier) - 16;
        write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_4), Set,
                   static_cast<uint32_t>(CFG0_Bits::PLLMF), pllmf_val);
    } else {
        // Default configuration
        write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_4), Clear,
                   static_cast<uint32_t>(CFG0_Bits::PLLMF), 18);  // Default to 72MHz
    }
}

PLL_Source RCU::get_pll_source() {
    uint32_t clock = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLSEL));

    for (const auto &mapping : pll_mapping) {
        if (mapping.value == clock) {
            return mapping.source;
        }
    }
    // Handle invalid selection
    return PLL_Source::PLLSRC_INVALID;
}

void RCU::set_pll_presel(PLL_Presel presel) {
    write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::PLLPRESEL), static_cast<uint32_t>(presel));
}

PLL_Presel RCU::get_pll_presel() {
    uint32_t clock = read_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::PLLPRESEL));

    for (const auto &mapping : pll_presel_mapping) {
        if (mapping.value == clock) {
            return mapping.source;
        }
    }
    // Handle invalid selection
    return PLL_Presel::PLLPRESRC_INVALID;
}

// 0: No predv0
// 1: clock / 2
void RCU::set_predv0_config(uint32_t div) {
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PREDV0), static_cast<uint32_t>(div));
}

void RCU::set_adc_prescaler(ADC_Prescaler prescaler) {
    // Reset prescaler
    write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC_2), Clear,
               static_cast<uint32_t>(CFG0_Bits::ADCPSC), Clear);
    write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Clear);

    // Set the prescaler
    switch (prescaler) {
    case ADC_Prescaler::CKAPB2_DIV2:
    case ADC_Prescaler::CKAPB2_DIV4:
    case ADC_Prescaler::CKAPB2_DIV6:
    case ADC_Prescaler::CKAPB2_DIV8:
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), static_cast<uint32_t>(prescaler));
        break;
    case ADC_Prescaler::CKAPB2_DIV12:
        write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), Set,
                   static_cast<uint32_t>(CFG0_Bits::ADCPSC_2), Set);
        break;
    case ADC_Prescaler::CKAPB2_DIV16:
        write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), 3,
                   static_cast<uint32_t>(CFG0_Bits::ADCPSC_2), Set);
        break;
    case ADC_Prescaler::CKAHB_DIV5:
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), Clear);
        write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Set);
        break;
    case ADC_Prescaler::CKAHB_DIV6:
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), Set);
        write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Set);
        break;
    case ADC_Prescaler::CKAHB_DIV10:
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), 2);
        write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Set);
        break;
    case ADC_Prescaler::CKAHB_DIV20:
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::ADCPSC), 3);
        write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::ADCPSC_3), Set);
        break;
    default:
        break;
    }
}

void RCU::set_usb_prescaler(USB_Prescaler prescaler) {
    uint32_t real_bit_value = 0;
    if (prescaler <= USB_Prescaler::DIV2) {
        write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::USBDPSC), static_cast<uint32_t>(prescaler));
    } else {
        switch (prescaler) {
        case USB_Prescaler::DIV3:
            real_bit_value = 0;
            break;
        case USB_Prescaler::DIV3_5:
            real_bit_value = 1;
            break;
        case USB_Prescaler::DIV4:
            real_bit_value = 2;
            break;
        default:
            break;
        }
        write_bits(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::USBDPSC), real_bit_value,
                   static_cast<uint32_t>(CFG0_Bits::USBDPSC_2), Set);
    }
}

void RCU::set_rtc_source(RTC_Source source) {
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::RTCSRC), static_cast<uint32_t>(source));
}

void RCU::set_ck48m_source(CK48M_Source source) {
    write_bit(*this, RCU_Regs::ADDCTL, static_cast<uint32_t>(ADDCTL_Bits::CK48MSEL), static_cast<uint32_t>(source));
}

void RCU::set_lxtal_drive_capability(LXTAL_Drive drive) {
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::LXTALDRI), static_cast<uint32_t>(drive));
}

void RCU::set_osci_enable(OSCI_Select osci, bool enable) {
    const auto &info = osci_select_index[static_cast<size_t>(osci)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

bool RCU::is_osci_stable(OSCI_Select osci) {
    volatile uint32_t count = 0;
    bool osci_stable = false;

    const uint32_t timeout = (osci == OSCI_Select::HXTAL) ? HXTAL_STARTUP_TIMEOUT :
                             (osci == OSCI_Select::LXTAL) ? LXTAL_STARTUP_TIMEOUT :
                             (osci == OSCI_Select::IRC8M) ? IRC8M_STARTUP_TIMEOUT :
                             OSC_STARTUP_TIMEOUT;  // Default for IRC48M, IRC40K, PLL_CK

    Status_Flags flag = (osci == OSCI_Select::HXTAL) ? Status_Flags::FLAG_HXTALSTB :
                        (osci == OSCI_Select::LXTAL) ? Status_Flags::FLAG_LXTALSTB :
                        (osci == OSCI_Select::IRC8M) ? Status_Flags::FLAG_IRC8MSTB :
                        (osci == OSCI_Select::IRC48M) ? Status_Flags::FLAG_IRC48MSTB :
                        (osci == OSCI_Select::IRC40K) ? Status_Flags::FLAG_IRC40KSTB :
                        Status_Flags::FLAG_PLLSTB; // Default for PLL_CK

    while (!osci_stable && count != timeout) {
        osci_stable = get_flag(flag);
        count = count + 1;
    }

    return osci_stable;
}

inline uint32_t RCU::get_clock_source_frequency(System_Clock_Source source) {
    switch (source) {
    case System_Clock_Source::SOURCE_IRC8M:
        return IRC8M_VALUE;
    case System_Clock_Source::SOURCE_HXTAL:
        return HXTAL_VALUE;
    case System_Clock_Source::SOURCE_PLL:
        return calculate_pll_frequency();
    default:
        return IRC8M_VALUE;
    }
}

uint32_t RCU::calculate_pll_frequency() {
    uint32_t ck_src, pll_mul_factor;

    // Determine PLL source (HXTAL or IRC8M/IRC48M)
    PLL_Source pll_source = get_pll_source();
    if (pll_source == PLL_Source::PLLSRC_HXTAL_IRC48M) {
        PLL_Presel pll_presel = get_pll_presel();
        ck_src = (pll_presel == PLL_Presel::PLLPRESRC_HXTAL) ? HXTAL_VALUE : IRC48M_VALUE;
        if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PREDV0))) {
            ck_src /= 2U;
        }
    } else {
        ck_src = IRC8M_VALUE / 2U;  // PLL source is IRC8M/2
    }

    // Calculate PLL multiplier
    pll_mul_factor = get_pll_multiplier();
    return ck_src * pll_mul_factor;
}

inline uint32_t RCU::get_pll_multiplier() {
    uint32_t pllmf = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF));
    if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_4))) {
        pllmf |= 0x10;
    }
    if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_5))) {
        pllmf |= 0x20;
    }

    // Adjust the multiplier based on the value
    if (pllmf < 15) {
        return pllmf + 2;
    } else if (pllmf <= 62) {
        return pllmf + 1;
    } else {
        return 63;
    }
}

uint32_t RCU::get_clock_frequency(Clock_Frequency clock) {
    System_Clock_Source current_source = get_system_source();
    uint32_t cksys_freq = get_clock_source_frequency(current_source);

    // Calculate AHB frequency
    uint32_t ahb_prescaler_idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC));
    uint32_t ahb_freq = cksys_freq >> AHB_EXP[ahb_prescaler_idx];

    // Calculate APB1 frequency
    uint32_t apb1_prescaler_idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB1PSC));
    uint32_t apb1_freq = ahb_freq >> APB1_EXP[apb1_prescaler_idx];

    // Calculate APB2 frequency
    uint32_t apb2_prescaler_idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB2PSC));
    uint32_t apb2_freq = ahb_freq >> APB2_EXP[apb2_prescaler_idx];

    // Return requested clock frequency
    switch (clock) {
    case Clock_Frequency::CK_SYS:
        return cksys_freq;
    case Clock_Frequency::CK_AHB:
        return ahb_freq;
    case Clock_Frequency::CK_APB1:
        return apb1_freq;
    case Clock_Frequency::CK_APB2:
        return apb2_freq;
    default:
        return 0;  // Handle invalid clock type
    }
}

void RCU::set_bypass_mode_enable(OSCI_Select osci, bool enable) {
    switch (osci) {
    case OSCI_Select::HXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HXTALBPS), enable ? Set : Clear);
        break;
    case OSCI_Select::LXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::LXTALBPS), enable ? Set : Clear);
        break;
    case OSCI_Select::IRC8M:
    case OSCI_Select::IRC48M:
    case OSCI_Select::IRC40K:
    case OSCI_Select::PLL_CK:
        break;
    default:
        break;
    }
}

void RCU::set_irc8m_adjustment_value(uint32_t value) {
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::IRC8MADJ), static_cast<uint32_t>(value));
}

void RCU::set_hxtal_monitor_enable(bool enable) {
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CKMEN), enable ? Set : Clear);
}

void RCU::set_deep_sleep_voltage(DeepSleep_Voltage voltage) {
    write_bit(*this, RCU_Regs::DSV, static_cast<uint32_t>(DSV_Bits::DSLPVS), static_cast<uint32_t>(voltage));
}

bool RCU::get_flag(Status_Flags flag) {
    return get_value(flag);
}

void RCU::clear_all_reset_flags() {
    write_bit(*this, RCU_Regs::RSTSCK, static_cast<uint32_t>(RSTSCK_Bits::RSTFC), Set);
}

bool RCU::get_interrupt_flag(Interrupt_Flags flag) {
    return get_value(flag);
}

void RCU::clear_interrupt_flag(Clear_Flags flag) {
    const auto &info = clear_flag_index[static_cast<int>(flag)];
    write_bit(*this, info.register_offset, info.bit_info, Set);
}

void RCU::set_interrupt_enable(Interrupt_Type type, bool enable) {
    const auto &info = interrupt_type_index[static_cast<int>(type)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

void RCU::update_system_clock() {
    System_Clock_Source current_source = get_system_source();

    switch (current_source) {
    case System_Clock_Source::SOURCE_IRC8M:
        SystemCoreClock = IRC8M_VALUE;
        break;
    case System_Clock_Source::SOURCE_HXTAL:
        SystemCoreClock = HXTAL_VALUE;
        break;
    case System_Clock_Source::SOURCE_PLL:
        SystemCoreClock = calculate_pll_frequency();
        break;
    default:
        SystemCoreClock = IRC8M_VALUE;
        break;
    }

    uint32_t ahb_bit = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC));

    // calculate AHB clock frequency
    SystemCoreClock >>= AHBPrescaler[static_cast<int>(ahb_bit)];
}

} // namespace rcu

// Instantiate the class of global RCU Device
rcu::RCU RCU_DEVICE;
