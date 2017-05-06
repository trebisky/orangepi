/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * cpu.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  1/19/2017
 *
 */

#define CPUCFG_BASE     0x01f01c00
#define PRCM_BASE       0x01f01400

/* The CPUCFG registers are in a 1K section starting at 0x01f01c00
 * The ROM_START register is in here and is undocumented in the H3 datasheet.
 * See page 143 in the datasheet.
 * The overall address map is on page 84
 */

// #define ROM_START       ((volatile unsigned long *) 0x01f01da4)
#define ROM_START        ((volatile unsigned long *) (CPUCFG_BASE + 0x1A4))

/* We don't need these, but want to play with them */
#define PRIVA        ((volatile unsigned long *) (CPUCFG_BASE + 0x1A8))
#define PRIVB        ((volatile unsigned long *) (CPUCFG_BASE + 0x1AC))

#define GEN_CTRL        ((volatile unsigned long *) (CPUCFG_BASE + 0x184))

// #define DBG_CTRL1       ((unsigned long *) (CPUCFG_BASE + 0x1e4))

/* These two are suggestive */

/* This has one bit, set low to assert it */
#define CPU_SYS_RESET      ((volatile unsigned long *) (CPUCFG_BASE + 0x140))

/* This comes up set to 0x10f after reset they say,
 * and that is exactly how I find it.
 * I can set it to 0x101 and everything still runs fine
 * However, if I set it to 0x102, the system hangs,
 *  so I think I understand (and can ignore) this register.
 */
#define CPU_CLK_GATE       ((volatile unsigned long *) (CPUCFG_BASE + 0x144))
#define GATE_L2		0x100
#define GATE_CPU0	0x01
#define GATE_CPU1	0x02
#define GATE_CPU2	0x04
#define GATE_CPU3	0x08

#define POWER_OFF       ((unsigned long *) (PRCM_BASE + 0x100))

/* When any ARM cpu starts running, it will set the PC to 0xffff0000
 * This is the location of the H3 bootrom code.
 * This code does clever things for a second core.
 * In particular, it reads the processor affinity register and
 *  discovers that the low 2 bits are non-zero (the core number).
 * Once it discovers this, it loads the PC from the value at 0x01f01da4
 *
 * Also as a side note on the bootrom.  The data sheet says various
 *  contradictory things about it (such as it being 32K, 64K, and 96K)
 * What I discover is that it is in fact a 32K image at ffff0000
 * There is an exact second copy immediately following at ffff8000
 * This is probably a redundant address decode, but who knows.
 */

void test_core ( void );

typedef void (*vfptr) ( void );

// extern unsigned long core_stacks;
unsigned long core_stacks;

void puts ( char *msg )
{
	serial_puts ( msg );
}

#include "kyu.h"
#include "thread.h"

struct thread bogus_thread;
struct thread *cur_thread;

void do_undefined_instruction ( void ) { puts ("BAD\n"); }
void do_software_interrupt ( void ) { puts ("BAD\n"); }
void do_prefetch_abort ( void ) { puts ("BAD\n"); }
void do_data_abort ( void ) { puts ("BAD\n"); }
void do_not_used ( void ) { puts ("BAD\n"); }
void do_irq ( void ) { puts ("BAD\n"); }
void do_fiq ( void ) { puts ("BAD\n"); }

/* This is in locore.S */
extern void newcore ( void );

/* Use for sub millisecond delays
 * argument is microseconds.
 */
void
core_delay ( int n )
{
    volatile long x = 250 * n;

    while ( x-- > 0 )
        ;
}

void
launch_core ( int cpu )
{
        volatile unsigned long *reset;
        unsigned long mask = 1 << cpu;

        reset = (volatile unsigned long *) ( CPUCFG_BASE + (cpu+1) * 0x40);
	// printf ( "-- reset = %08x\n", reset );

        *ROM_START = (unsigned long) newcore;  /* in locore.S */

        *reset = 0;                     /* put core into reset */

        *GEN_CTRL &= ~mask;             /* reset L1 cache */
        *POWER_OFF &= ~mask;            /* power on */
	// thr_delay ( 2 );
	//_udelay ( 2000 );
	core_delay ( 2000 );

        *reset = 3;			/* take out of reset */
}

// #define SENTINEL	ROM_START
#define SENTINEL	(volatile unsigned long *) 4

#ifdef notdef
void
watch_core ( void )
{
	volatile unsigned long *sent;
	int i;

	sent = SENTINEL;

	printf ( "\n" );
	printf ( "Sentinel addr: %08x\n", sent );

	for ( i=0; i<40; i++ ) {
	    thr_delay ( 100 );
	    // printf ( "Core status: %08x\n", *sent );
	    if ( *sent == 0 ) {
		printf ( "Core started !!\n" );
		return;
	    }
	}
	printf ( "** Core failed to start\n" );

}
#endif

