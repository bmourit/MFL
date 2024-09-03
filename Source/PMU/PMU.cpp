// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "gd32f303re.h"
#include "PMU.hpp"
#include "PMU_C.h"

namespace pmu {

PMU::PMU() {
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_PMU, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, false);
}

// Reset PMU
void PMU::reset()
{
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, false);
}

// Enable PMU pclk
void PMU::set_pclk_enable(bool enable)
{
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_PMU, enable);
}

// Enable low voltage detection (LVD)
void PMU::lvd_enable(LVD_Threshold threshold)
{
    // Reset
    write_bits(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDEN), Clear,
            static_cast<uint32_t>(CTL_Bits::LVDT), Clear,
            static_cast<uint32_t>(CTL_Bits::LVDT), static_cast<uint32_t>(threshold),
            static_cast<uint32_t>(CTL_Bits::LVDEN), Set);
}

void PMU::lvd_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDEN), Clear);
}

void PMU::set_ldo_output(Output_Voltage level)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDOVS), static_cast<uint32_t>(level));
}

void PMU::high_driver_switch(bool enable)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDS), enable ? Set : Clear);
    while (get_flag(Status_Flags::HDSR_FLAG) == false) {
        // Just wait
    }
}

void PMU::high_driver_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), Set);
    while (get_flag(Status_Flags::HDR_FLAG) == false) {
        // Just wait
    }
}

void PMU::high_driver_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), Clear);
}

// Enable or disable high driver mode
void PMU::set_high_driver_enable(bool enable)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), enable ? Set : Clear);
}

// Enable low driver for deep sleep
void PMU::low_driver_on_deep_sleep_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 3);
}

// Disable low driver for deep sleep
void PMU::low_driver_on_deep_sleep_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), Clear);
}

// Enable or disable low driver for deep sleep
//  Low_Driver:
//      LOW_DRIVER_DEEP_SLEEP_DISABLE
//      LOW_DRIVER_DEEP_SLEEP_ENABLE
void PMU::set_low_driver_on_deep_sleep(Low_Driver mode)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), static_cast<uint32_t>(mode));
}

// Set driver for low power
//  Power_Driver:
//      NORMAL_DRIVER
//      LOW_DRIVER
void PMU::set_driver_on_low_power(Power_Driver driver)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDLP), static_cast<uint32_t>(driver));
}

// Set driver for normal power
//  Power_Driver:
//      NORMAL_DRIVER
//      LOW_DRIVER
void PMU::set_driver_on_normal_power(Power_Driver driver)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDNP), static_cast<uint32_t>(driver));
}

void PMU::set_standby_enable()
{
    write_bits(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBMOD), Set,
            static_cast<uint32_t>(CTL_Bits::WURST), Set);

    set_standby_mode();
}

void PMU::set_sleep_enable(PMU_Commands cmd)
{
    uint8_t value = 0;

    // encode the cmd as uint8_t value
    if (cmd == PMU_Commands::WFI_CMD) {
        value = 1;
    } else {
        value = 2;
    }

    set_sleep_mode_command(value);
}

void PMU::set_deep_sleep_enable(Power_Driver driver, PMU_Commands cmd, bool enable)
{
    uint8_t value = 0;

    if (cmd == PMU_Commands::WFI_CMD) {
        value = 1;
    } else if (cmd == PMU_Commands::WFE_CMD) {
        value = 2;
    }

    write_bits(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBMOD), Clear,
            static_cast<uint32_t>(CTL_Bits::LDOLP), Clear,
            static_cast<uint32_t>(CTL_Bits::LDEN), Clear,
            static_cast<uint32_t>(CTL_Bits::LDNP), Clear,
            static_cast<uint32_t>(CTL_Bits::LDLP), Clear,
            static_cast<uint32_t>(CTL_Bits::LDOLP), static_cast<uint32_t>(driver));
    // low drive mode config in deep-sleep mode
    if (enable) {
        if (driver == Power_Driver::NORMAL_DRIVER) {
            write_bits(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDNP), Set,
                static_cast<uint32_t>(CTL_Bits::LDEN), Set);
        } else {
            write_bits(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDLP), Set,
                static_cast<uint32_t>(CTL_Bits::LDEN), Set);
        }
    }

    set_deep_sleep_mode_command(value);
}

