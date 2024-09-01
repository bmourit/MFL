// gd32f30x CONFIG definitions
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include "gd32f303re.h"

#define REG_BIT_DEF(start, end)  ((start << 16) | (end - start + 1))

//#define DISABLE_SDIO_CARD_DRIVER
// Uncomment if you need the vector table in SRAM
//#define VECT_TAB_SRAM

#define	DISABLE_CEE_ENHANCE

// Set the oppropriate offset here.
// This should match the offset expected by the bootloader.
// If no bootloader exists, use 0x00000000
constexpr uintptr_t VECT_TAB_OFFSET = 0x00007000;

// DO NOT CHANGE THESE
constexpr uintptr_t NVIC_VECTTAB_SRAM = 0x20000000;
constexpr uintptr_t NVIC_VECTTAB_FLASH = 0x08000000;


#ifdef VECT_TAB_SRAM
constexpr uintptr_t VTOR_ADDRESS = NVIC_VECTTAB_SRAM + VECT_TAB_OFFSET;
#else
constexpr uintptr_t VTOR_ADDRESS = NVIC_VECTTAB_FLASH + VECT_TAB_OFFSET;
#endif
