# MFL Microcontoller Firmware Library

This is a C++ microcontroller firmware library for the GD32F303RE ARM Cortex-M4 microcontroller.
Written from scratch by yours truly using the vendor manuals and datasheets.

Certain peripherals are still a work in progress, and some may require more testing.
That being said, it is currently stable for use in Marlin Firmware and the peripherals used there.

The library does not yet support CAN bus nor the I2S portion of the SPI peripheral. They may be added in the future.

There is only one example right now, however, since this library was developed in tandum with a custom Arduino Core,
you can refer to the Arduino Core for reference.
https://github.com/bmourit/ArduinoCore-MFL

As mentioned, the Marlin HAL for this library and the its coresponding Arduino Core has now been upstreamed into Marlin Firmware.
[https://github.com/bmourit/Marlin_MFL](https://github.com/MarlinFirmware/Marlin)

The MFL project is LGPLv3 licensed.

# Library Size

This is an MCU with limited SRAM (64KB) and flash (256/512KB) memory, so the MFL library has been written to produce small binary size be runtime memory effecient.
Thanks to C++23 GCC compiler and some clever techniques, we can accomplish this without sacrificing any performance.
For example, the GPIO peripheral can compile the code abstraction to allow direct IO with zero overhead.

Optimization work will always remain a high priority.

# Compiling the examples

To compile the examples:
Check CONFIG.hpp for your specific needs and then build the stardard way using eg: make usart_dmarx_irq_example

You will need an Arm embeded toolchain.
Tested with v13.2.1 and v14.2.1 from
xPack GNU Arm Enbedded GCC which can be found here:
https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/
