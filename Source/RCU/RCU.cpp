// gd32f30x RCU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "RCU_C.h"
#include "CEE.hpp"
#include "PMU.hpp"
#include "RCU.hpp"

namespace rcu {

constexpr uint32_t CoreClockFrequency = 120000000;

void RCU::reset()
{
    // Enable IRC8M
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::IRC8MEN), Set);
    while (is_osci_stable(OSCI_Select::IRC8M) == false) {
        //Just wait
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
void RCU::set_pclk_enable(RCU_PCLK pclk, bool enable)
{
    const auto &info = pclk_index[static_cast<size_t>(pclk)];
    uint32_t reg_value = read_register<uint32_t>(info.register_offset);

    // Decode bit information
    uint32_t start_bit = static_cast<uint32_t>(info.bit_info) >> 16;
    uint32_t bit_width = static_cast<uint32_t>(info.bit_info) & 0xFFFF;

    // Create bit mask
    uint32_t mask = ((1 << bit_width) - 1) << start_bit;

    reg_value = enable ? (reg_value | mask) : (reg_value & ~mask);

    write_register(info.register_offset, reg_value);
}

// Enable or disable peripheral clock sleep
void RCU::set_pclk_sleep_enable(RCU_PCLK_Sleep pclk, bool enable)
{
    const auto &info = pclk_sleep_index[static_cast<size_t>(pclk)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

// Enable or disable peripheral clock reset
void RCU::set_pclk_reset_enable(RCU_PCLK_Reset pclk, bool enable)
{
    const auto &info = pclk_reset_index[static_cast<size_t>(pclk)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

// Enable or disable backup clock reset
void RCU::set_backup_reset_enable(bool enable)
{
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::BKPRST), enable ? Set : Clear);
}

void RCU::set_system_source(System_Clock_Source source)
{
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::SCS), static_cast<uint32_t>(source));
}

System_Clock_Source RCU::get_system_source()
{
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

void RCU::set_ahb_prescaler(AHB_Prescaler prescaler)
{
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_apb1_prescaler(APB_Prescaler prescaler)
{
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB1PSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_apb2_prescaler(APB_Prescaler prescaler)
{
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB2PSC), static_cast<uint32_t>(prescaler));
}

void RCU::set_ckout0_source(CKOUT0_Source source)
{
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

PLL_Source RCU::get_pll_source()
{
    uint32_t clock = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLSEL));

    for (const auto &mapping : pll_mapping) {
        if (mapping.value == clock) {
            return mapping.source;
        }
    }
    // Handle invalid selection
    return PLL_Source::PLLSRC_INVALID;
}

void RCU::set_pll_presel(PLL_Presel presel)
{
    write_bit(*this, RCU_Regs::CFG1, static_cast<uint32_t>(CFG1_Bits::PLLPRESEL), static_cast<uint32_t>(presel));
}

PLL_Presel RCU::get_pll_presel()
{
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
void RCU::set_predv0_config(uint32_t div)
{
    write_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PREDV0), static_cast<uint32_t>(div));
}

void RCU::set_adc_prescaler(ADC_Prescaler prescaler)
{
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

void RCU::set_usb_prescaler(USB_Prescaler prescaler)
{
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

void RCU::set_rtc_source(RTC_Source source)
{
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::RTCSRC), static_cast<uint32_t>(source));
}

void RCU::set_ck48m_source(CK48M_Source source)
{
    write_bit(*this, RCU_Regs::ADDCTL, static_cast<uint32_t>(ADDCTL_Bits::CK48MSEL), static_cast<uint32_t>(source));
}

void RCU::set_lxtal_drive_capability(LXTAL_Drive drive)
{
    write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::LXTALDRI), static_cast<uint32_t>(drive));
}

