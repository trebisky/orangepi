# Makefile for bare metal projects on the Orange Pi H3
#
# Tom Trebisky  10-17-2023

INCS = -I.

include Makefile.inc

all: cores

COBJS =  start.o \
    core_start.o \
    main.o \
    core.o \
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

cores: $(COBJS)
	$(LD) -g -T kyu.lds -e asm_startup -o cores.elf $(COBJS) $(LIBS)
	$(BIN) cores.elf cores.bin
	$(DUMP) cores.elf >cores.dump
	cp cores.bin /var/lib/tftpboot/orange.bin

clean:
	rm -f $(COBJS)
	rm -f cores.dump
	rm -f cores.elf
	rm -f cores.bin

ver:
	@echo $(GCCVERSION)
	@echo $(ABI)
	$(CC) --version

term:
	picocom -b 115200 /dev/ttyUSB0

# THE END
