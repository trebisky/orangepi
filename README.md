This is a collection of programs I have been writing to run on
the Orange Pi PC with the Allwinner H3 chip.
The H3 has a 4 core Cortex-A7 inside (32 bit armv7),

I used to have projects for the Allwinner H5 here also
(the Orange Pi PC 2), but I moved them to their own repository
"orangepi_h5

They are what you might call "bare-metal"
programming, although they get loaded by U-Boot.

I also provide a disassembly of the H3 bootrom that I am working
on annotating and studying as I find time.
I have not invested much time in this (or needed to).

For lots of notes and supporting information, see my website:

http://cholla.mmto.org/orange_pi/

============

I did most of this work back in 2017, but I decided to revisit it
again in 2023.  I had to do some updating to get things to build
without warnings with new gnu cross compile tools.
Also my tftp boot setup on the Orange Pi PC was loading to
0x4000_0000 and some of the old demos linked for 0x4200_000.
I needed to change the demos to link for the same address they
load to.

If you want to follow my work in order, take them like this.
For the H3 (Orange Pi PC):

1. hello - just send output to the serial port, no assembly startup.
1. blink - blink both on board LED's and write to serial port
2. float - bare metal floating point
2. print - partition files and add a printf

The demos below here have not been verified since 2017 so beware.

3. cores1 - get a second CPU core to fire up (not yet working)
3. cores2 - another attempt at a second core (not yet working)
3. cores3 - a simple example that does work, pruned back from Kyu sources
3. cores4 - an extension of cores3 with additional experimenting
5. timer - get a timer running (but not yet interrupting)
6. inter_ez - set up the GIC so we get timer interrupts (has problems)
7. inter_kyu - interrupts with timer and GIC with Kyu additions (works)

In 2017, I stopped working on these demos.
My efforts transitioned to getting Kyu to run on
the Orange Pi, with general success.
Both interrupts and multiple core startup work nicely with Kyu.

I will probably never get the "inter_ez" demo to work (but you never know).

Some things here are unfinished (and not working) and are unlikely to
get further attention in this context.
