core

Start a second core.

I began this by copying the cpu demo.  It has a somewhat fancy start.S that does
initialize the BSS.  Then I added stuff from blink3, as follows:

- proper irq code from start.S
- gic.c
- serial.c
- timer.c

And I did a bunch of reorganization and cleanup, introducing led.c, ccm.c and so forth.
Then I ensured it would do a simple LED blink driven by interrupts and that the
whole mess would compile before I started work on starting a core.

