# MFL Microcontoller Firmware Library

This is a C++ microcontroller firmware library for the GD32F303RE chip.
Written from scratch by yours truly using the vendor manuals and datasheets.

I DO NOT RECOMMEND USE!
It is currently a work in progress and NOT sufficiently tested.
I plan to eventually release the project under GPLv3 license, or some variant.

There are still some peripherals and functionality missing, one big one being timers.
No I2S support in the SPI library, missing I2C support, no SDIO support, etc.
That said, the missing support is mainly wrapping up, so expect full functionality soon.

Eventually I indend to use this along with a custom Arduino Core for this chip.
Documentation is non-existant as of now. This is still early stages, but it does build.

# Compiling the examples

To compile the examples, simply move their main.cpp file to the base project folder,
check CONFIG.hpp for your specific needs, then build the stardard way using make.

You will need an Arm embeded toolchain. Only tested with v13.2.1-1.1 of
the xPack GNU Arm Enbedded GCC which can be found here:
https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/
