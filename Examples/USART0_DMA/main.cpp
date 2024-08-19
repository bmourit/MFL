// gd32f303re USART0 DMA example
// Copyright (c) B. Mourit <bnmguy@gmail.com
// All rights reserved.

#include <cstdio>
#include <cstdint>

#include "CEE.hpp"
#include "GPIO.hpp"
#include "AFIO.hpp"
#include "RCU.hpp"
#include "PMU.hpp"
#include "USART.hpp"
#include "DMA.hpp"

#ifndef GD32F30x
#define GD32F30x
#endif
#ifndef GD32F30X_HD
#define GD32F30X_HD
#endif

#define ARRAYNUM(arr_name)     (uint32_t)(sizeof(arr_name) / sizeof(*(arr_name)))

uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";

usart::USART& usart0 = usart::USART::instance(usart::USART_Base::USART0_BASE);

static void USART0_Init();

int main(void) {
    // Initialize system and system bus clocks
    RCU_DEVICE.clocks_init();

    // Initialize USART0, GPIOA and AFIO
    USART0_Init();

    // DMA structure for configuration paramaters
    dma::DMA_Config dma_tx_config;
    dma_tx_config.direction = dma::Transfer_Direction::M2P;
    dma_tx_config.memory_address = reinterpret_cast<uint32_t>(txbuffer);
    dma_tx_config.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    dma_tx_config.memory_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_tx_config.count = ARRAYNUM(txbuffer);
    dma_tx_config.peripheral_address = reinterpret_cast<uint32_t>(usart0.reg_address(usart::USART_Regs::DATA));
    dma_tx_config.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    dma_tx_config.peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_tx_config.channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY;

    // Get the DMA0 instance
    dma::DMA& dma_dev = dma::DMA::instance(dma::DMA_Base::DMA0_BASE, dma_tx_config);
    // Enable DMA0 Clock
    dma_dev.set_pclk_enable(true);
    // Reset DMA
    dma_dev.reset(dma::DMA_Channel::CHANNEL3);
    // Initialize DMA
    dma_dev.init(dma::DMA_Channel::CHANNEL3);
    // Enable DMA circular mode
    dma_dev.circular_mode_enable(dma::DMA_Channel::CHANNEL3);
    // Enable channel 3 of DMA0
    dma_dev.channel_enable(dma::DMA_Channel::CHANNEL3);
    // Enable DMA transmission on USART0
    usart0.send_data_dma(usart::Bit_State::BIT_ENABLE);
    while (dma_dev.get_flag(dma::DMA_Channel::CHANNEL3, dma::State_Flags::FLAG_FTFIF) == false) {
    }

    while (1) {
        //dma_rx_config.direction = dma::Transfer_Direction::P2M;
        //dma_rx_config.memory_address = reinterpret_cast<uint32_t>(rxbuffer);
        //dma_rx_config.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
        //dma_rx_config.memory_bit_width = dma::Bit_Width::WIDTH_8BIT;
        //dma_rx_config.count = 10;
        //dma_rx_config.peripheral_address = periph_address;
        //dma_rx_config.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
        //dma_rx_config.peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT;
        //dma_rx_config.channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY;

        //dma::DMA& dma_rx = dma::DMA::instance(DMA_Base::DMA0_BASE, dma_rx_config);

        //dma_rx.deinit(dma::DMA_Channel::CHANNEL4);
        usart0.clear_flag(usart::Status_Flags::FLAG_RBNE);

        //dma_rx.init(dma::DMA_Channel::CHANNEL4);
        // Set DMA circular mode
        //dma_rx.circular_mode_enable(dma::DMA_Channel::CHANNEL4);
        // Enable DMA channel 4
        //dma_rx.channel_enable(dma::DMA_Channel::CHANNEL4);
        // Enable USART DMA for receiving
        usart0.receive_data_dma(usart::Bit_State::BIT_DISABLE);
        // Start receiving
        usart0.receive_data();
        // Wait until DMA channel transfer is complete
        //while (dma_rx.get_flag(dma::DMA_Channel::CHANNEL4, dma::State_Flags::FLAG_FTFIF) == 0) {
        //}
        printf("\n\r%s\n\r", rxbuffer);
    }
}

static void USART0_Init()
{
    // Get GPIOA instance
    gpio::GPIO& gpio_dev = gpio::GPIO::instance(gpio::GPIO_Base::GPIOA_BASE);

    // Enable GPIOA clock
    gpio_dev.set_pclk_enable(true);
    // Enable AFIO clck
    AFIO_DEVICE.set_pclk_enable(true);
    // Enable USART0 clock
    usart0.set_pclk_enable(true);

    // Reset GPIOA
    gpio_dev.reset();
    // Reset AFIO
    AFIO_DEVICE.reset();
    // Reset USART0
    usart0.reset();

    // Configure USART0 Tx as alternate function push-pull
    gpio_dev.init(gpio::Pin_Number::PIN_9, gpio::Pin_Mode::ALT_PUSHPULL, gpio::Output_Speed::SPEED_50MHZ);
    // Configure USART0 Rx as input floating
    gpio_dev.init(gpio::Pin_Number::PIN_10, gpio::Pin_Mode::INPUT_FLOATING, gpio::Output_Speed::SPEED_50MHZ);

    // Set USART0 paramaters
    usart0.set_baudrate(115200);
    usart0.set_word_length(usart::Word_Length::WL_8BITS);
    usart0.set_stop_bits(usart::Stop_Bits::STB_1BIT);
    usart0.set_parity(usart::Parity_Mode::PM_NONE);
    usart0.set_direction(usart::Direction_Mode::RXTX_MODE);
    usart0.enable();
}

#define UNUSED(x)   (void)(x)
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart0.send_data(static_cast<uint16_t>(ch));
    while (usart0.get_flag(usart::Status_Flags::FLAG_TBE) == 0);
    return ch;
    UNUSED(f);
}
