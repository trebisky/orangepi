CROSS = arm-linux-gnu-

CC = $(CROSS)gcc

#ARM_ELF_FLAGS = -Os -marm -fpic -Wall
#ARM_ELF_FLAGS = -marm -fpic -Wall
#ARM_ELF_FLAGS = -O2 -marm -fpic -Wall
ARM_ELF_FLAGS = -O2 -marm -Wall

ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security

TARGET = print.bin

%.bin: %.elf
	$(CROSS)objcopy -O binary $< $@

all: install

install: $(TARGET)
	cp $(TARGET) /var/lib/tftpboot/orange

DUMP = arm-linux-gnu-objdump -d

dump:       print.elf
	$(DUMP) print.elf >print.dump

clean:
	rm -rf *.bin *.elf *.o *.dump

obj:
	$(CROSS)objdump -h print.o

version:
	$(CC) --version

print.o: print.c
	$(CC) -c $(ARM_ELF_FLAGS) -nostdlib -c $<

#LINKER = -Wl,-N,--build-id=none
LINKER = -Wl,--build-id=none

print.elf: print.o
	$(CC) -o $@ -nostdlib $< -T basic.lds $(LINKER)
