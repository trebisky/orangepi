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

1. http://cholla.mmto.org/orange_pi/
2. http://cholla.mmto.org/orange_pi/pc_h3/

***

I did most of this work back in 2017, but I decided to revisit it
again in 2023.  I had to do some updating to get things to build
without warnings with new gnu cross compile tools.
Also my tftp boot setup on the Orange Pi PC was loading to
0x4000_0000 and some of the old demos linked for 0x4200_0000.
I needed to change many of the demos to link to the 0x4000_0000 address.

This first group of demos have no assembly language startup file.
Just C code from the very start.
The first two link to address zero and rely on the compiler
producing position independent code entirely.

0. bootrom - on chip bootrom disassembly
1. hello - just send output to the serial port, no assembly startup.
1. blink - blink both on board LED's and write to serial port
2. print - partition files and add a printf
5. timer - get a timer running, add printf (not yet interrupting)

The next two add an assembly startup and exception handling.
The "inter_kyu" was added after I had done a lot of work in Kyu
and wanted to revisit getting interrupts to work.
The "float" was just a curiosity exercise since I never try to
do floating point in any of my bare metal projects.

2. float - bare metal floating point with exception handling from Kyu
3. inter_ez - set up the GIC so we get timer interrupts (works)
1. inter_kyu - interrupts with timer and GIC with Kyu additions (works)

And here are some new things for 2023

1. cpu - check what speed the CPU runs at from U-Boot .. and more!
2. blink2 - blink LEDs using timer and interrupts (works)
3. blink3 - fancier way to blink LEDs using timer and interrupts

It is important to point out that after doing a number of these little
projects back in 2017, I shifted my attention to getting Kyu to run on
the Orange Pi, with general success.
Both interrupts and multiple core startup work nicely with Kyu,
and the bulk of my Orange Pi work will be found in the Kyu project code.
Now in 2023 I am returning to these with much greater knowledge.

***

The demos below don't work and for one reason or other I have no intentions
of doing any more work on them.  Revisited in 2023, but not worth investing more time in.

3. xcores1 - get a second CPU core to fire up (not working)
3. xcores2 - another attempt at a second core (not working)
3. xcores3 - a simple example, pruned back from Kyu sources (rarely works)
3. xcores4 - an extension of cores3 with additional experimenting (not working)

