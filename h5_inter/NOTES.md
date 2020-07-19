These are some notes as I research how to get the R_GPIO and
the interrupts to work on the OrangePi H5 board.

My first place to look is the U-Boot sources.
And the hot tip there is to look for the string "sunxi"
in filenames.  This leads to:

board/sunxi

Pathnames on my machine are unique, but do offer helpful
information:

OrangePi/pc2/U-Boot/u-boot-2018.09/drivers/gpio/axp_gpio.c

The above file seems to be used by the sunxi startup code.
At least the routine axp_gpio_init() is called by the sunxi
code.
