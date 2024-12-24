// gd32f303re EXAMPLE class
// Copyright (c) B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdio>
#include <cstdint>

#include "F303RE.hpp"
#include "interrupts.hpp"

#include "CORTEX.hpp"
#include "GPIO.hpp"
#include "AFIO.hpp"
#include "RCU.hpp"
#include "STARTUP.hpp"
#include "USART.hpp"
#include "DMA.hpp"

namespace example {

class EXAMPLE {
public:
    EXAMPLE() {}

    void premain();
    void init();
    void setup_usart();
    void setup_dma();
    void transfer();

    volatile bool is_transfer_complete = false;

    usart::USART* usart_;
    usart::USART_Base usart_base_;
    usart::USART_Pins usart_pins_;
    usart::USART_Config usart_config_;
    dma::DMA* dma_;
    dma::DMA_Config dma_config_;
};

EXAMPLE& EXAMPLE_DEVICE();

} // namespace example
