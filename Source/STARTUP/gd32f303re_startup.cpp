// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdint>

#include "RCU.hpp"
#include "gd32f303re.h"
#include "gd32f303re_vector_table.hpp"

extern int main();

extern "C"
void Reset_Handler()
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

    //std::fill(&__bss_start__, &__bss_end__, UINT8_C(0x00));

    // Initialize static objects by calling their constructors
    extern void (*__preinit_array_start[])(void);
    extern void (*__preinit_array_end[])(void);
    uintptr_t n = __preinit_array_end - __preinit_array_start;
    uintptr_t i = 0;
    while (i < n)
        __preinit_array_start[i++]();

    // Call system startup
    RCU_DEVICE.system_startup();

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