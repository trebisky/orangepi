This program prints a friendly greeting on the serial console,
then goes into a loop trying to blink both on board LED's.

Both LED would blink with this code on the H3 chip, but so
far the PWR LED (green on port L10) stays off.

This gets loaded by U-Boot and is the first bit of bare-metal
C code I have run on the Orange Pi PC2 (Allwinner H5).
