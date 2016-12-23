This is the bootrom image from my Orange Pi PC board.

What you are probably most interested in here is the file
bootrom.txt, which is my annotated disassembly of the BROM.

What I did to get started on this was to run Armbian linux on
my OrangePi PC board.  Then I wrote the program dumprom.c to
fetch the bootrom image and generate bootrom.bin.
I then copied this to my linux desktop machine.

On that machine I worked over the wrap.c program, which wraps
up the binary image into an ELF format file.  This makes it
easy to use objdump to do the disassembly and obtain
proper addresses in the disassembly.

The ROM is 32K bytes at address 0xffff0000.

I read out 64K and discovered that the second 32K
was an exact repeat of the first 32K.
