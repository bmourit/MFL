//
// MFL gd32f30x STARTUP and clock initialization in C++
//
// Copyright (C) 2024 B. Mouritsen <bnmguy@gmail.com>. All rights reserved.
//
// This file is part of the Microcontroller Firmware Library (MFL).
//
// MFL is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// MFL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with MFL.
// If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

//#include <stdlib.h>
//#include <stdint.h>

//#include "CONFIG.hpp"
//#include "rcu_config.hpp"

namespace startup {


///////////////////////////// ENUMS /////////////////////////////

/*enum class Startup_Clocks : uint8_t {
    SYSTEM = 1,
    AHB = 2,
    APB1 = 4,
    APB2 = 8
};

enum class Startup_Oscillators : uint8_t {
    NONE,
    HXTAL,
    IRC8M,
    LXTAL = 4,
    IRC48K = 8
};

enum class Startup_Peripherals : uint8_t {
    RTC = 1,
    ADC = 2,
    I2S0 = 4,
    I2S1 = 8,
    USB = 10
};

enum class STARTUP_Error_Type : uint8_t {
    OK,
    ERROR,
    TIMEOUT
};*/


///////////////////////////// STRUCTURES /////////////////////////////

/*struct Pll_Config {
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
};

struct Oscillator_Config {
    uint32_t hxtal_state;
    uint32_t irc8m_state;
    uint32_t irc40k_state;
    uint32_t lxtal_state;
    Startup_Oscillators oscillator;
    Pll_Config config;
    bool is_hxtal_prediv;
};

struct Peripheral_Config {
    Startup_Peripherals peripheral;
}; */

} // namespace startup
