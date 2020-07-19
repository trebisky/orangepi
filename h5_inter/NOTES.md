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

I am not sure what "axp" is about, but there are include files:

asm/arch/pmic_bus.h
asm/gpio.h
axp_pmic.h

Full paths to these are:

include/axp_pmic.h
arch/arm/include/asm/arch-sunxi/pmic_bus.h

The above path is used because of a link in the directory:

U-Boot/u-boot-2018.09/arch/arm/include/asm

Here the link "arch" points to "arch-sunxi"

So the directory "arch/arm/include/asm/arch-sunxi"
holds a wealth of interesting material.

This is what is loosely refered to as U-Boot hell.
Files are scattered in a multitude of directories, and you need
to have multiple windows open and be ready to hunt around for
files that would be better kept all in one place.

This bad idea was picked up from the Linux sources perhaps.
But I have noticed this silly brain damage elsewhere.
Some people think that all the header files need to be gathered
in a directory of their own and you should never have .h and .c
files in the same directory.  This is generally refered to as
foolishness and/or stupidity.  But I hold back from saying what
I really think about all this and I hope you appreciate it.

