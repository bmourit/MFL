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

// USART0 configuration storage for init()
usart::USART_Config usart_config = {};
// USART0 instance
usart::USART& usart0 = usart::USART::instance(usart::USART_Base::USART0_BASE, usart_config);
// DMA rx/tx config storage
dma::DMA_Config dma_rx_config = {};
dma::DMA_Config dma_tx_config = {};
// DMA0 instance
dma::DMA& dma_dev = dma::DMA::instance(dma::DMA_Base::DMA0_BASE, dma_tx_config);

static inline void setup_serial_port();

int main(void) {
    // Initialize system clocks
    RCU_DEVICE.clocks_init();

    // Enable DMA0 Clock
    dma_dev.set_pclk_enable(true);

    // Setup the serial port
    setup_serial_port();

    // Reset DMA
    dma_dev.reset(dma::DMA_Channel::CHANNEL3);
    // Initialize DMA with defaults
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

static inline void setup_serial_port()
{
    // Enable USART0 clock
    usart0.set_pclk_enable(true);
    usart0.reset();

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

    // Setup gpio
    usart_config.rx_pin_config = rx_pin_params;
    usart_config.tx_pin_config = tx_pin_params;

    // Set USART0 paramaters
    usart_config.baudrate = 115200;
    usart_config.dma_pin_ops = usart::USART_DMA_Config::DMA_DUAL;
    usart_config.word_length = usart::Word_Length::WL_8BITS;
    usart_config.stop_bits = usart::Stop_Bits::STB_1BIT;
    usart_config.parity = usart::Parity_Mode::PM_NONE;
    usart_config.direction = usart::Direction_Mode::RXTX_MODE;
    usart_config.msbf = usart::MSBF_Mode::MSBF_MSB;

    usart0.configure(&usart_config);
}

#define UNUSED(x)   (void)(x)
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    UNUSED(f);
    usart0.send_data(static_cast<uint16_t>(ch));
    while (usart0.get_flag(usart::Status_Flags::FLAG_TBE) == 0);
    return ch;
}
