// gd32f303re USART0 DMA example
// Copyright (c) B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdio>
#include <cstdint>

#include "EXAMPLE.hpp"

__attribute__((constructor(101))) void premain() {
    example::EXAMPLE_DEVICE().premain();
}

int main() {
    cortex::CORTEX_DEVICE().nvic_irq_enable(DMA0_Channel3_IRQn, 0, 0);
    cortex::CORTEX_DEVICE().nvic_irq_enable(DMA0_Channel4_IRQn, 0, 1);

    example::EXAMPLE_DEVICE().init();

    return 0;
}

// Retarget printf to USART0 using __io_putchar
int __io_putchar(int ch) {

    // Get USART0 instance
    auto usart_result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (usart_result.error() != usart::USART_Error_Type::OK) {
        return 0;
    }

    usart::USART& usart0 = usart_result.value();

    usart0.send_data(static_cast<uint8_t>(ch));
    while (usart0.get_flag(usart::Status_Flags::FLAG_TBE) == false);
    return ch;
}
