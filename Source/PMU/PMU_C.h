// gd32f30x PMU peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

void set_sleep_mode_command(uint8_t value);
void set_deep_sleep_mode_command(uint8_t value);
void set_standby_mode(void);

#ifdef __cplusplus
}
#endif
