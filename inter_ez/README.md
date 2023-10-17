The goal here is to write code that will make
interrupts (in particular timer interrupts) work on
the Allwinner H3 using the ARM GIC.

I had weird issues with this, for a while.
I would get 269 interrupts, then it would somehow
jump to the "blink" code and blink the red LED
endlessly.

I made some changes (10-2023) to investigate how the
initial interrupt stack and main stack was being
set up and it has worked fine ever since.

All this makes me a bit nervous.
