// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <algorithm>
#include <cstdint>

#include "RCU.hpp"
#include "BUZZER.hpp"

extern int main();

extern "C" {
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    extern uint32_t __bss_start__;
    extern uint32_t __bss_end__;
    extern uint32_t _etext;
    extern uint32_t end;
    extern uint32_t _end;
    extern uint32_t _sidata;
    extern uint32_t _sdata;
    extern uint32_t _edata;
}

extern "C" void system_startup();

extern "C" void Reset_Handler() {
    // Initialize data section
    uint32_t *src = &_sidata;
    uint32_t *dest = &_sdata;
    while (dest < &_edata)
        *dest++ = *src++;

    uint32_t *bss_start = &_sbss;
    uint32_t *bss_end = &_ebss;
    while (bss_start < bss_end)
        *(bss_start++) = 0;

    // Initialize static objects by calling their constructors
    extern void (*__preinit_array_start[])(void);
    extern void (*__preinit_array_end[])(void);

    uintptr_t n = __preinit_array_end - __preinit_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
        __preinit_array_start[i]();
    }

    // Call system startup function
    system_startup();

    // Initialize C++ static objects
    extern void (*__init_array_start[])(void);
    extern void (*__init_array_end[])(void);

    n = __init_array_end - __init_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
        __init_array_start[i]();
    }

    // Call destructors for static objects if needed
    extern void (*__fini_array_start[])(void);
    extern void (*__fini_array_end[])(void);

    n = __fini_array_end - __fini_array_start;
    for (uintptr_t i = 0; i < n; ++i) {
        __fini_array_start[i]();
    }

    // jump to main
    main();

    while (true);
}

extern "C" {
// Simple register mapping for startup code
    struct RCUMin {
        volatile uint32_t CTL;
        volatile uint32_t CFG0;
        volatile uint32_t INTR;
    };

    static const uintptr_t rcuRegsAddr = 0x40021000;
    static RCUMin* regs = reinterpret_cast<RCUMin*>(rcuRegsAddr);
}

void system_startup() {
    regs->CTL |= 0x00000001U;
    regs->CFG0 &= 0xF8FF0000U;
    regs->CTL &= 0xFEF6FFFFU;
    regs->CTL &= 0xFFFBFFFFU;
    regs->CFG0 &= 0xFF80FFFFU;
    regs->INTR = 0x009F0000U;

    SCB->VTOR = 0x08007000U;
}
