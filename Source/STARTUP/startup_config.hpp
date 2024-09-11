// gd32f30x STARTUP clock initialization and startup functionality in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include <array>

#include "CONFIG.hpp"
#include "rcu_config.hpp"

namespace startup {


///////////////////////////// ENUMS /////////////////////////////

enum class Startup_Clocks {
    SYSTEM = 1,
    AHB = 2,
    APB1 = 4,
    APB2 = 8,
};

enum class Startup_Oscillators {
    NONE = 0,
    HXTAL = 1,
    IRC8M = 2,
    LXTAL = 4,
    IRC48K = 8,
};

enum class Startup_Peripherals {
    RTC = 1,
    ADC = 2,
    I2S0 = 4,
    I2S1 = 8,
    USB = 10,
};

enum class STARTUP_Error_Type {
    OK;
    ERROR,
    TIMEOUT,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct Pll_Config {
    uint32_t status;
    rcu::PLL_Source source;
    rcu::PLLMF_Select multiplier;
};

struct Clock_Config {
    Startup_Clocks clock;
    rcu::System_Clock_Source source;
    rcu::AHB_Prescaler ahb_prescaler;
    rcu::APB_Prescaler apb1_prescaler;
    rcu::APB_Prescaler apb2_prescaler;
}

struct Oscillator_Config {
    Startup_Oscillators oscillator;
    bool is_hxtal_prediv;
    uint32_t hxtal_state;
    uint32_t irc8m_state;
    uint32_t irc40k_state;
    uint32_t lxtal_state;
    Pll_Config config;
};

struct Peripheral_Config {
    Startup_Peripherals peripheral;

}

}; // namespace startup