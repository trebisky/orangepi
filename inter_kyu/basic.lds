SECTIONS
{
	. = 0x42000000;
	.start    : { *(.start) }
	.text     : { *(.text) }

	_etext = .;           /* End of text section */

	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
	/DISCARD/ : { *(.note*) }

        .rodata : { *(.rodata) *(.rodata.*) }

	.data : {                     /* Data */
            *(.data)
        }
	_edata = .;                   /* End of data section */


	__bss_start__ = .;            /* BSS */
	.bss : { *(.bss) }
	__bss_end__ = . ;
	_end = . ;
}

/*
.note.gnu.build-id section discarded, --build-id ignored.
*/
