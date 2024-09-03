// gd32f30x SDIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "SDIO.hpp"

// Set a default
constexpr gpio::Output_Speed defaultSdioGpioSpeed = gpio::Output_Speed::SPEED_50MHZ;

namespace sdio {

void SDIO::init()
{
    auto result = gpio::GPIO::get_instance(config_.CMD_Pin->gpio_port);
    if (result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& port = result.value();
    // Initialize GPIO pin
    port.init_pin(config_.CMD_Pin->pin, config_.CMD_Pin->mode, defaultSdioGpioSpeed);
    // Check if the next pin uses a different port
    if (config_.CK_Pin->gpio_port != config_.CMD_Pin->gpio_port) {
        auto result1 = gpio::GPIO::get_instance(config_.CK_Pin->gpio_port);
        if (result1.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& ck_port = result1.value();
        ck_port.init_pin(config_.CK_Pin->pin, config_.CK_Pin->mode, defaultSdioGpioSpeed);
    } else {
        // Same port
        port.init_pin(config_.CK_Pin->pin, config_.CK_Pin->mode, defaultSdioGpioSpeed);
    }
    // D0
    auto d0_result = gpio::GPIO::get_instance(config_.D0_Pin->gpio_port);
    if (d0_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d0_port = d0_result.value();
    d0_port.init_pin(config_.D0_Pin->pin, config_.D0_Pin->mode, defaultSdioGpioSpeed);
    // D1
    auto d1_result = gpio::GPIO::get_instance(config_.D1_Pin->gpio_port);
    if (d1_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d1_port = d1_result.value();
    d1_port.init_pin(config_.D1_Pin->pin, config_.D1_Pin->mode, defaultSdioGpioSpeed);
    // D2
    auto d2_result = gpio::GPIO::get_instance(config_.D2_Pin->gpio_port);
    if (d2_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d2_port = d2_result.value();
    d2_port.init_pin(config_.D2_Pin->pin, config_.D2_Pin->mode, defaultSdioGpioSpeed);
    // D3
    auto d3_result = gpio::GPIO::get_instance(config_.D3_Pin->gpio_port);
    if (d3_result.error() != gpio::GPIO_Error_Type::OK) {
        return;
    }
    gpio::GPIO& d3_port = d3_result.value();
    d3_port.init_pin(config_.D3_Pin->pin, config_.D3_Pin->mode, defaultSdioGpioSpeed);

    // Check pins exist
    // All of these pins must exist or none of them get configured
    if ((config_.D4_Pin != nullptr) && (config_.D5_Pin != nullptr) && (config_.D6_Pin != nullptr) && (config_.D7_Pin != nullptr)) {
        // D4
        auto d4_result = gpio::GPIO::get_instance(config_.D4_Pin->gpio_port);
        if (d4_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& d4_port = d4_result.value();
        d4_port.init_pin(config_.D4_Pin->pin, config_.D4_Pin->mode, defaultSdioGpioSpeed);
        // D5
        auto d5_result = gpio::GPIO::get_instance(config_.D5_Pin->gpio_port);
        if (d5_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& d5_port = d5_result.value();
        d5_port.init_pin(config_.D5_Pin->pin, config_.D5_Pin->mode, defaultSdioGpioSpeed);
        // D6
        auto d6_result = gpio::GPIO::get_instance(config_.D6_Pin->gpio_port);
        if (d6_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& d6_port = d6_result.value();
        d6_port.init_pin(config_.D6_Pin->pin, config_.D6_Pin->mode, defaultSdioGpioSpeed);
        // D7
        auto d7_result = gpio::GPIO::get_instance(config_.D7_Pin->gpio_port);
        if (d7_result.error() != gpio::GPIO_Error_Type::OK) {
            return;
        }
        gpio::GPIO& d7_port = d7_result.value();
        d7_port.init_pin(config_.D7_Pin->pin, config_.D7_Pin->mode, defaultSdioGpioSpeed);
    }
}

void SDIO::reset(void)
{
    // Set to reset values (0x00000000)
    write_register(SDIO_Regs::PWRCTL, 0);
    write_register(SDIO_Regs::CMDAGMT, 0);
    write_register(SDIO_Regs::CMDCTL, 0);
    write_register(SDIO_Regs::DATATO, 0);
    write_register(SDIO_Regs::DATALEN, 0);
    write_register(SDIO_Regs::DATACTL, 0);
    write_register(SDIO_Regs::INTC, 0);
    write_register(SDIO_Regs::INTEN, 0);
};

void SDIO::clock_configure(Clock_Edge edge, bool bypass, bool low_power, uint16_t divider)
{
    // Configure the parameters
    write_bits(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::CLKEDGE), static_cast<uint32_t>(edge),
               static_cast<uint32_t>(CLKCTL_Bits::CLKBYP), bypass ? Set : Clear,
               static_cast<uint32_t>(CLKCTL_Bits::CLKPWRSAV), low_power ? Set : Clear);
    // Check if we need to set DIV8 bit
    if (divider >= 256) {
        write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV8), 1);
        divider -= 256;
    }
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::DIV), static_cast<uint32_t>(divider));
}

void SDIO::set_hardware_clock_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::HWCLKEN), enable ? 1 : 0);
}

void SDIO::set_bus_width(Bus_Width width)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::BUSMODE), static_cast<uint32_t>(width));
}

