
h5_no_atf

This little project accomplishes two purposes.  One is that it examines in detail the h5 boot process.
The other is that it eliminates the ATF part of that process (the "ARM trusted firmware" component).
The ATF component (otherwise known as bl31.bin) switches from EL3 to EL2, and it also places the
processor in NS (non-secure) mode, which is a sort of "sanboxed" mode.  For me, this is just a
nuisance since I am developing my own RTOS (the Kyu Project), not running linux.
If you want to run linux, you can use the standard setup which includes ATF.

My aim here is to start with various binary files and reproduce the packaging and place the
result on a bootable SD card which can be placed into an Orange Pi PC2 board (with H5 chip)
and end up running U-Boot in secure mode at EL2.

To build bl31.bin (not part of this project) --

1. cd arm-trusted-firmware
1. make realclean
1. make PLAT=sun50iw1p1 DEBUG=1 bl31
1. or perhaps: make V=1 PLAT=sun50iw1p1 DEBUG=1 bl31
1. yields: build/sun50iw1p1/debug/bl31/bl31.elf

Using "make sdcard" will yield an SD card that will run U-boot with the U-boot environment
set up to do DHCP and TFTP booting of an image called "h5.bin"
