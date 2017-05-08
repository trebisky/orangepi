Cores3

This is a simple demo to start up the other cores on an Orange Pi,
namely the Allwinner H3 chip.

It began as the full blown Kyu operating system (which was working
reliably, at least so we thought, starting other cores).
It was whittled down to just a handful of files with a lot of
surprises along the way.  

This code usually works, but sometimes the board will get
"in a mood" not to start cores, and then can be "fixed" by
running Kyu and using, "i" and "t8" to run the core test.
No clue why this odd rain dance works or is needed.

There is a big .skip in locore.S that needs to be there
for things to work.  Who knows why.
We are continuing with other experiments and leaving this
in a known somewhat useful state.

