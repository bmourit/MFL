// gd32f30x SDIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "SDIO.hpp"

// Initialize the static member
bool sdio::SDIO::is_clock_enabled = false;

namespace sdio {

void SDIO::reset() {
    // Set to reset values (0x00000000)
    write_register(*this, SDIO_Regs::PWRCTL, 0, true);
    write_register(*this, SDIO_Regs::CMDAGMT, 0, true);
    write_register(*this, SDIO_Regs::CMDCTL, 0, true);
    write_register(*this, SDIO_Regs::DATATO, 0, true);
    write_register(*this, SDIO_Regs::DATALEN, 0, true);
    write_register(*this, SDIO_Regs::DATACTL, 0, true);
    write_register(*this, SDIO_Regs::INTC, 0, true);
    write_register(*this, SDIO_Regs::INTEN, 0, true);
};

void SDIO::pin_config_init() {
    auto configure_pin = [](gpio::GPIO& port, const SDIO_Pin_Config& pin_config) {
        port.init_pin(pin_config.pin, pin_config.mode, pin_config.speed);
    };

    auto cmd_result = gpio::GPIO::get_instance(pin_config_.cmd_pin.gpio_port);
    if (cmd_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& cmd_port = cmd_result.value();
    configure_pin(cmd_port, pin_config_.cmd_pin);

    // Check if the next pin uses a different port
    if (pin_config_.ck_pin.gpio_port != pin_config_.cmd_pin.gpio_port) {
        auto ck_result = gpio::GPIO::get_instance(pin_config_.ck_pin.gpio_port);
        if (ck_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& ck_port = ck_result.value();
        configure_pin(ck_port, pin_config_.ck_pin);
    } else {
        configure_pin(cmd_port, pin_config_.ck_pin);
    }
    // D0
    auto d0_result = gpio::GPIO::get_instance(pin_config_.d0_pin.gpio_port);
    if (d0_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d0_port = d0_result.value();
    configure_pin(d0_port, pin_config_.d0_pin);
    // D1
    auto d1_result = gpio::GPIO::get_instance(pin_config_.d1_pin.gpio_port);
    if (d1_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d1_port = d1_result.value();
    configure_pin(d1_port, pin_config_.d1_pin);
    // D2
    auto d2_result = gpio::GPIO::get_instance(pin_config_.d2_pin.gpio_port);
    if (d2_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d2_port = d2_result.value();
    configure_pin(d2_port, pin_config_.d2_pin);
    // D3
    auto d3_result = gpio::GPIO::get_instance(pin_config_.d3_pin.gpio_port);
    if (d3_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d3_port = d3_result.value();
    configure_pin(d3_port, pin_config_.d3_pin);

    // Check pins exist
    // All of these pins must exist or none of them get configured
    //if ((pin_config_.d4_pin) && (pin_config_.d5_pin) && (pin_config_.d6_pin) && (pin_config_.d7_pin)) {
    // D4
    auto d4_result = gpio::GPIO::get_instance(pin_config_.d4_pin.gpio_port);
    if (d4_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d4_port = d4_result.value();
    configure_pin(d4_port, pin_config_.d4_pin);
    // D5
    auto d5_result = gpio::GPIO::get_instance(pin_config_.d5_pin.gpio_port);
    if (d5_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d5_port = d5_result.value();
    configure_pin(d5_port, pin_config_.d5_pin);
    // D6
    auto d6_result = gpio::GPIO::get_instance(pin_config_.d6_pin.gpio_port);
    if (d6_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d6_port = d6_result.value();
    configure_pin(d6_port, pin_config_.d6_pin);
    // D7
    auto d7_result = gpio::GPIO::get_instance(pin_config_.d7_pin.gpio_port);
    if (d7_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d7_port = d7_result.value();
    configure_pin(d7_port, pin_config_.d7_pin);
    //}
}

void SDIO::clock_configure(Clock_Edge edge, bool bypass, bool low_power, uint16_t divider)
{
    // Configure the parameters
    write_bits(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::CLKEDGE), static_cast<uint32_t>(edge),
               static_cast<uint32_t>(CLKCTL_Bits::CLKBYP), bypass ? Set : Clear,
               static_cast<uint32_t>(CLKCTL_Bits::CLKPWRSAV), low_power ? Set : Clear);
    // Check if we need to set DIV8 bit
    if (divider >= 256) {
        write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV8), 1, true);
        divider -= 256;
    }
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV), static_cast<uint32_t>(divider), true);
}

void SDIO::set_hardware_clock_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::HWCLKEN), enable ? 1 : 0, true);
}

void SDIO::set_bus_width(Bus_Width width)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::BUSMODE), static_cast<uint32_t>(width), true);
}

uint32_t SDIO::get_power_mode()
{
    return read_bit(*this, SDIO_Regs::PWRCTL, static_cast<uint32_t>(PWRCTL_Bits::PWRCTL), true);
}

