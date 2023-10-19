cpu

This is the first demo I worked up in 2023.

I was unsure what demo to copy to begin this one.  I first copied "float",
but then added stuff from inter_ez (in particular prf.c and protos.h)

The game here is to get the details about what the state of the CPU is
when it is passed to use by U-Boot, and to do this by probing and
experimentation.  Some basics about the processors in the H3 chip:

4 cores -- Cortex A7
  more specifically Cortex-A7 MPCore
  these implement the ARM v7-A architecture
  the Cortex A7 has an 8 stage pipeline.

512K of L2 cache shared by all 4 cores
Each core has 32K data and 32K instruction L1 cache

The board I am experimenting with is an Orange Pi PC plus with 1G of ram
(ram is from 0x4000_0000 to 0x7fff_ffff)

Sunxi documents say that running above 1.296 Ghz is overclocking
and likely to result in overheating.

Here is what I have found:

    Only core 0 started
    Stack at 0x79F68498 (middle of the last 256M block)
    CPU running at 1008 Mhz
    CPSR = 600001D3