void PMU::wakeup_pin_enable()
{
    write_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(CS_Bits::WUPEN), Set);
}

void PMU::wakeup_pin_disable()
{
    write_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(CS_Bits::WUPEN), Clear);
}

void PMU::backup_write_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::BKPWEN), Set);
}

void PMU::backup_write_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::BKPWEN), Clear);
}

bool PMU::get_flag(Status_Flags flag)
{
    uint32_t value = read_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(flag));
    return (value != Clear);
}

void PMU::clear_flag(Clear_Flags flag)
{
    if (flag == Clear_Flags::RESET_WAKEUP_FLAG) {
        write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::WURST), Set);
    } else if (flag == Clear_Flags::RESET_STANDBY_FLAG) {
        write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBRST), Set);
    } else {
        //TODO: Handle invalid flag
    }
}

} // namespace pmu


extern "C" {
    void set_sleep_mode_command(uint8_t value)
    {
        if ((value < 1) || (value > 2)) {
            return;
        }

        // Clear sleepdeep bit of Cortex-M4 SCR
        SCB->SCR &= ~(static_cast<uint32_t>(SCB_SCR_SLEEPDEEP_Msk));

        if (value == 1) {
            __WFI();
        } else if (value == 2) {
            __WFE();
        }
    }

    void set_deep_sleep_mode_command(uint8_t value)
    {
        static uint32_t register_snapshot[4];

        // Set sleepdeep bit of Cortex-M4 SCR
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

        register_snapshot[0] = (*(volatile uint32_t *)(uint32_t)(0xE000E010U));
        register_snapshot[1] = (*(volatile uint32_t *)(uint32_t)(0xE000E100U));
        register_snapshot[2] = (*(volatile uint32_t *)(uint32_t)(0xE000E104U));
        register_snapshot[3] = (*(volatile uint32_t *)(uint32_t)(0xE000E108U));

        (*(volatile uint32_t *)(uint32_t)(0xE000E010U)) &= 0x00010004U;
        (*(volatile uint32_t *)(uint32_t)(0xE000E180U)) = 0XFF7FF83DU;
        (*(volatile uint32_t *)(uint32_t)(0xE000E184U)) = 0XFFFFF8FFU;
        (*(volatile uint32_t *)(uint32_t)(0xE000E188U)) = 0xFFFFFFFFU;

        // select WFI or WFE command to enter deepsleep mode
        if (value == 1) {
            __WFI();
        } else if (value == 2) {
            __SEV();
            __WFE();
            __WFE();
        }

        (*(volatile uint32_t *)(uint32_t)(0xE000E010U)) = register_snapshot[0];
        (*(volatile uint32_t *)(uint32_t)(0xE000E100U)) = register_snapshot[1];
        (*(volatile uint32_t *)(uint32_t)(0xE000E104U)) = register_snapshot[2];
        (*(volatile uint32_t *)(uint32_t)(0xE000E108U)) = register_snapshot[3];

        // Reset sleepdeep bit of Cortex-M4 SCR
        SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    }

    void set_standby_mode(void)
    {
        // Set sleepdeep bit of Cortex-M4 SCR
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

        (*(volatile uint32_t *)(uint32_t)(0xE000E010U)) &= 0x00010004U;
        (*(volatile uint32_t *)(uint32_t)(0xE000E180U)) = 0XFFFFFFF7U;
        (*(volatile uint32_t *)(uint32_t)(0xE000E184U)) = 0XFFFFFDFFU;
        (*(volatile uint32_t *)(uint32_t)(0xE000E188U)) = 0xFFFFFFFFU;

        __WFI();
    }
} // extern "C"

// Instantiate class for global device
pmu::PMU PMU_DEVICE;
