#CROSS_COMPILE ?= arm-none-eabi-
CROSS_COMPILE ?= arm-linux-gnu-

CROSS_CC ?= $(CROSS_COMPILE)gcc

#ARM_ELF_FLAGS = -Os -marm -fpic -Wall
#ARM_ELF_FLAGS = -marm -fpic -Wall
ARM_ELF_FLAGS = -O2 -marm -fpic -Wall

ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security

TARGET = hello.bin

%.bin: %.elf
	$(CROSS_COMPILE)objcopy -O binary $< $@

all: install

install: $(TARGET)
	cp $(TARGET) /var/lib/tftpboot/orange

DUMP = arm-linux-gnu-objdump -d

dump:       hello.elf
	$(DUMP) hello.elf >hello.dump

clean:
	rm -rf *.bin *.elf *.o *.dump

version:
	$(CROSS_CC) --version

hello.elf: hello.c
	$(CROSS_CC) -g $(ARM_ELF_FLAGS) $< -nostdlib -o $@ -T simple.lds -Wl,-N
