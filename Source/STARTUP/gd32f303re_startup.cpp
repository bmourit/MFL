// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdint>

#include "RCU.hpp"
#include "gd32f303re.h"
#include "gd32f303re_vector_table.hpp"

extern int main();

extern "C" void Reset_Handler()
{
    // Initialize data section
    extern uint32_t _sidata;
    extern uint32_t _sdata;
    extern uint32_t _edata;

    uint32_t *src = &_sidata;
    uint32_t *dest = &_sdata;
    while (dest < &_edata)
        *(dest++) = *(src++); //(&_etext, &_etext + size, &_sdata);

    // Initialize bss section
    extern uint32_t __bss_start__;
    extern uint32_t __bss_end__;
    dest = &__bss_start__;
    while (dest < &__bss_end__)
        *(dest++) = 0;

    // Initialize static objects by calling their constructors
    extern void (*__preinit_array_start[])(void);
    extern void (*__preinit_array_end[])(void);
    uintptr_t n = __preinit_array_end - __preinit_array_start;
    uintptr_t i = 0;
    while (i < n)
        __preinit_array_start[i++]();

    // Call system startup
    system_startup();

    extern void (*__init_array_start[])(void);
    extern void (*__init_array_end[])(void);
    n = __init_array_end - __init_array_start;
    i = 0;
    while (i < n)
        __init_array_start[i++]();

    extern void (*__fini_array_start[])(void);
    extern void (*__fini_array_end[])(void);
    n = __fini_array_end - __fini_array_start;
    i = 0;
    while (i < n)
        __fini_array_start[i++]();

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

static RCUMin* regs = (RCUMin*)rcuRegsAddr;

void system_startup()
{
    regs->CTL |= 0x00000001;
    regs->CFG0 &= 0xF8FF0000;
    regs->CTL &= 0xFEF6FFFF;
    regs->CTL &= 0xFFFBFFFF;
    regs->CFG0 &= 0xFF80FFFF;
    regs->INTR = 0x009F0000;

    SCB->VTOR = VTOR_ADDRESS;
}

