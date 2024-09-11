// gd32f303re USART0 DMA example
// Copyright (c) B. Mourit <bnmguy@gmail.com
// All rights reserved.

#include <cstdio>
#include <cstdint>

#include "gd32f303re.h"

#include "CORTEX.hpp"
#include "CEE.hpp"
#include "GPIO.hpp"
#include "AFIO.hpp"
#include "RCU.hpp"
#include "STARTUP.hpp"
#include "PMU.hpp"
#include "USART.hpp"
#include "DMA.hpp"

static void handle_error(const char* error_message);
static usart::USART& init_usart(bool use_dma_rx, bool use_dma_tx);
static dma::DMA& init_dma(usart::USART& usart, bool use_dma_rx, bool use_dma_tx);
static void usart_send_polling(usart::USART& usart, const char* data);
static void wait_for_dma_transfer_complete(dma::DMA& dma_dev);

constexpr size_t RX_BUFFER_SIZE = 64;
constexpr uint32_t BAUD_RATE = 115200;
constexpr uint8_t RX_DATA_SIZE = 10;
constexpr char TX_MESSAGE[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";

// Buffer declarations
uint8_t rxbuffer[RX_DATA_SIZE];
volatile uint8_t tx_count = 0;
volatile uint16_t rx_count = 0;

__attribute__((constructor(101))) void premain() {
    CORTEX_DEVICE.set_nvic_priority_group(cortex::Priority_Group::PRIO_GROUP_PRE4SUB0);
    STARTUP_DEVICE.startup_init();
}

extern "C" void USART0_IRQHandler(void) {
    // Get USART0 instance
    auto usart_result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (usart_result.error() != usart::USART_Error_Type::OK) {
        handle_error("USART0 IRQ Handler Error");
    }
    usart::USART& usart = usart_result.value();

    if (usart.get_flag(usart::Status_Flags::FLAG_RBNE)) {
        // Read data from USART
        uint8_t data = static_cast<uint8_t>(usart.receive_data() & 0xFF);

        // Store data in RX buffer if space is available
        if (rx_count < RX_BUFFER_SIZE) {
            rx_count = rx_count + 1;
            rxbuffer[rx_count] = data;
        }

        // Clear RX flag
        //usart.clear_flag(usart::Status_Flags::FLAG_RBNE);
    }
}

static usart::USART& init_usart(bool use_dma_rx, bool use_dma_tx) {
    auto usart_result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (usart_result.error() != usart::USART_Error_Type::OK) {
        handle_error("USART Initialization Failed");
    }

    usart::USART& usart = usart_result.value();

    usart::USART_Config usart_config;
    usart::USART_Pins pin_config;

    // USART0 pin configurations
    usart::USART_Pin_Config rx_pin = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_10,
        .mode = gpio::Pin_Mode::INPUT_PULLUP,
        .speed = gpio::Output_Speed::SPEED_10MHZ,
    };

    usart::USART_Pin_Config tx_pin = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_9,
        .mode = gpio::Pin_Mode::ALT_PUSHPULL,
        .speed = gpio::Output_Speed::SPEED_10MHZ,
    };

    // Set USART configuration parameters
    pin_config.rx_pin = rx_pin;
    pin_config.tx_pin = tx_pin;
    usart_config.baudrate = BAUD_RATE;
    usart_config.dma_pin_ops = use_dma_rx ? usart::USART_DMA_Config::DMA_RX :  use_dma_tx  ? usart::USART_DMA_Config::DMA_TX : usart::USART_DMA_Config::DMA_NONE;
    usart_config.word_length = usart::Word_Length::WL_8BITS;
    usart_config.stop_bits = usart::Stop_Bits::STB_1BIT;
    usart_config.parity = usart::Parity_Mode::PM_NONE;
    usart_config.direction = usart::Direction_Mode::RXTX_MODE;
    usart_config.msbf = usart::MSBF_Mode::MSBF_MSB;
    usart.reset();
    usart.clear_flag(usart::Status_Flags::FLAG_TC);
    usart.pins_configure(pin_config);
    usart.configure(usart_config);

    if (use_dma_rx || use_dma_tx) {
        // If DMA is used, initialize DMA here
        dma::DMA& dma_dev = init_dma(usart, use_dma_rx, use_dma_tx);
        // Wait for DMA transfer completion if receiving
        if (use_dma_rx) {
            wait_for_dma_transfer_complete(dma_dev);
        }
    } else {
        usart.set_interrupt_enable(usart::Interrupt_Type::INTR_RBNEIE, true);
    }

    return usart;
}

