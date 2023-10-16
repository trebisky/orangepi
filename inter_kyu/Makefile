# makefile
CROSS = arm-linux-gnu-

CC = $(CROSS)gcc

GCCVERSION = `$(CC) -dumpversion`

ABI = -isystem /usr/lib/gcc/arm-linux-gnueabi/$(GCCVERSION)/include
LIBS = -L /usr/lib/gcc/arm-linux-gnueabi/$(GCCVERSION) -lgcc

#ARM_ELF_FLAGS = -Os -marm -fpic -Wall
#ARM_ELF_FLAGS = -marm -fpic -Wall
#ARM_ELF_FLAGS = -O2 -marm -fpic -Wall
#ARM_ELF_FLAGS = -O2 -marm -Wall
ARM_ELF_FLAGS = -O2 -marm

ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security
ARM_ELF_FLAGS += -Wno-implicit-function-declaration
ARM_ELF_FLAGS += -Wno-unused-function $(ABI)


OBJS = start.o interrupts.o prf.o serial.o gpio.o timer.o bbb_irq.o show_regs.o gic.o

TARGET = interrupts.bin

%.bin: %.elf
	$(CROSS)objcopy -O binary $< $@

%.o: %.c
	$(CC) -c $(ARM_ELF_FLAGS) -nostdlib -c $<

.S.o:
	        $(CC) -o $@ -c $<

.c.S:
	        $(CC) -S $(ARM_ELF_FLAGS) $<

all: install

install: $(TARGET)
	cp $(TARGET) /var/lib/tftpboot/orange.bin

DUMP = arm-linux-gnu-objdump -d

dump:       interrupts.elf
	$(DUMP) interrupts.elf >interrupts.dump

clean:
	rm -rf *.bin *.elf *.o *.dump
	rm -rf interrupts.S

obj:
	$(CROSS)objdump -h interrupts.o

version:
	$(CC) --version

#LINKER = -Wl,-N,--build-id=none
#LINKER = -Wl,--build-id=none
LINKER = -Wl,--build-id=none -Wl,--no-warn-rwx-segments

interrupts.elf: $(OBJS)
	$(CC) -o $@ -nostdlib $(OBJS) -T basic.lds $(LINKER) $(LIBS)

# THE END
