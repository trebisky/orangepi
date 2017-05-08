# Makefile for core test 4 on the Orange Pi H3
#
# Tom Trebisky  4-15-2015

INCS = -I. -Iarm

include Makefile.inc

all: core

# ------------------------ core 3 stuff

COBJS =  locore.o \
    smp.o \
    traps.o \
    orange_pi/gpio.o \
    orange_pi/serial.o

core: $(COBJS)
	$(LD) -g -T kyu.lds -e asm_startup -o core.elf $(COBJS) $(LIBS)
	$(BIN) core.elf core.bin
	$(DUMP) core.elf >core.dump
	cp core.bin /var/lib/tftpboot/orange.bin

clean:
	rm -f $(COBJS)
	rm -f core.dump
	rm -f core.elf
	rm -f core.bin

ver:
	@echo $(GCCVERSION)
	@echo $(ABI)
	$(CC) --version

term:
	picocom -b 115200 /dev/ttyUSB0

# THE END
