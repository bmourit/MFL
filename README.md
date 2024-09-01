# MFL Microcontoller Firmware Library

This is a C++ microcontroller firmware library for the GD32F303RE ARM Cortex-M4 microcontroller.
Written from scratch by yours truly using the vendor manuals and datasheets.

It is currently a work in progress and NOT tested. If you use this, you are doing sp at your own risk!
I plan to eventually release the project under GPLv3 license, or some variant.

There are still some functionality missing, for example no I2S support yet in the SPI peripheral library. Most major portions, however, are complete.

Eventually I indend to use this along with a custom Arduino Core for this chip.
Documentation is non-existant as of now. This is still early stages, but it does build.

# Library Size
This being intended for an MCU with limited memory and flash size, it is important to keep this base MFL library as small as possible. It should also be memory effecient as well, though I don't have exact numbers yet. This will be tackeled more completely during the optimization phase.

# Compiling the examples

To compile the examples, simply move their main.cpp file to the base project folder,
check CONFIG.hpp for your specific needs, then build the stardard way using make.

You will need an Arm embeded toolchain. Only tested with v13.2.1-1.1 of
the xPack GNU Arm Enbedded GCC which can be found here:
https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/
