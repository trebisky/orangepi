This is a collection of programs I have been writing to run on
the Orange Pi PC and PC2 boards.
These boards use the Allwinner H3 and H5 chips respectively.
The H3 has a 4 core Cortex-A7 inside (32 bit armv7),
while the H5 has a 4 core A53 (64 bit armv8) inside.
Amazingly the peripherals wrapped around each CPU core are
almost identical.

They are what you might call "bare-metal"
programming, although they get loaded by U-Boot.

I also provide a disassembly of the H3 bootrom that I am working
on annotating and studying as I find time.
I have not invested much time in this (or needed to).

For lots of notes and supporting information, see my website:

http://cholla.mmto.org/orange_pi/

If you want to follow my work in order, take them like this.
For the H3 (Orange Pi PC):

1. hello - first output to the serial port
2. blink - blink both on board LED's
2. float - bare metal floating point
3. cores1 - get a second CPU core to fire up (not yet working)
3. cores2 - another attempt at a second core (not yet working)
3. cores3 - a simple example that does work, pruned back from Kyu sources
3. cores4 - an extension of cores3 with additional experimenting
4. print - partition files and add a printf
5. timer - get a timer running (but not yet interrupting)
6. inter_ez - set up the GIC so we get timer interrupts (has problems)
7. inter_kyu - interrupts with timer and GIC with Kyu additions (works)

For the H5 (Orange Pi PC2):

1. h5_hello_asm - hello world written entirely in assembly.
2. h5_hello - hello world written in C
3. h5_no_atf - U-Boot without ATF and secure mode

After this, my efforts have transitioned to getting Kyu to run on
the Orange Pi, so you should go there next.  I will probably never
get the "inter_ez" demo to work (but you never know), and expect to
try to push forwad the second core business with the aid of the Kyu
infrastructure.  So this project is not expected to get much more
attention from me, and some things are unfinished.

The bootrom is the one thing that may get further attention.
