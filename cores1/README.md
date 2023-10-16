This is a quick proof of concept demo to see if I understand how to start up one of
the other cores in the Quad core Allwinner H3 chip.

This does not work.  In retrospect, a lot more needs to be done.
In particular, in order to access hardware (like the GPIO to blink an LED)
we need to initialize the MMU (to ensure that the hardware address space
does not get cached).

STATUS: This does not work yet.
