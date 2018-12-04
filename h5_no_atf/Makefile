# Makefile to build FIT image for PC2 (H5) U-Boot
# Tom Trebisky  12-3-2018

tools: mksunxiboot

MANYJ = -j .text -j .secure_text -j .secure_data -j .rodata -j .data -j .u_boot_list -j .rela.dyn -j .got -j .got.plt -j .binman_sym_table -j .text_rest -j .dtb.init.rodata -j .efi_runtime -j .efi_runtime_rel

u-boot-spl.bin: u-boot-spl.elf
	aarch64-linux-gnu-objcopy  $(MANYJ) -O binary  u-boot-spl.elf u-boot-spl.bin

# cc -Wp,-MD,tools/.mksunxiboot.d -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer  -std=gnu11
#   -include ./include/compiler.h -idirafterinclude -idirafter./arch/arm/include -I./scripts/dtc/libfdt -I./tools
#  -DUSE_HOSTCC -D__KERNEL_STRICT_NAMES -D_GNU_SOURCE
#  -o tools/mksunxiboot tools/mksunxiboot.c

mksunxiboot: mksunxiboot.c spl.h
	cc -o mksunxiboot mksunxiboot.c

# ./tools/mksunxiboot --default-dt "sun50i-h5-orangepi-pc2" spl/u-boot-spl.bin spl/sunxi-spl.bin

# THE END
