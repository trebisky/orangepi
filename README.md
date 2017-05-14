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
2. float - bare metal floating point
3. cores1 - get a second CPU core to fire up (not yet working)
3. cores2 - another attempt at a second core (not yet working)
3. cores3 - a simple example that does work, pruned back from Kyu sources
3. cores4 - an extension of cores3 with additional experimenting
4. print - partition files and add a printf
5. timer - get a timer running (but not yet interrupting)
6. inter_ez - set up the GIC so we get timer interrupts (has problems)
7. inter_kyu - interrupts with timer and GIC with Kyu additions (works)

After this, my efforts have transitioned to getting Kyu to run on
the Orange Pi, so you should go there next.  I will probably never
get the "inter_ez" demo to work (but you never know), and expect to
try to push forwad the second core business with the aid of the Kyu
infrastructure.  So this project is sort of finished (maybe).

The bootrom though may get further attention.
