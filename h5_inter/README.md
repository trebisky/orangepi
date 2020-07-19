This is a research and experimentation project.

I am trying to solve two problems which may or may not be related.

The first is that I have been unable to get timer interrupts from
the H5 board.  It is unclear whether the timer is not working or if
interrupts in general do not work.

The second is that I am unable to manipulate (i.e. blink) the POWER
led.  This is on the oddball R_GPIO port on bit 10.  I can blink it
fine on my H3 boards, so why not here?  My guess is that I need to
do something about clocks on the bus that holds all the "R_"
peripherals, but nothing is really clear at this point.
