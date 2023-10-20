SECTIONS
{
	. = 0x40000000;
	.start    : { *(.start) }
	.text     : { *(.text) }
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
	/DISCARD/ : { *(.note*) }
}

/*
.note.gnu.build-id section discarded, --build-id ignored.
*/
