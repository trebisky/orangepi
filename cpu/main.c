/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * main.c for the Orange Pi PC and PC Plus
 *
 * The game here is to discover how U-Boot passes us the CPU.
 * What speed is the clock running?
 * is the cache and/or MMU enabled.
 *
 * Tom Trebisky  5/13/2017 
 * Tom Trebisky  10/17/2023
 */

#include "protos.h"

static void show_stack ( void );
static void stopwatch ( void );

/* The show starts here.
 */
void
main ( void )
{
	int val;

	serial_init ( 115200 );

	// arm_float ();

	// show_stack ();

	ccnt_enable ( 0 );

	// stopwatch ();

	val = get_cpu_clock ();
	printf ( "CPU clock found set to: %d\n", val );

	blink ();

	printf ( "Main returns to spin\n" );
}

/* This shows us:
 * SP from U-boot: 79F68498
 * --
 * Nothing tremendously exciting.
 * We have 1G of ram from 0x4000_0000 thru 0x7fff_ffff
 * so U-boot puts its stack near the end of
 * whatever RAM it finds.
 */
static void
show_stack ( void )
{
	int cur_sp;

	asm volatile ("add %0, sp, #0" : "=r"(cur_sp) );
	printf ( "SP from U-boot: %08x\n", cur_sp );
}

/* I measure 2.4 seconds with a stopwatch to get
 * 24 * 100 * 1024 * 1024 counts.
 * 10 * 100 * 1024 * 1024 counts per second.
 * i.e. the CPU is running at 1000 Mhz
 */

static void
stopwatch ( void )
{
	unsigned int target;
	int num;

	target = 10 * 24 * 1024 * 1024;
        printf ( " target = %08x\n", target );

	printf ( "Start\n" );

	for ( num = 0 ; num < 10 ; num++ ) {
	    ccnt_reset ();
	    while ( ccnt_read () < target )
		;
	}

/* We see:
 *  target = 0F000000
 *  CCNT = 0F00003D
 */
        // printf ( " CCNT = %08x\n", ccnt_read() );
	printf ( "Done\n" );
}

/* THE END */