static dma::DMA& init_dma(usart::USART& usart, bool use_dma_rx, bool use_dma_tx) {
    auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA0_BASE);
    if (dma_result.error() != dma::DMA_Error_Type::OK) {
        handle_error("DMA Initialization Failed");
    }

    dma::DMA& dma_dev = dma_result.value();

    dma::DMA_Config dma_rxtx_config;

    if (use_dma_tx) {
        handle_error("DMA on TX pin is not supported in this example");
    }
    // DMA RX Configuration
    if (use_dma_rx) {
        dma_dev.reset(dma::DMA_Channel::CHANNEL4);
        usart.clear_flag(usart::Status_Flags::FLAG_RBNE);
        usart.receive_data_dma(true);

        dma_rxtx_config = {
            .peripheral_address = reinterpret_cast<uint32_t>(usart.reg_address(usart::USART_Regs::DATA)),
            .peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT,
            .memory_address = reinterpret_cast<uint32_t>(rxbuffer),
            .memory_bit_width = dma::Bit_Width::WIDTH_8BIT,
            .count = RX_DATA_SIZE,
            .peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE,
            .memory_increase = dma::Increase_Mode::INCREASE_ENABLE,
            .channel_priority = dma::Channel_Priority::ULTRA_HIGH_PRIORITY,
            .direction = dma::Transfer_Direction::P2M,
        };

        dma_dev.configure(dma::DMA_Channel::CHANNEL4, dma_rxtx_config);
        dma_dev.set_circulation_mode_enable(dma::DMA_Channel::CHANNEL4, false);
        dma_dev.set_channel_enable(dma::DMA_Channel::CHANNEL4, true);
    }

    return dma_dev;
}

static void wait_for_dma_transfer_complete(dma::DMA& dma_dev) {
    while (!dma_dev.get_flag(dma::DMA_Channel::CHANNEL4, dma::Status_Flags::FLAG_FTFIF)) {
        // Could implement a timeout here to avoid infinite loop
    }
}

static void usart_send_polling(usart::USART& usart, const char* data) {
    while (*data != '\0') {
        usart.send_data(static_cast<uint16_t>(*data++));
        while (!usart.get_flag(usart::Status_Flags::FLAG_TC)) {
            // Wait until transmission buffer is empty
        }
    }
}

static void handle_error(const char* error_message) {
    // Implement error handling, such as logging to a file, flashing an LED, etc.
    printf("Error: %s\n", error_message);
    // Optionally halt execution
    while (true) {
        // Infinite loop to indicate error condition
    }
}

int main() {

    // Initialize USART with DMA RX and non-DMA TX
    usart::USART& usart = init_usart(false, false);

    NVIC_EnableIRQ(USART0_IRQn);

    // Transmit message using non-DMA USART (USART0 TX)
    usart_send_polling(usart, TX_MESSAGE);
    while (rx_count < RX_DATA_SIZE) {
        // Wait for all data to be received
    }

    // Output received data
    printf("\n\r%s\n\r", rxbuffer);
    return 0;
}

// Retarget printf to USART0 using __io_putchar
int __io_putchar(int ch) {
    // Get USART0 instance
    auto usart_result = usart::USART::get_instance(usart::USART_Base::USART0_BASE);
    if (usart_result.error() != usart::USART_Error_Type::OK) {
        return -1;
    }
    usart::USART& usart0 = usart_result.value();

    usart0.send_data(static_cast<uint16_t>(ch));
    while (!usart0.get_flag(usart::Status_Flags::FLAG_TC)) {
    }
    return ch;
}
