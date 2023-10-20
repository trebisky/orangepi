This began by copying inter_ez on 10-19-2023

The idea is to blink LEDs using interrupts rather
than CPU burning delay loops.
And we will set up the timer to do the interrupts
and perhaps play a bit with the timer as well.

I added the ISR (status) info to the GIC structure
and I do see pending interrupts there, both for
the timer (for 51 when using timer 1) and for
the uart.  This could be useful for identifying
the IRQ of unknown interrupts sources
(perhaps the GTIMER?).
