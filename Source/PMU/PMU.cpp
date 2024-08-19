// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "gd32f303re.h"
#include "PMU.hpp"
#include "PMU_C.h"

namespace pmu {

// Reset PMU
void PMU::reset()
{
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, true);
    RCU_DEVICE.set_pclk_reset_enable(rcu::RCU_PCLK_Reset::PCLK_PMURST, false);
}

// Enable PMU pclk
void PMU::set_pclk_enable(bool enable)
{
    RCU_DEVICE.set_pclk_enable(rcu::RCU_PCLK::PCLK_PMU, enable ? true : false);
}

// Enable low voltage detection (LVD)
void PMU::lvd_enable(LVD_Threshold threshold)
{
    // Reset
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDEN), 0);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDT), 0);

    // Set LVDT bits by threshold value
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDT), static_cast<uint32_t>(threshold));
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDEN), 1);
}

void PMU::lvd_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LVDEN), 0);
}

void PMU::set_ldo_output(Output_Voltage level)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDOVS), static_cast<uint32_t>(level));
}

void PMU::high_driver_switch(bool enable)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDS), enable ? 1 : 0);
    while (get_flag(Status_Flags::HDSR_FLAG) == false) {
        // Just wait
    }
}

void PMU::high_driver_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), 1);
    while (get_flag(Status_Flags::HDR_FLAG) == false) {
        // Just wait
    }
}

void PMU::high_driver_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), 0);
}

// Enable or disable high driver mode
void PMU::set_high_driver_enable(bool enable)
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::HDEN), enable ? 1 : 0);
}

// Enable low driver for deep sleep
void PMU::low_driver_on_deep_sleep_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 3);
}

// Disable low driver for deep sleep
void PMU::low_driver_on_deep_sleep_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 0);
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
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBMOD), 1);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::WURST), 1);

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

void PMU::set_deep_sleep_enable(Power_Driver driver, Bit_State state, PMU_Commands cmd)
{
    uint8_t value = 0;

    if (cmd == PMU_Commands::WFI_CMD) {
        value = 1;
    } else if (cmd == PMU_Commands::WFE_CMD) {
        value = 2;
    }

    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBMOD), 0);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDOLP), 0);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 0);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDNP), 0);
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDLP), 0);

    // Set ldolp bit according to driver
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDOLP), static_cast<uint32_t>(driver));

    // low drive mode config in deep-sleep mode
    if (state == Bit_State::BIT_ENABLE) {
        if (driver == Power_Driver::NORMAL_DRIVER) {
            write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDNP), 1);
            write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 1);
        } else {
            write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDLP), 1);
            write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::LDEN), 1);
        }
    }

    set_deep_sleep_mode_command(value);
}

void PMU::wakeup_pin_enable()
{
    write_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(CS_Bits::WUPEN), 1);
}

void PMU::wakeup_pin_disable()
{
    write_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(CS_Bits::WUPEN), 0);
}

void PMU::backup_write_enable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::BKPWEN), 1);
}

void PMU::backup_write_disable()
{
    write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::BKPWEN), 0);
}

bool PMU::get_flag(Status_Flags flag)
{
    uint32_t value = read_bit(*this, PMU_Regs::CS, static_cast<uint32_t>(flag));
    return (value != 0) ? true : false;
}

void PMU::clear_flag(Reset_State_Flags flag)
{
    if (flag == Reset_State_Flags::RESET_WAKEUP_FLAG) {
        write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::WURST), 1);
    } else if (flag == Reset_State_Flags::RESET_STANDBY_FLAG) {
        write_bit(*this, PMU_Regs::CTL, static_cast<uint32_t>(CTL_Bits::STBRST), 1);
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
