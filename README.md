This is a collection of programs I have been writing to run on
the Orange Pi PC board.  They are what you might call "bare-metal"
programming, although they get loaded by U-Boot.

I also provide a disassembly of the H3 bootrom that I am working
on annotating and studying as I find time.

For lots of notes and supporting information, see my website:

http://cholla.mmto.org/orange_pi/

If you want to follow my work in order, take them like this:

1. hello - first output to the serial port
2. blink - blink both on board LED's
3. cores - get a second CPU core to fire up (not yet working)
4. print - partition files and add a printf
5. timer - get a timer running (but not yet interrupting)
6. inter_ez - set up the GIC so we get timer interrupts (has problems)
7. inter_kyu - interrupts with timer and GIC with Kyu additions (works)
