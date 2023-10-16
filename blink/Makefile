#CROSS_COMPILE ?= arm-none-eabi-
CROSS_COMPILE ?= arm-linux-gnu-

CROSS_CC ?= $(CROSS_COMPILE)gcc

#ARM_ELF_FLAGS = -Os -marm -fpic -Wall
#ARM_ELF_FLAGS = -marm -fpic -Wall
ARM_ELF_FLAGS = -O2 -marm -fpic -Wall

ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security

TARGET = blink.bin

%.bin: %.elf
	$(CROSS_COMPILE)objcopy -O binary $< $@

all: install

install: $(TARGET)
	cp $(TARGET) /var/lib/tftpboot/orange.bin

DUMP = arm-linux-gnu-objdump -d

dump:       blink.elf
	$(DUMP) blink.elf >blink.dump

clean:
	rm -rf *.bin *.elf *.o *.dump

version:
	$(CROSS_CC) --version

# Without this, we get this senseless warning:
# /usr/bin/arm-linux-gnu-ld: warning: .note.gnu.build-id section discarded, --build-id ignored
LFLAGS = -Wl,--build-id=none -Wl,--no-warn-rwx-segments

blink.elf: blink.c
	$(CROSS_CC) -g $(ARM_ELF_FLAGS) $(LFLAGS) $< -nostdlib -o $@ -T simple.lds -Wl,-N