void SDIO::set_power_mode(Power_Control power)
{
    write_bit(*this, SDIO_Regs::PWRCTL, static_cast<uint32_t>(PWRCTL_Bits::PWRCTL), static_cast<uint32_t>(power), true);
}

void SDIO::set_clock_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::CLKEN), enable ? 1 : 0, true);
}

void SDIO::set_command_config(Command_Index index, uint32_t argument, Command_Response response, Wait_Type type)
{
    write_register(*this, SDIO_Regs::CMDAGMT, argument, true);
    write_bits(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::CMDIDX), static_cast<uint32_t>(index),
               static_cast<uint32_t>(CMDCTL_Bits::CMDRESP), static_cast<uint32_t>(response),
               static_cast<uint32_t>(CMDCTL_Bits::WAITTYPE), static_cast<uint32_t>(type));
}

void SDIO::send_command(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::CSMEN), enable ? 1 : 0, true);
}

uint8_t SDIO::get_command_index()
{
    return read_bit8(*this, SDIO_Regs::RSPCMDIDX, static_cast<uint32_t>(RSPCMDIDX_Bits::RSPCMDIDX), true);
}

uint32_t SDIO::get_response(Response_Type type)
{
    return read_register<uint32_t>(*this,
                                   (type == Response_Type::RESPONSE0) ? SDIO_Regs::RESP0 :
                                   (type == Response_Type::RESPONSE1) ? SDIO_Regs::RESP1 :
                                   (type == Response_Type::RESPONSE2) ? SDIO_Regs::RESP2 : SDIO_Regs::RESP3,
                                   true);
}

void SDIO::data_configure(uint32_t timeout, uint32_t length, Block_Size size)
{
    write_register(*this, SDIO_Regs::DATATO, timeout, true);
    write_bit(*this, SDIO_Regs::DATALEN, static_cast<uint32_t>(DATALEN_Bits::DATALEN), length, true);
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ), static_cast<uint32_t>(size), true);
}

void SDIO::data_transfer_configure(Transfer_Mode mode, Transfer_Direction direction)
{
    write_bits(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::TRANSMOD), static_cast<uint32_t>(mode),
               static_cast<uint32_t>(DATACTL_Bits::DATADIR), static_cast<uint32_t>(direction));
}

void SDIO::set_data_state_machine_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::DATAEN), enable ? 1 : 0, true);
}

uint32_t SDIO::read_data_word()
{
    return read_register<uint32_t>(*this, SDIO_Regs::FIFO, true);
}

void SDIO::write_data_word(uint32_t data)
{
    write_register(*this, SDIO_Regs::FIFO, data, true);
}

uint32_t SDIO::get_data_count()
{
    return read_bit(*this, SDIO_Regs::DATACNT, static_cast<uint32_t>(DATACNT_Bits::DATACNT), true);
}

uint32_t SDIO::get_fifo_count()
{
    return read_bit(*this, SDIO_Regs::FIFOCNT, static_cast<uint32_t>(FIFOCNT_Bits::FIFOCNT), true);
}

void SDIO::set_dma_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::DMAEN), enable ? 1 : 0, true);
}

void SDIO::set_read_wait_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWEN), enable ? 1 : 0, true);
}

void SDIO::set_stop_read_wait_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWSTOP), enable ? 1 : 0, true);
}

void SDIO::set_read_wait_type(Read_Wait type)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWTYPE), static_cast<uint32_t>(type), true);
}

// IO mode
void SDIO::set_IO_operation_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::IOEN), enable ? 1 : 0, true);
}

void SDIO::set_IO_suspend_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::SUSPEND), enable ? 1 : 0, true);
}

// CE-ATA mode
void SDIO::set_CE_ATA_command_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::ATAEN), enable ? 1 : 0, true);
}

void SDIO::set_CE_ATA_interrupt_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::NINTEN), enable ? 0 : 1, true);
}

void SDIO::set_CE_ATA_command_completion_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::ENCMDC), enable ? 1 : 0, true);
}

bool SDIO::get_flag(Status_Flags flag) const
{
    return (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(flag), true) != 0);
}

void SDIO::clear_flag(Clear_Flags flag)
{
    write_bit(*this, SDIO_Regs::INTC, static_cast<uint32_t>(flag), 1, true);
}

void SDIO::clear_all_flags()
{
    write_register(*this, SDIO_Regs::INTC, static_cast<uint32_t>(All_Flags_Mask), true);
};

bool SDIO::get_interrupt_flag(Interrupt_Flags flag)
{
    return (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(flag), true) != 0);
}

void SDIO::clear_interrupt_flag(Clear_Flags flag)
{
    write_bit(*this, SDIO_Regs::INTC, static_cast<uint32_t>(flag), 1, true);
}

void SDIO::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    write_bit(*this, SDIO_Regs::INTEN, static_cast<uint32_t>(type), enable ? 1 : 0, true);
}

} // namespace sdio

// Instantiate class for global device
sdio::SDIO SDIO_DEVICE;
