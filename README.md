# Monster Joysticks Pi Zero Bare Metal USB

This software is still in developement, it is using cannibalized code from the following sources and tutorials.

http://www.valvers.com/open-software/raspberry-pi/step01-bare-metal-programming-in-cpt1/

https://github.com/petrockblog/SNESDev-RPi

You'll need the ARM Embedded toochain https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads to compile the kernel.img file which can then be placed in a FAT formatted Micro SD card along with the bootcode.bin and start.elf from https://github.com/raspberrypi/firmware/tree/master/boot

# Current Status
Inputs from the GPIO adapter are read and then output to the screen representing the colour of the putton pressed, joystick movements will adjust the position of the crosshair on the screen.
