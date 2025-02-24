# MFL Microcontoller Firmware Library

This is a C++ microcontroller firmware library for the GD32F303RE ARM Cortex-M4 microcontroller.
Written from scratch by yours truly using the vendor manuals and datasheets.

It is a work in progress and some portions need more testing.
The MFL project is LGPLv3 licensed.

There are only a couple things missing: CAN bus support and I2S support for SPI peripheral library.

There is only one example right now, however, since this library was developed in tandum with a custom Arduino Core,
you can refer to the Arduino Core for reference.
https://github.com/bmourit/ArduinoCore-MFL

Also note, this library and the Arduino Core is being used, and is fully functional,
in a fork of Marlin Firmware for Creality boards carrying this chip.
The hope is to eventually get this upstreamed.
https://github.com/bmourit/Marlin_MFL


# Library Size

This being intended for an MCU with limited SRAM and flash memory, it is important to keep the MFL library as small as possible and memory effecient. This library and Arduino Core are currently being used to build Marlin Firmware
and the resulting binary size is slightly smaller than when built using the "compatible" STM32 HAL libraries.

That being said, optimization work is still ongoing.

# Compiling the examples

To compile the examples:
Check CONFIG.hpp for your specific needs and then build the stardard way using eg: make usart_dmarx_irq_example

You will need an Arm embeded toolchain.
Tested with v13.2.1 and v14.2.1 from
xPack GNU Arm Enbedded GCC which can be found here:
https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/