void RCU::set_osci_enable(OSCI_Select osci, bool enable)
{
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

uint32_t RCU::get_clock_frequency(Clock_Frequency clock)
{
    System_Clock_Source current_source = get_system_source();

    uint32_t ck_freq = 0;
    uint32_t cksys_freq, ahb_freq, apb1_freq, apb2_freq;

    PLL_Source pll_source;
    PLL_Presel pll_presel;

    uint32_t predv0_sel, pllmf, ck_src, idx, clk_exp;

    uint8_t ahb_exp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    uint8_t apb1_exp[8] = {0, 0, 0, 0, 1, 2, 3, 4};
    uint8_t apb2_exp[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    switch (current_source) {
    case System_Clock_Source::SOURCE_IRC8M:
        cksys_freq = IRC8M_VALUE;
        break;
    case System_Clock_Source::SOURCE_HXTAL:
        cksys_freq = HXTAL_VALUE;
        break;
    case System_Clock_Source::SOURCE_PLL:
        // PLL driven by HXTAL, IRC48M or IRC8M/2
        pll_source = get_pll_source();
        if (pll_source == PLL_Source::PLLSRC_HXTAL_IRC48M) {
            // PLL source is HXTAL or IRC48M
            pll_presel = get_pll_presel();
            if (pll_presel == PLL_Presel::PLLPRESRC_HXTAL) {
                ck_src = HXTAL_VALUE;
            } else {
                ck_src = IRC48M_VALUE;
            }
            predv0_sel = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PREDV0));
            // Half the clock if PREDV0 bit is set
            if (predv0_sel != 0) {
                ck_src /= 2U;
            }
        } else {
            // PLL source is IRC8M/2
            ck_src = IRC8M_VALUE / 2U;
        }

        // PLL multiplication
        pllmf = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF));
        if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_4))) {
            pllmf |= 0x10;
        }
        if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_5))) {
            pllmf |= 0x20;
        }
        if (pllmf < 15) {
            pllmf += 2;
        } else if ((pllmf >= 15) && (pllmf <= 62)) {
            pllmf += 1;
        } else {
            pllmf = 63;
        }
        cksys_freq = ck_src * pllmf;
        break;
    default:
        cksys_freq = IRC8M_VALUE;
        break;
    }

    // AHB frequency
    idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC));
    clk_exp = ahb_exp[idx];
    ahb_freq = cksys_freq >> clk_exp;

    // APB1 frequency
    idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB1PSC));
    clk_exp = apb1_exp[idx];
    apb1_freq = ahb_freq >> clk_exp;

    // APB2 frequency
    idx = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::APB2PSC));
    clk_exp = apb2_exp[idx];
    apb2_freq = ahb_freq >> clk_exp;

    // return clocks frequency by clock type
    switch (clock) {
    case Clock_Frequency::CK_SYS:
        ck_freq = cksys_freq;
        break;
    case Clock_Frequency::CK_AHB:
        ck_freq = ahb_freq;
        break;
    case Clock_Frequency::CK_APB1:
        ck_freq = apb1_freq;
        break;
    case Clock_Frequency::CK_APB2:
        ck_freq = apb2_freq;
        break;
    default:
        break;
    }

    return ck_freq;
}

void RCU::bypass_mode_enable(OSCI_Select osci)
{
    switch (osci) {
    case OSCI_Select::HXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HXTALBPS), Set);
        break;
    case OSCI_Select::LXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::LXTALBPS), Set);
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

void RCU::bypass_mode_disable(OSCI_Select osci)
{
    switch (osci) {
    case OSCI_Select::HXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HXTALBPS), Clear);
        break;
    case OSCI_Select::LXTAL:
        set_osci_enable(osci, false);
        write_bit(*this, RCU_Regs::BDCTL, static_cast<uint32_t>(BDCTL_Bits::LXTALEN), Clear);
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

void RCU::set_irc8m_adjustment_value(uint32_t value)
{
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::IRC8MADJ), static_cast<uint32_t>(value));
}

void RCU::hxtal_monitor_enable(void)
{
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CKMEN), Set);
}

void RCU::hxtal_monitor_disable(void)
{
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::CKMEN), Clear);
}

void RCU::set_deep_sleep_voltage(DeepSleep_Voltage voltage)
{
    write_bit(*this, RCU_Regs::DSV, static_cast<uint32_t>(DSV_Bits::DSLPVS), static_cast<uint32_t>(voltage));
}

bool RCU::get_flag(Status_Flags flag)
{
    return get_value(flag);
}

void RCU::clear_all_reset_flags()
{
    write_bit(*this, RCU_Regs::RSTSCK, static_cast<uint32_t>(RSTSCK_Bits::RSTFC), Set);
}

bool RCU::get_interrupt_flag(Interrupt_Flags flag)
{
    return get_value(flag);
}

void RCU::clear_interrupt_flag(Clear_Flags flag)
{
    const auto &info = clear_flag_index[static_cast<size_t>(flag)];
    write_bit(*this, info.register_offset, info.bit_info, 1);
}

void RCU::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    const auto &info = interrupt_type_index[static_cast<size_t>(type)];
    write_bit(*this, info.register_offset, info.bit_info, enable ? Set : Clear);
}

