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

/* The show starts here.
 */
void
main ( void )
{
	int val;
	int val1, val2;
	// u64 val64;

	serial_init ( 115200 );

	// arm_float ();

	// show_stack ();

	ccnt_enable ( 0 );

	// stopwatch ();

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
	blink ();

	printf ( "Main returns to spin\n" );
}

/* Notes on the CPSR.
 * We see CPSR: 600001D3
 * - the upper 4 bits are condition codes (NCZV)
 * - the low 5 bits are the processor mode (here 0x13)
 * -- 0x13 is SVC
 * -- 0x11 is FIQ
 * -- 0x12 is IRQ
 * -- 0x17 is Abort
 * -- 0x1B is Undef
 * -- 0x1F is System
 * - the 0x1D is IFT-
 * -- I is the IRQ mask (now disabled)
 * -- F is the FIQ mask (now disabled)
 * -- T is the thumb bit (disabled)
 *
 *   CPSR:  600001D3
 *   SCTLR: 00C5187D
 *   ACTLR: 00006040
 *
 * To work with these, seacrh on Cortex A7 MPcore SCTLR and
 *   find the specific manual for this exact core.
 *
 * SCTLR = System Control Register
 *  low 4 bits (0-3) are -CAM = D = 1101
 *   m = 1 address translation enabled (MMU on)
 *   a = 0 alignment fault checking disabled
 *   d = 1 data and unfied caches enabled
 *  bits 12-15 are --VI = 0001
 *   v = 0 vectors normal, can be remapped with VBAR
 *   i = 1 instruction cache enabled.
 *
 * There are lots more bits in the SCTLR, but the
 * cache and MMU bits are all I am interested in now.
 *
 * And I'm not even looking at the ACTLR for details
 *  It has a variety of bits that relate to the cache
 *
 *  TTBR0: 7FFF4000
 *  TTBR1: 40044059, TTBR1: C485A0B2
 *  TTBCR: 80000F00
 *
 * These are described in the ARMv7-A manual
 * First the TTBCR --
 * The high bit says use EAE (40 bit translation tables)
 * The 0F00 sets bits 11-8 to ones  these are OOII and control
 *	inner/outer cacheability
 * TTBR1 is just random junk and changes every power cycle

TT 0: 7FFF4000 - 7FFF0003
TT 1: 7FFF4004 - 00000000
TT 2: 7FFF4008 - 7FFF1003
TT 3: 7FFF400C - 00000000
TT 4: 7FFF4010 - 7FFF2003
TT 5: 7FFF4014 - 00000000
TT 6: 7FFF4018 - 7FFF3003
TT 7: 7FFF401C - 00000000
TT 8: 7FFF4020 - FFFFFFFF
TT 9: 7FFF4024 - FFFFFFFF

Kyu does this:

MMU setup for Orange Pi
mmu_setup, page table at: 40044000
Ram at 40000000, 1073741824 bytes (1048576K)
TJT in mmu_set_ttbr, page table: 40044000
 set TTBCR to 0, read back: 00000000
 set TTBR0 to 40044059, read back: 40044059
 set TTBR1 to 40044059, read back: 40044059
 set DACR to ffffffff, read back: ffffffff
  ======================== We made it to kyu_startup!
orig SCTLR = 00c5187d
orig ACTLR = 00006040
orig    SP = 79f684a8
orig TTBR0 = 7fff4000
orig TTBR1 = c485a0b2
orig TTBCR = 80000f00
orig DACR  = 55555555
SCTLR = 00c5187d
ACTLR = 00006000

 * ======================================================
 */

/* These are 64 bit items in little endian order, so the first 32 bits
 * we read are the low bits.
 */
void
tt_dump ( int base, int count )
{
	int i;
	//int *bp;
	u64 *up;

	//bp = (int *) base;
	up = (u64 *) base;

	for ( i=0; i<count; i++ ) {
	    //printf ( "TT %d: %08x - %08x %08x\n", i, bp, bp[1], bp[0] );
	    printf ( "TZ %d: %08x - %Z\n", i, up, up[0] );
	    //bp++;
	    //bp++;
	    up++;
	}
}

/* ============================================================ */
/* ============================================================ */
/* ============================================================ */

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
