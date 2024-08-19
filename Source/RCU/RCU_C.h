// gd32f30x RCU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

// C linkage for CMSIS globals
#ifdef __cplusplus
extern "C" {
#endif

unsigned int SystemCoreClock;

void SystemInit();

#ifdef __cplusplus
}
#endif
