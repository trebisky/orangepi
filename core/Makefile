# Makefile for bare metal projects on the Orange Pi H3
#
# Tom Trebisky  10-17-2023

INCS = -I.

include Makefile.inc

all: cpu

COBJS =  start.o \
    main.o \
    ccm.o \
    gic.o \
    prf.o \
    traps.o \
    ccnt.o \
    pll.o \
    gpio.o \
    led.o \
    timer.o \
    serial.o

cpu: $(COBJS)
	$(LD) -g -T kyu.lds -e asm_startup -o cpu.elf $(COBJS) $(LIBS)
	$(BIN) cpu.elf cpu.bin
	$(DUMP) cpu.elf >cpu.dump
	cp cpu.bin /var/lib/tftpboot/orange.bin

clean:
	rm -f $(COBJS)
	rm -f cpu.dump
	rm -f cpu.elf
	rm -f cpu.bin

ver:
	@echo $(GCCVERSION)
	@echo $(ABI)
	$(CC) --version

term:
	picocom -b 115200 /dev/ttyUSB0

# THE END
