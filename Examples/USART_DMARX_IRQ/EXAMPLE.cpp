// gd32f303re EXAMPLE class
// Copyright (c) B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "EXAMPLE.hpp"

namespace example {

constexpr uint32_t BAUD_RATE = 115200;
constexpr uint8_t RX_DATA_SIZE = 10;
constexpr char TX_MESSAGE[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";

volatile uint8_t tx_count = 0;
volatile uint16_t rx_count = 0;
char rxbuffer[RX_DATA_SIZE];
char txbuffer[sizeof(TX_MESSAGE)];

void EXAMPLE::premain() {
    cortex::CORTEX_DEVICE().set_nvic_priority_group(cortex::Priority_Group::PRIO_GROUP_PRE4SUB0);
    startup::STARTUP_DEVICE().startup_init();
    SysTick_Config(rcu::RCU_DEVICE().SystemCoreClock / 1000);
}

void EXAMPLE::init() {
    // Call AFIO_DEVICE to enable its clock
    gpio::AFIO_DEVICE();

    usart_pins_.rx_pin = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_10,
        .mode = gpio::Pin_Mode::INPUT_PULLUP,
        .speed = gpio::Output_Speed::SPEED_50MHZ,
    };

    usart_pins_.tx_pin = {
        .gpio_port = gpio::GPIO_Base::GPIOA_BASE,
        .pin = gpio::Pin_Number::PIN_9,
        .mode = gpio::Pin_Mode::ALT_PUSHPULL,
        .speed = gpio::Output_Speed::SPEED_50MHZ,
    };

    usart_base_ = usart::USART_Base::USART0_BASE;

    usart_config_.baudrate = BAUD_RATE;
    usart_config_.dma_pin_ops = usart::USART_DMA_Config::DMA_RX;
    usart_config_.word_length = usart::Word_Length::WL_8BITS;
    usart_config_.stop_bits = usart::Stop_Bits::STB_1BIT;
    usart_config_.parity = usart::Parity_Mode::PM_NONE;
    usart_config_.direction = usart::Direction_Mode::RXTX_MODE;
    usart_config_.msbf = usart::MSBF_Mode::MSBF_MSB;

    setup_usart();
    transfer();
}

void EXAMPLE::setup_usart() {
    auto usart_result = usart::USART::get_instance(usart_base_);
    if (usart_result.error() != usart::USART_Error_Type::OK) {
        return;
    }

    usart::USART& usart = usart_result.value();
    usart_ = &usart;

    usart_->reset();

    usart_->clear_flag(usart::Status_Flags::FLAG_TBE);
    usart_->pins_configure(usart_pins_);
    usart_->configure(usart_config_);

    setup_dma();
}

void EXAMPLE::setup_dma() {
    auto dma_result = dma::DMA::get_instance(dma::DMA_Base::DMA0_BASE);
    if (dma_result.error() != dma::DMA_Error_Type::OK) {
        return;
    }

    dma::DMA& dma_dev = dma_result.value();
    dma_ = &dma_dev;

    dma_->reset(dma::DMA_Channel::CHANNEL3);
    dma_config_.peripheral_address = reinterpret_cast<uint32_t>(usart_->reg_address(usart::USART_Regs::DATA));
    dma_config_.peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_config_.memory_address = reinterpret_cast<uint32_t>(txbuffer);
    dma_config_.memory_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_config_.count = sizeof(TX_MESSAGE) - 1;
    dma_config_.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    dma_config_.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    dma_config_.channel_priority = dma::Channel_Priority::LOW_PRIORITY;
    dma_config_.direction = dma::Transfer_Direction::M2P;
    dma_->configure(dma::DMA_Channel::CHANNEL3, dma_config_);

    dma_->reset(dma::DMA_Channel::CHANNEL4);
    dma_config_.peripheral_address = reinterpret_cast<uint32_t>(usart_->reg_address(usart::USART_Regs::DATA));
    dma_config_.peripheral_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_config_.memory_address = reinterpret_cast<uint32_t>(rxbuffer);
    dma_config_.memory_bit_width = dma::Bit_Width::WIDTH_8BIT;
    dma_config_.count = RX_DATA_SIZE;
    dma_config_.peripheral_increase = dma::Increase_Mode::INCREASE_DISABLE;
    dma_config_.memory_increase = dma::Increase_Mode::INCREASE_ENABLE;
    dma_config_.channel_priority = dma::Channel_Priority::LOW_PRIORITY;
    dma_config_.direction = dma::Transfer_Direction::P2M;
    dma_->configure(dma::DMA_Channel::CHANNEL4, dma_config_);

    dma_->set_circulation_mode_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_->set_memory_to_memory_enable(dma::DMA_Channel::CHANNEL3, false);
    dma_->set_circulation_mode_enable(dma::DMA_Channel::CHANNEL4, false);
    dma_->set_memory_to_memory_enable(dma::DMA_Channel::CHANNEL4, false);
}

void EXAMPLE::transfer() {
    std::memcpy(txbuffer, TX_MESSAGE, sizeof(TX_MESSAGE));

    // RX
    usart_->receive_data_dma(true);
    dma_->set_interrupt_enable(dma::DMA_Channel::CHANNEL4, dma::Interrupt_Type::INTR_FTFIE, true);
    dma_->set_channel_enable(dma::DMA_Channel::CHANNEL4, true);

    // TX
    usart_->send_data_dma(true);
    dma_->set_interrupt_enable(dma::DMA_Channel::CHANNEL3, dma::Interrupt_Type::INTR_FTFIE, true);
    dma_->set_channel_enable(dma::DMA_Channel::CHANNEL3, true);

    while (is_transfer_complete == false);

    is_transfer_complete = false;

    while (is_transfer_complete == false);

    printf("\n\r%s\n\r", rxbuffer);
    while (1);
}

EXAMPLE& EXAMPLE_DEVICE() {
    static EXAMPLE instance;
    return instance;
}

} // namespace example
