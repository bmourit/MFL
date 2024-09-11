// gd32f30x SAFE_CLOCKS startup clock initialization in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "STARTUP.hpp"

namespace startup {

void STARTUP::startup_init() {
    FMC_DEVICE.set_wait_state(fmc::Wait_State::WS_WSCNT_2);
    play_tone();
    write_bit(*this, rcu::RCU_Regs::CTL, static_cast<uint32_t>(rcu::CTL_Bits::HXTALEN), Set);
    play_tone();
    // Wait until HXTAL stable flag is set or a timeout occurs
    while (RCU_DEVICE.is_osci_stable(rcu::OSCI_Select::HXTAL) == false) {
    }
    play_tone();
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
    play_tone();
    // AHB = SYSCLK
    RCU_DEVICE.set_ahb_prescaler(rcu::AHB_Prescaler::CKSYS_DIV1);
    play_tone();
    RCU_DEVICE.set_apb2_prescaler(rcu::APB_Prescaler::CKAHB_DIV1);
    play_tone();
    RCU_DEVICE.set_apb1_prescaler(rcu::APB_Prescaler::CKAHB_DIV2);
    play_tone();
    // CK_PLL = (CK_HXTAL / 2) * 30 = 120 MHz
    //set_predv0_config(Set);
    RCU_DEVICE.set_predv0_config(Clear);
    play_tone();
    RCU_DEVICE.set_pll_config(rcu::PLL_Source::PLLSRC_HXTAL_IRC48M, rcu::PLLMF_Select::PLL_MUL9);
    play_tone();
    // Enable PLL
    write_bit(*this, rcu::RCU_Regs::CTL, static_cast<uint32_t>(rcu::CTL_Bits::PLLEN), Set);
    play_tone();
    // Wait for PLL to stablize
    while (RCU_DEVICE.is_osci_stable(rcu::OSCI_Select::PLL_CK) == false) {
    }
    play_tone();
    // Enable high-drive for high clock frequency
    PMU_DEVICE.set_high_driver_enable(true);
    play_tone();
    // Select high-drive mode
    PMU_DEVICE.high_driver_switch(true);
    play_tone();

    // PLL as system clock
    RCU_DEVICE.set_system_source(rcu::System_Clock_Source::SOURCE_PLL);
    play_tone();

    // Verify PLL is set as system clock
    while (RCU_DEVICE.get_system_source() != rcu::System_Clock_Source::SOURCE_PLL) {
    }
    play_tone();

    // Set the global variable
    RCU_DEVICE.update_system_clock();
    play_tone();
}

} // namespace startup

startup::STARTUP STARTUP_DEVICE;
