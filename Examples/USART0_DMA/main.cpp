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

usart::USART_Config usart_config = {};
// DMA rx/tx config storage
dma::DMA_Config dma_rx_config = {};
dma::DMA_Config dma_tx_config = {};

int main(void)
{
    RCU_DEVICE.clocks_init();

    // Get USART0 instance
    auto result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (result.error() != usart::USART_Error_Type::OK) {
        return -1;
    }
    usart::USART& usart0 = result.value();

    usart::USART_Pin_Config rx_pin_params = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_10,
        .mode = gpio::Pin_Mode::INPUT_PULLUP,
        .speed = gpio::Output_Speed::SPEED_50MHZ,
    };

    usart::USART_Pin_Config tx_pin_params = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_9,
        .mode = gpio::Pin_Mode::ALT_PUSHPULL,
        .speed = gpio::Output_Speed::SPEED_50MHZ,
    };

    // Set pin config
    usart_config.rx_pin_config = rx_pin_params;
    usart_config.tx_pin_config = tx_pin_params;
    // Set paramaters
    usart_config.baudrate = 115200;
    usart_config.dma_pin_ops = usart::USART_DMA_Config::DMA_DUAL;
    usart_config.word_length = usart::Word_Length::WL_8BITS;
    usart_config.stop_bits = usart::Stop_Bits::STB_1BIT;
    usart_config.parity = usart::Parity_Mode::PM_NONE;
    usart_config.direction = usart::Direction_Mode::RXTX_MODE;
    usart_config.msbf = usart::MSBF_Mode::MSBF_MSB;
    usart0.configure(&usart_config);

    // Get DMA0 instance
    auto ret = dma::DMA::get_instance(dma::DMA_Base::DMA0_BASE);
    if (ret.error() != dma::DMA_Error_Type::OK) {
        return -1;
    }
    dma::DMA& dma_dev = ret.value();

    // Reset dma
    dma_dev.reset(dma::DMA_Channel::CHANNEL3);
    // Initialize defaults
    dma_dev.init(dma::DMA_Channel::CHANNEL3);

    // TX parameters
    dma_tx_config = {
        .peripheral_address = reinterpret_cast<uint32_t>(usart0.reg_address(usart::USART_Regs::DATA)),
        .peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT,
        .memory_address = reinterpret_cast<uint32_t>(txbuffer),
        .memory_bit_width = dma::Bit_Width::WIDTH_8BIT,
        .count = ARRAYNUM(txbuffer),
        .peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE,
        .memory_increase = dma::Increase_Mode::INCREASE_ENABLE,
        .channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY,
        .direction = dma::Transfer_Direction::M2P,
    };
    dma_dev.configure(dma::DMA_Channel::CHANNEL3, &dma_tx_config);
    // Disable DMA circular mode
    dma_dev.circular_mode_disable(dma::DMA_Channel::CHANNEL3);
    // Enable channel 3 of DMA0
    dma_dev.channel_enable(dma::DMA_Channel::CHANNEL3);

    // Enable DMA transmission on USART0
    usart0.send_data_dma(usart::Bit_State::BIT_ENABLE);
    // Wait for transfer completion
    while (dma_dev.get_flag(dma::DMA_Channel::CHANNEL3, dma::State_Flags::FLAG_FTFIF) == false) {
    }

    // Disable DMA transmission on USART0
    usart0.send_data_dma(usart::Bit_State::BIT_DISABLE);

    while (1) {
        // Reset DMA channel 4
        dma_dev.reset(dma::DMA_Channel::CHANNEL4);
        // Clear RBNE flag
        usart0.clear_flag(usart::Status_Flags::FLAG_RBNE);
        // Enable USART DMA for receiving
        usart0.receive_data_dma(usart::Bit_State::BIT_DISABLE);
        // Initialize DMA channel 4
        dma_dev.init(dma::DMA_Channel::CHANNEL4);
        // RX parameters
        dma_rx_config = {
            .peripheral_address = reinterpret_cast<uint32_t>(usart0.reg_address(usart::USART_Regs::DATA)),
            .peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT,
            .memory_address = reinterpret_cast<uint32_t>(rxbuffer),
            .memory_bit_width = dma::Bit_Width::WIDTH_8BIT,
            .count = 10,
            .peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE,
            .memory_increase = dma::Increase_Mode::INCREASE_ENABLE,
            .channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY,
            .direction = dma::Transfer_Direction::P2M,
        };
        dma_dev.configure(dma::DMA_Channel::CHANNEL4, &dma_rx_config);
        // Disable DMA circular mode
        dma_dev.circular_mode_disable(dma::DMA_Channel::CHANNEL4);
        // Enable DMA channel 4
        dma_dev.channel_enable(dma::DMA_Channel::CHANNEL4);
        // Waut for transfer completion
        while (dma_dev.get_flag(dma::DMA_Channel::CHANNEL4, dma::State_Flags::FLAG_FTFIF) == false) {
        }
        // Disable DMA reception
        usart0.receive_data_dma(usart::Bit_State::BIT_DISABLE);
        printf("\n\r%s\n\r", rxbuffer);
    }
}

#define UNUSED(x)   (void)(x)

// Retarget printf to USART0
int fputc(int ch, FILE *f)
{
    // Get instance
    auto result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (result.error() != usart::USART_Error_Type::OK) {
        return -1;
    }

    usart::USART& usart0 = result.value();

    UNUSED(f);
    usart0.send_data(static_cast<uint16_t>(ch));
    while (usart0.get_flag(usart::Status_Flags::FLAG_TBE) == 0);
    return ch;
}
