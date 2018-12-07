# Makefile to build FIT image for PC2 (H5) U-Boot
# Tom Trebisky  12-3-2018

SDCARD = /dev/sdf

tools: mksunxiboot
spl: u-boot-spl.img
atf: bl31.bin
u-boot: u-boot.img

sdcard:	u-boot-spl.img
	./mksdcard $(SDCARD)
	dd if=u-boot-spl.img of=$(SDCARD) bs=8k seek=1
	dd if=u-boot.img of=$(SDCARD) bs=8k seek=5
	sync

# Is all this really needed?  Who knows, but this is what the U-Boot build does
MANYJ = -j .text -j .secure_text -j .secure_data -j .rodata -j .data -j .u_boot_list -j .rela.dyn -j .got -j .got.plt -j .binman_sym_table -j .text_rest -j .dtb.init.rodata -j .efi_runtime -j .efi_runtime_rel

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

bl31.bin: bl31.elf
	aarch64-linux-gnu-objcopy -O binary bl31.elf bl31.bin

u-boot.bin: u-boot.elf
	aarch64-linux-gnu-objcopy  $(MANYJ) -O binary  u-boot.elf u-boot.bin

u-boot.its: bl31.bin mksunxi_fit_atf.sh
	./mksunxi_fit_atf.sh sun50i-h5-orangepi-pc2.dtb > u-boot.its


# mkimage  -f u-boot.its -E u-boot.itb >/dev/null  && cat /dev/null
u-boot.img: u-boot.its u-boot.bin
	./mkimage  -f u-boot.its -E u-boot.img

# SPL image is always 32K
# u-boot-with-spl.img:
#	cat u-boot-spl.img u-boot.img > u-boot-with-spl.bin

clean:
	rm -f u-boot.its
	rm *.bin
	rm *.img

# THE END
