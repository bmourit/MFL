// gd32f30x CORTEX peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

namespace cortex {


///////////////////////////// ADDRESS MAPS /////////////////////////////

static constexpr unsigned int NVIC_vectorTable[] = {
    0x20000000,
    0x08000000,
};

static constexpr unsigned int NVIC_priorityGroup[] = {
    0x00000700,
    0x00000600,
    0x00000500,
    0x00000400,
    0x00000300,
};


///////////////////////////// ENUMS /////////////////////////////

enum class Priority_Group {
    PRIO_GROUP_PRE0SUB4,
    PRIO_GROUP_PRE1SUB3,
    PRIO_GROUP_PRE2SUB2,
    PRIO_GROUP_PRE3SUB1,
    PRIO_GROUP_PRE4SUB0,
};

enum class Vector_Table {
    NVIC_VECTTAB_RAM,
    NVIC_VECTTAB_FLASH,
};

enum class Low_Power_Mode {
    SLEEP_ON_EXIT = 1,
    SLEEP_DEEP = 2,
    SEV_ON_PEND = 4,
};

enum class Systick_Source {
    SYSTICK_SOURCE_HCLK,
    SYSTICK_SOURCE_HCLK_DIV8,
};

} // namespace cortex
