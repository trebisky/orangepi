OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

ENTRY(Startup)

SECTIONS
{
        .text 0x44000 :
        {
                *(.text)
        }

        . = ALIGN(4);
        .rodata : { *(.rodata*) }

        . = ALIGN(4);
        .data : { *(.data*) }

        . = ALIGN(4);
        .got : { *(.got) }

        _end = .;

        . = ALIGN(16); /* need 8, use 16 for even calculation of image size in NSIH header */
        __bss_start__ = .;
        .bss_start (OVERLAY) : {
                KEEP(*(.__bss_start));
                __bss_base = .;
        }
        .bss __bss_base (OVERLAY) : {
                *(.bss*)
                . = ALIGN(4);
                __bss_limit = .;
        }
        .bss_end __bss_limit (OVERLAY) : {
                KEEP(*(.__bss_end));
        }
        __bss_end__ = .;
}
