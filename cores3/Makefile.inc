# Makefile.inc
# 
# These are the common definitions for building on the ARM processor
#
# Tom Trebisky  5-18-2015  12-12-2016

CCX = arm-linux-gnu-gcc

# For a long time it was an issue to keep stdarg.h in sync
#  with the compiler.  Then I realized it was best to just let
#  the build system find it in the "usual spot", but the usual
#  spot changed with each compiler version update.
#  Then I realized I could just ask the compiler for its
#  version number

# Note that I copied stdarg.h from here.
# I probably should arange so the compiler looks
# here by default.
#  /usr/lib/gcc/arm-linux-gnueabi/4.9.1/include/stdarg.h
# And the ABI option is the way to do it:

# This has to be adjusted every time a new compiler version comes along.
# As of 5-26-2016 ...
#GCCVERSION = 5.3.1
# As of 8-30-2016 (Fedora 24)
# As of 9-17-2016 (Fedora 24)
#GCCVERSION = 6.1.1

GCCVERSION = `$(CCX) -dumpversion`

#ABI = -isystem /usr/lib/gcc/arm-linux-gnueabi/4.9.2/include
#ABI = -isystem /usr/lib/gcc/arm-linux-gnueabi/5.1.1/include
ABI = -isystem /usr/lib/gcc/arm-linux-gnueabi/$(GCCVERSION)/include

# IMPORTANT --
# I have tried several times to use the gcc builtin functions
# and it always leads to misery and heartache.
# I honestly don't think it will work on an embedded system
# without the standard glibc
# Be warned.

# Perhaps I should find a way to cooperate with gcc and printf.
# It optimizes simple calls to just a puts call, which ought to
# be fine.  I need the builtin ffs() function, (do I????)
#  so I need to selectively turn off printf

#NOB = -fno-builtin-printf -fno-builtin-putchar -fno-builtin-puts
#NOB = -fno-builtin
# XXX - turning off these warnings is only a temporary thing,
#  this should be enabled and all necessary prototypes added.
NOB = -fno-builtin -Wno-implicit-function-declaration

OPT = -fno-omit-frame-pointer
#OPT = -Os
#OPT =

#COPTS = -DKYU -nostdinc -fno-builtin -marm -march=armv7-a -msoft-float $(INCS)
#COPTS = -DKYU -nostdinc -fno-builtin -Os -marm -march=armv7-a -msoft-float $(INCS)
#COPTS = -DKYU -nostdinc $(NOB) -Os -marm -march=armv7-a -msoft-float $(INCS)
#COPTS = -DKYU -nostdinc $(NOB) -marm -march=armv7-a -msoft-float $(INCS)

COPTS = -DKYU -nostdinc $(NOB) $(OPT) -marm -march=armv7-a -msoft-float $(INCS) $(ABI) $(DEFS)

CC = $(CCX) $(COPTS)

# It is a bug that we cannot invoke gcc to do this,
# but doing this directly gets the job done.
LD = arm-linux-gnu-ld.bfd

# Should we get this via linux arch/arm/asm/lib ??
# for LINUX comment this out -----
#LIBS = -L /usr/lib/gcc/arm-linux-gnueabi/5.1.1 -lgcc
LIBS = -L /usr/lib/gcc/arm-linux-gnueabi/$(GCCVERSION) -lgcc

BIN = arm-linux-gnu-objcopy -O binary
DUMP = arm-linux-gnu-objdump -d
NM = arm-linux-gnu-nm

.c.o:
	$(CC) -o $@ -c $<

# from linux 4.0.5 build ---
#  arm-linux-gnu-gcc -Wp,-MD,arch/arm/lib/.memcpy.o.d  -nostdinc -isystem /usr/lib/gcc/arm-linux-gnueabi/4.9.2/include -I./arch/arm/include -Iarch/arm/include/generated/uapi -Iarch/arm/include/generated  -Iinclude -I./arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-versatile/include -Iarch/arm/plat-versatile/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float         -c -o arch/arm/lib/memcpy.o arch/arm/lib/memcpy.S

.S.o:
	$(CC) $(AOPTS) -o $@ -c $<

.c.s:
	$(CC) -S $<
