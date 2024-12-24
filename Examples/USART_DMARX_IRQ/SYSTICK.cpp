// gd32f303re SYSTICK example
// Copyright (c) B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdio>
#include <cstdint>

#include "F303RE.hpp"
#include "RCU.hpp"

volatile uint32_t systick_counter = 0;

extern "C" void SysTick_Handler(void) {
    // Increment a global counter or do time-related tasks
    systick_counter = systick_counter + 1;
}

uint32_t get_millis() {
    return systick_counter;
}
