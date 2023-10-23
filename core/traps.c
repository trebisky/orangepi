/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * traps.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  5/7/2017
 *
 */

void trap ( char *who )
{
	int cpu;

	serial_puts ( "---------------------- TRAP ------------------------\n" );
	serial_puts ( "---------------------- TRAP ------------------------\n" );

	/* Read processor affinity register */
        asm volatile("mrc 15, 0, %0, cr0, cr0, 5" : "=r" (cpu) : : "cc");
        cpu &= 0x3;
	print_num ( "Core: ", cpu );

	print_str ( "  type:", who );
	print_hex ( "TEST: ", 0xc0ffee );
	print_num ( "NUM: ", 1234 );

	serial_puts ( "---------------------- TRAP ------------------------\n" );
	serial_puts ( "---------------------- TRAP ------------------------\n" );

	serial_puts ( "Halted (spinning)\n" );
	for ( ;; ) ;
}

void trap_ui ( void ) { trap ( "undefined instruction" ); }
void trap_si ( void ) { trap ( "software interrupt" ); }
void trap_pa ( void ) { trap ( "prefetch abort" ); }
void trap_da ( void ) { trap ( "data abort" ); }
void trap_nu ( void ) { trap ( "not used" ); }
void trap_irq ( void ) { trap ( "IRQ" ); }
void trap_fiq ( void ) { trap ( "FIQ" ); }

/* Hook for gcc arithmetic for divide by zero */
void raise ( void ) {}

/* THE END */
