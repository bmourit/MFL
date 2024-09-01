// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdint>

#include "RCU.hpp"
#include "gd32f303re.h"
#include "gd32f303re_vector_table.hpp"

extern int main();

extern "C" {
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    extern uint32_t __bss_start__;
    extern uint32_t __bss_end__;
}

void Reset_Handler() {
    // Initialize data section
    extern uint32_t _sidata;
    extern uint32_t _sdata;
    extern uint32_t _edata;

    uint32_t *src = &_sidata;
    uint32_t *dest = &_sdata;
    while (dest < &_edata)
        *dest++ = *src++;

    // Initialize bss section
    uint32_t *bss_start = &__bss_start__;
    uint32_t *bss_end = &__bss_end__;
    while (bss_start < bss_end)
        *(bss_start++) = 0;

    // Initialize static objects by calling their constructors
    extern void (*__preinit_array_start[])(void);
    extern void (*__preinit_array_end[])(void);

    uintptr_t n = __preinit_array_end - __preinit_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
         __preinit_array_start[i]();
    }

    // Initialize C++ static objects
    extern void (*__init_array_start[])(void);
    extern void (*__init_array_end[])(void);

    n = __init_array_end - __init_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
        __init_array_start[i]();
    }

    // Call system startup function
    system_startup();

    // Call destructors for static objects if needed
    extern void (*__fini_array_start[])(void);
    extern void (*__fini_array_end[])(void);

    n = __fini_array_end - __fini_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
        __fini_array_start[i]();
    }

    // jump to main
    main();
}

// Simple register mapping for startup code
struct RCUMin {
    volatile uint32_t CTL;
    volatile uint32_t CFG0;
    volatile uint32_t INTR;
};

static const uintptr_t rcuRegsAddr = 0x40021000;
static RCUMin* regs = reinterpret_cast<RCUMin*>(rcuRegsAddr);

void system_startup() {
    regs->CTL |= 0x00000001;
    regs->CFG0 &= 0xF8FF0000;
    regs->CTL &= 0xFEF6FFFF;
    regs->CTL &= 0xFFFBFFFF;
    regs->CFG0 &= 0xFF80FFFF;
    regs->INTR = 0x009F0000;

    SCB->VTOR = static_cast<uint32_t>(VTOR_ADDRESS);
}
