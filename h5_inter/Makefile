# Makefile for Allwinner H5 blink
# Adaped from Metro94 Github s5p6818_spl project.
# Tom Trebisky  11-8-2018
# tom@mmto.org

BOARD = h5
CROSS_COMPILE = aarch64-linux-gnu-

# -------------------------------------

OBJS = start.o inter.o gpio.o uart.o

TARGET = $(BOARD).bin

# CFLAGS		:=	-g -Wall -Wextra -ffreestanding -fno-builtin -mlittle-endian
CFLAGS		:=	-g -Wall -ffreestanding -fno-builtin -mlittle-endian
CFLAGS		+=	-Wno-unused-variable
CFLAGS		+=	-Wno-pedantic
CFLAGS		+=	-Wno-unused-but-set-variable
CFLAGS		+= 	-march=armv8-a+crc
CFLAGS		+= 	-mtune=cortex-a53
CFLAGS		+= -I.

LDFLAGS		:=	-Bstatic \
			-Tspl.lds \
			-Wl,--start-group \
			-Wl,--end-group \
			-Wl,--build-id=none \
			-nostdlib

CC			=	$(CROSS_COMPILE)gcc $(CFLAGS)
LD 			=	$(CROSS_COMPILE)gcc $(LDFLAGS)
OBJCOPY			=	$(CROSS_COMPILE)objcopy
DUMP			=	$(CROSS_COMPILE)objdump

LOAD			=	tools/loader -h64

# This gives us dependencies in .d files.
# CFLAGS		+= -MMD
# This gives us a map file.
# CFLAGS		+= -Wl,-Map=$(BOARD).map,--cref \

.c.o:
	@echo " [CC]   $<"
	@$(CC) $< -c -o $@

.S.o:
	@echo " [CC]   $<"
	@$(CC) $< -c -o $@

# -------------------------------------

all: install
#all: $(TARGET)

install: $(TARGET)
	cp $(TARGET) /var/lib/tftpboot


$(BOARD).elf: $(OBJS)
	@echo " [LD]   $(BOARD).elf"
	@$(LD) $(OBJS) -o $(BOARD).elf

$(TARGET): $(BOARD).elf
	@#echo " [IMG]  $(TARGET)
	@$(OBJCOPY) -O binary $(BOARD).elf $(TARGET)

dis: $(BOARD).elf
	$(DUMP) -d $(BOARD).elf -z >$(BOARD).dis

fetch:
	cp ../USB_loader/loader tools

# -------------------
# You will need to be root for either of these to work.

usb:  $(TARGET)
	$(LOAD) $(TARGET)

sdcard:  $(TARGET)
	$(LOAD) -o $(TARGET) | dd of=/dev/sdf seek=1 conv=fdatasync

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.img
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
	rm -f *.dis
#	rm -f *.d

# THE END