/* Most of the time a core takes 30 counts to start */
#define MAX_CORE	100

int
wait_core ( void )
{
	volatile unsigned long *sent;
	int i;

	sent = SENTINEL;

	for ( i=0; i<MAX_CORE; i++ ) {
	    if ( *sent == 0 ) {
		// printf ( "Core started in %d\n", i );
		return 1;
	    }
	}
	return 0;
}

static void
test_one ( int cpu )
{
	int stat;

	*SENTINEL = 0xdeadbeef;

	// printf ( "Starting core %d ...\n", cpu );
	puts ( "Starting core ...\n" );
	launch_core ( cpu );

	// watch_core ();
	stat = wait_core ();
	/*
	if ( stat )
	    printf ( " Core %d verified to start\n", cpu );
	else
	    printf ( "** Core %d failed to start\n", cpu );
	    */
	if ( stat )
	    puts ( " Core verified to start\n" );
	else
	    puts ( "** Core failed to start\n" );


	*ROM_START = 0;
}

#ifdef notdef
void
test_reg ( volatile unsigned long *reg )
{
	unsigned long val;

	printf ( "Test REG, read %08x as %08x\n", reg, *reg );
	*reg = val = 0;
	printf ( "Test REG, set %08x: read %08x as %08x\n", val, reg, *reg );
	*reg = val = 0xdeadbeef;
	printf ( "Test REG, set %08x: read %08x as %08x\n", val, reg, *reg );
	*reg = val = 0;
	printf ( "Test REG, set %08x: read %08x as %08x\n", val, reg, *reg );
}
#endif

/* This gets called by the test menu
 *   (or something of the sort)
 */
void
test_core ( void )
{
	int reg;

	led_init ();
	serial_init ( 115200 );
	core_stacks = 0x50000000;
	cur_thread = & bogus_thread;

#ifdef notdef
	asm volatile ("mrs %0, cpsr\n" : "=r"(reg) : : "cc" );
	printf ( "CPSR  = %08x\n", reg );
	asm volatile ("mrs %0, cpsr\n" : "=r"(reg) : : "cc" );
	printf ( "CPSR  = %08x\n", reg );
	printf ( "CPSR  = %08x\n", get_cpsr() );
	printf ( "CPSR  = %08x\n", get_cpsr() );

	/* SCTRL */
        asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
	printf ( "SCTRL = %08x\n", reg );
#endif

#ifdef notdef
	unsigned long val;
	val = *CPU_CLK_GATE;
	printf ( "Gate register: %08x\n", val );
	*CPU_CLK_GATE = 0x101;
	val = *CPU_CLK_GATE;
	printf ( "Gate register: %08x\n", val );
#endif

	// printf ( "Address of core stacks: %08x\n", core_stacks );
	test_one ( 1 );
	test_one ( 2 );
	test_one ( 3 );

	/*
	test_reg ( ROM_START );
	test_reg ( PRIVA );
	test_reg ( PRIVB );
	*/

	puts ( "Done\n" );

}

/* If all goes well, we will be running here,
 * And indeed we are, this is the first C code
 * run by a new core.
 */

/* Runs mighty slow without D cache enabled */
static void
delay_ms_cache ( int msecs )
{
        volatile int count = 100000 * msecs;

        while ( count-- )
            ;
}

static void
delay_ms ( int msecs )
{
	delay_ms_cache ( msecs );
	// delay_ms_nocache ( msecs );
}

#define CORE_QUIET

/* This is the first C code a new core runs */
void
kyu_newcore ( int core )
{
	volatile unsigned long *sent;
	unsigned long sp;
	int val = 0;
	int cpu = 99;

	sent = SENTINEL;
	*sent = 0;

	/* Read processor affinity register */
	asm volatile("mrc 15, 0, %0, cr0, cr0, 5" : "=r" (cpu) : : "cc");
	cpu &= 0x3;

	asm volatile ("add %0, sp, #0\n" :"=r"(sp));

#ifndef CORE_QUIET
	/* Makes a mess without synchronization */
	printf ( "Core %d running with sp = %08x\n", cpu, sp );
	printf ( "Core %d core (arg) = %08x\n", cpu, core );
	// printf ( "Core %d running\n", cpu );
#endif

#ifdef notdef
	for ( ;; ) {
	    *sent = val++;
	    delay_ms ( 1 );
	    if ( val % 5 == 0 )
		printf ( "Tick !!\n" );
	    if ( *ROM_START == 0 )
		break;
	}
#endif

	if ( cpu == 1 )
	    gpio_blink_red ();

	for ( ;; )
	    ;
}

/* THE END */
