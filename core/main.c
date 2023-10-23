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

typedef unsigned long long u64;

static void show_stack ( void );
static void stopwatch ( void );
void tt_dump ( int, int );

#define get_SP(x)       asm volatile ("add %0, sp, #0\n" :"=r" ( x ) )

#define get_CPSR(val)   asm volatile ( "mrs %0, cpsr" : "=r" ( val ) )
#define set_CPSR(val)   asm volatile ( "msr cpsr, %0" : : "r" ( val ) )

#define get_SCTLR(val)  asm volatile ( "mrc p15, 0, %0, c1, c0, 0" : "=r" ( val ) )
#define set_SCTLR(val)  asm volatile ( "mcr p15, 0, %0, c1, c0, 0" : : "r" ( val ) )

#define get_ACTLR(val)  asm volatile ( "mrc p15, 0, %0, c1, c0, 1" : "=r" ( val ) )
#define set_ACTLR(val)  asm volatile ( "mcr p15, 0, %0, c1, c0, 1" : : "r" ( val ) )

#define get_TTBR0(val)  asm volatile ( "mrc p15, 0, %0, c2, c0, 0" : "=r" ( val ) )
#define set_TTBR0(val)  asm volatile ( "mcr p15, 0, %0, c2, c0, 0" : : "r" ( val ) )

// MRRC p15, 0, <Rt>, <Rt2>, c2 ; Read 64-bit TTBR0 into Rt (low word) and Rt2 (high word)
// asm volatile("mrrc p15, 0, %0, %1, c14" : "=r" (nowl), "=r" (nowu));
// val1 is low, val2 is high
#define get_XTTBR0(val1, val2)  asm volatile ( "mrrc p15, 0, %0, %1, c2" : "=r" ( val1 ), "=r" ( val2 ) )

#define get_TTBR1(val)  asm volatile ( "mrc p15, 0, %0, c2, c0, 1" : "=r" ( val ) )
#define set_TTBR1(val)  asm volatile ( "mcr p15, 0, %0, c2, c0, 1" : : "r" ( val ) )

#define get_TTBCR(val)  asm volatile ( "mrc p15, 0, %0, c2, c0, 2" : "=r" ( val ) )
#define set_TTBCR(val)  asm volatile ( "mcr p15, 0, %0, c2, c0, 2" : : "r" ( val ) )

#define set_DACR(val)   asm volatile ( "mcr p15, 0, %0, c3, c0, 0" : : "r" ( val ) )
#define get_DACR(val)   asm volatile ( "mrc p15, 0, %0, c3, c0, 0" : "=r" ( val ) )

#define get_VBAR(val)   asm volatile ( "mrc p15, 0, %0, c12, c0, 0" : "=r" ( val ) )
#define set_VBAR(val)   asm volatile ( "mcr p15, 0, %0, c12, c0, 0" : : "r" ( val ) )

// The ID_PFR0 and ID_PFR1 registers.
#define get_PFR0(val)   asm volatile ( "mrc p15, 0, %0, c0, c1, 0" : "=r" ( val ) )
#define get_PFR1(val)   asm volatile ( "mrc p15, 0, %0, c0, c1, 1" : "=r" ( val ) )

// #define INT_lock        
#define int_DISABLE        \
        asm volatile (  "mrs     r0, cpsr; \
                        orr     r0, r0, #0xc0; \
                        msr     cpsr, r0" ::: "r0" )

// #define INT_unlock     
#define int_ENABLE      \
        asm volatile (  "mrs     r0, cpsr; \
                        bic     r0, r0, #0xc0; \
                        msr     cpsr, r0" ::: "r0" )


/* The show starts here.
 */
void
main ( void )
{
	int val;
	int val1, val2;
	// u64 val64;

	gpio_init ();
	serial_init ( 115200 );

	printf ( "Core demo starting 10-22-2023\n" );

	gic_init ();
	blink_setup ();

	// show_stack ();

	ccnt_enable ( 0 );

	int_ENABLE;

#ifdef notdef
	// printf ( "u64 is %d bytes\n", sizeof(u64) );

	val = get_cpu_clock ();
	printf ( "CPU clock found set to: %d\n", val );

	get_CPSR ( val );
	printf ( "CPSR: %08x\n", val );

	get_SCTLR ( val );
	printf ( "SCTLR: %08x\n", val );

	get_ACTLR ( val );
	printf ( "ACTLR: %08x\n", val );

	get_TTBCR ( val );
	printf ( "TTBCR: %08x\n", val );

	get_TTBR0 ( val );
	printf ( "TTBR0: %08x\n", val );

	/* val2 is high, val1 is low */
	get_XTTBR0 ( val1, val2 );
	printf ( "TTBR0: %08x %08x\n", val2, val1 );

	/* Random junk every power cycle */
	// get_TTBR1 ( val );
	// printf ( "TTBR1: %08x\n", val );

	get_PFR0 ( val );
	printf ( "PFR0: %08x\n", val );
	get_PFR1 ( val );
	printf ( "PFR1: %08x\n", val );

	get_TTBR0 ( val );
	tt_dump ( val, 5 );

	tt_dump ( 0x7fff0000, 4 );
	tt_dump ( 0x7fff1000, 4 );
	tt_dump ( 0x7fff2000, 4 );
	tt_dump ( 0x7fff3000, 4 );

	printf ( "Main blinks ...\n" );
	// blink ();
#endif

	printf ( "Main spins on WFE\n" );
	for ( ;; )
	    asm volatile ( "wfe" );
}

/* THE END */
