# Makefile to build FIT image for PC2 (H5) U-Boot
# Tom Trebisky  12-3-2018

# Ultimately you can use this to prepare an SD card to run U-Boot
# with U-Boot set up to network boot h5.bin
# Find a blank SD card (or one you don't care about) and do:
# su ; make sdcard
# You need to run as root to have permission to write to the SD card,
# at least on my system.

SDCARD = /dev/sdf

CROSS = aarch64-linux-gnu
XCC = $(CROSS)-gcc
XOBJ = $(CROSS)-objcopy -O binary

LDFLAGS = -Bstatic                                                        \
          -Tswitch_el.lds                                               \
          -Wl,--start-group                                               \
          -Wl,--end-group -Wl,--build-id=none                                             \
          -nostdlib

# =======================

# pick one of these.
# The ATF choice is the original and runs U-Boot
#  in EL2 and non-secure.
# The TJT choice is my atf bypass and runs U-Boot
#  in EL2 and stays secure
UBOOT = u-boot-tjt.img
#UBOOT = u-boot-atf.img

all:	switch

tools: mksunxiboot
spl: u-boot-spl.img
atf: bl31.bin
switch: switch_el.bin
u-boot: u-boot-atf.img u-boot-tjt.img

sdcard:	u-boot-spl.img
	./mksdcard $(SDCARD)
	dd if=u-boot-spl.img of=$(SDCARD) bs=8k seek=1
	dd if=$(UBOOT) of=$(SDCARD) bs=8k seek=5
	sync

# ===== SPL

u-boot-spl.bin: u-boot-spl.elf
	aarch64-linux-gnu-objcopy  $(MANYJ) -O binary  u-boot-spl.elf u-boot-spl.bin

mksunxiboot: mksunxiboot.c spl.h
	cc -o mksunxiboot mksunxiboot.c

##mkimage: mkimage.c mkimage.h
##	cc -o mkimage mkimage.c

# Note that the name of the DTB goes in here, but apparently just gets saved
# as a string, the DTB itself doesn't get involved with the SPL.
# In fact, I am betting that even the string doesn't get involved at all, the DTB is
# wrapped up as part of the FIT image that the SPL loads.
u-boot-spl.img: u-boot-spl.bin mksunxiboot
	./mksunxiboot --default-dt "sun50i-h5-orangepi-pc2" u-boot-spl.bin u-boot-spl.img

# ===== BL31

bl31.bin: bl31.elf
	$(XOBJ) bl31.elf bl31.bin

# ===== SWITCH

switch_el.bin: switch_el.elf
	$(XOBJ) switch_el.elf switch_el.bin

switch_el.elf: switch_el.o
	$(XCC) switch_el.o $(LDFLAGS) -o switch_el.elf

switch_el.o: switch_el.S
	$(XCC) $< -c -o $@

# ===== U-BOOT

# Is all this really needed?  Who knows, but this is what the U-Boot build does
MANYJ = -j .text -j .secure_text -j .secure_data -j .rodata -j .data -j .u_boot_list \
	    -j .rela.dyn -j .got -j .got.plt -j .binman_sym_table -j .text_rest \
	    -j .dtb.init.rodata -j .efi_runtime -j .efi_runtime_rel

u-boot.bin: u-boot.elf
	aarch64-linux-gnu-objcopy  $(MANYJ) -O binary  u-boot.elf u-boot.bin

# This was a one-time use thing, then I hand edited the resulting file
#u-boot.its: bl31.bin mksunxi_fit_atf.sh
#	./mksunxi_fit_atf.sh sun50i-h5-orangepi-pc2.dtb > u-boot.its

# This does spew out a bunch of output, but I guess I don't mind.
# mkimage  -f u-boot.its -E u-boot.itb >/dev/null  && cat /dev/null
u-boot-atf.img: u-boot-atf.its u-boot.bin
	./mkimage  -f u-boot-atf.its -E u-boot-atf.img

u-boot-tjt.img: u-boot-tjt.its u-boot.bin
	./mkimage  -f u-boot-tjt.its -E u-boot-tjt.img

# SPL image is always 32K
# rather than fusing the two together like this,
#  I put them seperately on the SD card.
# u-boot-with-spl.img:
#	cat u-boot-spl.img u-boot.img > u-boot-with-spl.bin

# ===== MISC

clean:
	rm -f *.bin
	rm -f *.img
	rm -f *.o
	rm switch_el.elf

# THE END