uint32_t SDIO::get_power_mode()
{
    return read_bit(*this, SDIO_Regs::PWRCTL, static_cast<uint32_t>(PWRCTL_Bits::PWRCTL));
}

void SDIO::set_power_mode(Power_Control power)
{
    write_bit(*this, SDIO_Regs::PWRCTL, static_cast<uint32_t>(PWRCTL_Bits::PWRCTL), static_cast<uint32_t>(power));
}

void SDIO::set_clock_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CLKCTL, static_cast<uint32_t>(CLKCTL_Bits::CLKEN), enable ? 1 : 0);
}

void SDIO::set_command_config(Command_Index index, uint32_t argument, Command_Response response, Wait_Type type)
{
    write_register(SDIO_Regs::CMDAGMT, argument);
    write_bits(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::CMDIDX), static_cast<uint32_t>(index),
               static_cast<uint32_t>(CMDCTL_Bits::CMDRESP), static_cast<uint32_t>(response),
               static_cast<uint32_t>(CMDCTL_Bits::WAITTYPE), static_cast<uint32_t>(type));
}

void SDIO::send_command(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::CSMEN), enable ? 1 : 0);
}

uint8_t SDIO::get_command_index()
{
    return read_bit8(*this, SDIO_Regs::RSPCMDIDX, static_cast<uint32_t>(RSPCMDIDX_Bits::RSPCMDIDX));
}

uint32_t SDIO::get_response(Response_Type type)
{
    return read_register<uint32_t>(
               (type == Response_Type::RESPONSE0) ? SDIO_Regs::RESP0 :
               (type == Response_Type::RESPONSE1) ? SDIO_Regs::RESP1 :
               (type == Response_Type::RESPONSE2) ? SDIO_Regs::RESP2 : SDIO_Regs::RESP3
           );
}

void SDIO::data_configure(uint32_t timeout, uint32_t length, Block_Size size)
{
    write_register(SDIO_Regs::DATATO, timeout);
    write_bit(*this, SDIO_Regs::DATALEN, static_cast<uint32_t>(DATALEN_Bits::DATALEN), length);
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::BLKSZ), static_cast<uint32_t>(size));
}

void SDIO::data_transfer_configure(Transfer_Mode mode, Transfer_Direction direction)
{
    write_bits(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::TRANSMOD), static_cast<uint32_t>(mode),
               static_cast<uint32_t>(DATACTL_Bits::DATADIR), static_cast<uint32_t>(direction));
}

void SDIO::set_data_state_machine_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::DATAEN), enable ? 1 : 0);
}

uint32_t SDIO::read_data_word()
{
    return read_register<uint32_t>(SDIO_Regs::FIFO);
}

void SDIO::write_data_word(uint32_t data)
{
    write_register(SDIO_Regs::FIFO, data);
}

uint32_t SDIO::get_data_count()
{
    return read_bit(*this, SDIO_Regs::DATACNT, static_cast<uint32_t>(DATACNT_Bits::DATACNT));
}

uint32_t SDIO::get_fifo_count()
{
    return read_bit(*this, SDIO_Regs::FIFOCNT, static_cast<uint32_t>(FIFOCNT_Bits::FIFOCNT));
}

void SDIO::set_dma_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::DMAEN), enable ? 1 : 0);
}

void SDIO::set_read_wait_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWEN), enable ? 1 : 0);
}

void SDIO::set_stop_read_wait_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWSTOP), enable ? 1 : 0);
}

void SDIO::set_read_wait_type(Read_Wait type)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::RWTYPE), static_cast<uint32_t>(type));
}

// IO mode
void SDIO::set_IO_operation_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::DATACTL, static_cast<uint32_t>(DATACTL_Bits::IOEN), enable ? 1 : 0);
}

void SDIO::set_IO_suspend_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::SUSPEND), enable ? 1 : 0);
}

// CE-ATA mode
void SDIO::set_CE_ATA_command_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::ATAEN), enable ? 1 : 0);
}

void SDIO::set_CE_ATA_interrupt_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::NINTEN), enable ? 0 : 1);
}

void SDIO::set_CE_ATA_command_completion_enable(bool enable)
{
    write_bit(*this, SDIO_Regs::CMDCTL, static_cast<uint32_t>(CMDCTL_Bits::ENCMDC), enable ? 1 : 0);
}

bool SDIO::get_flag(Status_Flags flag) const
{
    return (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(flag)) != 0);
}

void SDIO::clear_flag(Clear_Flags flag)
{
    write_bit(*this, SDIO_Regs::INTC, static_cast<uint32_t>(flag), 1);
}

void SDIO::clear_all_flags()
{
    write_register(SDIO_Regs::INTC, static_cast<uint32_t>(All_Flags_Mask));
};

bool SDIO::get_interrupt_flag(Interrupt_Flags flag)
{
    return (read_bit(*this, SDIO_Regs::STAT, static_cast<uint32_t>(flag)) != 0);
}

void SDIO::clear_interrupt_flag(Clear_Flags flag)
{
    write_bit(*this, SDIO_Regs::INTC, static_cast<uint32_t>(flag), 1);
}

void SDIO::set_interrupt_enable(Interrupt_Type type, bool enable)
{
    write_bit(*this, SDIO_Regs::INTEN, static_cast<uint32_t>(type), enable ? 1 : 0);
}

} // namespace sdio

// Instantiate class for global device
sdio::SDIO SDIO_DEVICE;
