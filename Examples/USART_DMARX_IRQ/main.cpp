//
// MFL gd32f30x USART DMA RX example
//
// Copyright (C) 2025 B. Mouritsen <bnmguy@gmail.com>. All rights reserved.
//
// This file is part of the Microcontroller Firmware Library (MFL).
//
// MFL is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// MFL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with MFL.
// If not, see <https://www.gnu.org/licenses/>.
//

#include <cstdio>
#include "USART.hpp"
#include "GPIO.hpp"
#include "DMA.hpp"
#include "CORTEX.hpp"
#include "STARTUP.hpp"
#include "RCU.hpp"
#include "F303RE.hpp"
#include "RingBuffer.hpp"

// Override the default buffer sizes
#define USART_RX_BUFFER_SIZE    128
#define USART_TX_BUFFER_SIZE    64

void startup_init();
void configure_pins();
void setup_dma();
void update_dma_rx_buffer();
void usart_handle_interrupt();

usart::USART& usart_instance = usart::USART::get_instance(usart::USART_Base::USART0_BASE).value();
dma::DMA& dma_instance = dma::DMA::get_instance(dma::DMA_Base::DMA0_BASE, dma::DMA_Channel::CHANNEL4).value();

// Retarget printf to USART0 using __io_putchar
int __io_putchar(int value) {
    while (usart_instance.get_config().state != usart::USART_State::IDLE) {
        // Wait for USART to become idle
    }
    while (!usart_instance.usart_transmit_interrupt(static_cast<uint8_t>(value))) {
        // Wait for room in buffer
    }
    return value;
}

int main() {
    // Low level startup
    startup_init();

    // Configure pins
    configure_pins();

    // Configure USART
    usart::USART_Config config = usart::default_config;
    config.baudrate = 115200;
    config.dma_ops = usart::USART_DMA_Config::DMA_RX;
    config.direction = usart::Direction_Mode::RXTX_MODE;
    config.state = usart::USART_State::IDLE;

    // Initialize USART
    usart_instance.init(config);

    // Disable hardware flow control
    usart_instance.set_hwfc_rts_enable(false);
    usart_instance.set_hwfc_cts_enable(false);

    // Configure DMA
    setup_dma();

    NVIC_DisableIRQ(USART0_IRQn);
    NVIC_SetPriority(USART0_IRQn, 1);
    NVIC_EnableIRQ(USART0_IRQn);

    printf("USART DMA RX Example Started\n");

    while (1) {
        // Update DMA RX buffer
        update_dma_rx_buffer();
        // Check for received data
        uint8_t data;
        if (usart_instance.read_rx_buffer(data)) {
            printf("Received: %c\n", data);
        }
    }

    return 0;
}

void startup_init() {
    cortex::CORTEX::get_instance().set_nvic_priority_group(cortex::Priority_Group::PRIO_GROUP_PRE4SUB0);
    startup::STARTUP::get_instance().startup_init();
    uint32_t clock = rcu::RCU::get_instance().get_system_clock();
    SysTick_Config(clock / 1000U);
}

void configure_pins() {
    gpio::GPIO& gpio_instance = gpio::GPIO::get_instance(gpio::GPIO_Base::GPIOA_BASE).value();
    // RX pin mode setup
    gpio_instance.set_pin_mode(gpio::Pin_Number::PIN_10, gpio::Pin_Mode::INPUT_PULLUP, gpio::Output_Speed::SPEED_MAX);
    // TX pin mode setup
    gpio_instance.set_pin_mode(gpio::Pin_Number::PIN_9, gpio::Pin_Mode::ALT_PUSHPULL, gpio::Output_Speed::SPEED_MAX);
}

void setup_dma() {
    // DMA setup
    dma_instance.clear_channel();
    dma_instance.init({
        usart::RxBufferSize,
        reinterpret_cast<uint32_t>(usart_instance.get_buffer_data(true)),
        reinterpret_cast<uint32_t>(usart_instance.reg_address(usart::USART_Regs::DATA)),
        dma::Bit_Width::WIDTH_8BIT,
        dma::Bit_Width::WIDTH_8BIT,
        dma::Increase_Mode::INCREASE_DISABLE,
        dma::Increase_Mode::INCREASE_ENABLE,
        dma::Channel_Priority::MEDIUM_PRIORITY,
        dma::Transfer_Direction::P2M,
    });
    
    dma_instance.set_memory_to_memory_enable(false);
    dma_instance.set_circulation_mode_enable(true);
    usart_instance.receive_data_dma_enable(true);
    dma_instance.set_channel_enable(true);
}

void update_dma_rx_buffer() {
    usart_instance.modify_rx_buffer_head(USART_RX_BUFFER_SIZE - dma_instance.get_transfer_count());
}

void usart_handle_interrupt() {
    usart_instance.handle_interrupt();
}


extern "C" {
    void USART0_IRQHandler() {
        NVIC_ClearPendingIRQ(USART0_IRQn);
        usart_instance.handle_interrupt();
    }
} // extern "C"
