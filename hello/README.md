I set up my Orange Pi PC to use U-Boot to transfer a binary file via tftp and jump
to the start of it.  After doing this, I was able to begin writing some simple programs.
The very first thing I ran was

sunxi/uart0-helloworld-sdboot.c

I was able to build and run this immediately.
This program along with the H3 chip datasheet led to this program.