void RCU::clocks_init()
{
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HXTALEN), Set);

    // Wait until HXTAL stable flag is set or a timeout occurs
    while (is_osci_stable(OSCI_Select::HXTAL) == false) {
    }

    // Enable PMU clock
    write_bit(*this, RCU_Regs::APB1EN, static_cast<uint32_t>(APB1EN_Bits::PMUEN), Set);

    // TODO: Make this a configurable setting
    //  Note:
    //  According to the manual, this can only be set while main PLL
    //  is off. This setting takes effect on the 1.2V power domain
    //  when the main PLL is on. The manual states the when the main
    //  PLL is off, this is automatically set to LDO_VOLATGE_LOW
    //  This helps lower power requirements at the expense of driving
    //  capabilities.
    //  Since we need PLL off for this, we should make this a
    //  configurable setting for different user requirments.
    //  For now, if you need something different comment out the
    //  default (LOW) and uncomment one of the other choices below.
    //  Once Arduino core is supported with this library, this whole
    //  clocks_init will move into board variant files.
    PMU_DEVICE.set_ldo_output(pmu::Output_Voltage::LDO_VOLTAGE_LOW);
    //PMU_DEVICE.set_ldo_output(pmu::Output_Voltage::LDO_VOLTAGE_MID);
    //PMU_DEVICE.set_ldo_output(pmu::Output_Voltage::LDO_VOLTAGE_HIGH);

    // AHB = SYSCLK
    set_ahb_prescaler(AHB_Prescaler::CKSYS_DIV1);
    set_apb2_prescaler(APB_Prescaler::CKAHB_DIV1);
    set_apb1_prescaler(APB_Prescaler::CKAHB_DIV2);

    // CK_PLL = (CK_HXTAL / 2) * 30 = 120 MHz
    set_predv0_config(Set);
    set_pll_config(PLL_Source::PLLSRC_HXTAL_IRC48M, PLLMF_Select::PLL_MUL30);

    // Enable PLL
    write_bit(*this, RCU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::PLLEN), Set);

    // Wait for PLL to stablize
    while (is_osci_stable(OSCI_Select::PLL_CK) == false) {
    }

    // Enable high-drive for high clock frequency
    PMU_DEVICE.set_high_driver_enable(true);
    // Select high-drive mode
    PMU_DEVICE.high_driver_switch(true);

    // PLL as system clock
    set_system_source(System_Clock_Source::SOURCE_PLL);

    // Verify PLL is set as system clock
    while (get_system_source() != System_Clock_Source::SOURCE_PLL) {
    }

    // Set the CMSIS global variable
    //SystemCoreClock = CoreClockFrequency;
}

void RCU::update_system_clock()
{
    System_Clock_Source current_source = get_system_source();

    switch (current_source) {
    case System_Clock_Source::SOURCE_IRC8M:
        SystemCoreClock = IRC8M_VALUE;
        break;
    case System_Clock_Source::SOURCE_HXTAL:
        SystemCoreClock = HXTAL_VALUE;
        break;
    case System_Clock_Source::SOURCE_PLL:
        SystemCoreClock = calculate_pll_clock();
        break;
    default:
        SystemCoreClock = IRC8M_VALUE;
        break;
    }

    uint32_t ahb_bit = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::AHBPSC));

    // calculate AHB clock frequency
    SystemCoreClock >>= AHBPrescaler[static_cast<int>(ahb_bit)];
}

inline uint32_t RCU::calculate_pll_clock()
{
    uint32_t value = 0;
    uint32_t clock_source = 0;
    uint32_t multiplier = 0;

    PLL_Source pll_source = get_pll_source();
    if (pll_source == PLL_Source::PLLSRC_HXTAL_IRC48M) {
        PLL_Presel pll_presel = get_pll_presel();
        if (pll_presel == PLL_Presel::PLLPRESRC_HXTAL) {
            clock_source = HXTAL_VALUE;
        } else {
            clock_source = IRC48M_VALUE;
        }
        uint32_t predv0_sel = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PREDV0));
        // PREDV0 0: no divider, 1: Divide by 2
        if (predv0_sel != 0) {
            clock_source /= 2;
        }
    } else {
        clock_source = IRC8M_VALUE / 2;
    }
    // PLL multiplier
    multiplier = read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF));
    if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_4))) {
        multiplier |= 0x10;
    }
    if (read_bit(*this, RCU_Regs::CFG0, static_cast<uint32_t>(CFG0_Bits::PLLMF_5))) {
        multiplier |= 0x20;
    }
    if (multiplier >= 15) {
        multiplier += 1;
    } else {
        multiplier += 2;
    }
    if (multiplier > 61) {
        multiplier = 63;
    }
    value = clock_source * multiplier;

    return value;
}

} // namespace rcu

// Instantiate the class of global RCU Device
rcu::RCU RCU_DEVICE;
