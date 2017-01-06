/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */
/* interrupts.c
 * - was trap.c on the x86
 */

#define HACK

#ifdef HACK
typedef void (*vfptr) ( void );

/* List of fault codes */
/* The first 8 are ARM hardware exceptions and interrupts */
#define F_NONE  0
#define F_UNDEF 1
#define F_SWI   2
#define F_PABT  3
#define F_DABT  4
#define F_NU    5
#define F_FIQ   6       /* not a fault */
#define F_IRQ   7       /* not a fault */

/* For Allwinner H3 */
#define NUM_INTS        157

#else
#include <kyu.h>
#include <kyulib.h>
#include <thread.h>

#include "hardware.h"
#include "interrupts.h"
#endif

#ifndef HACK
/* -------------------------------------------- */
/* This is a pseudo exception to satisfy
 * linux code for a divide by zero.
 * It is called from:
 *  ./arch/arm/lib/lib1funcs.S:	bl	__div0
 *  ./arch/arm/lib/div64.S:	bl	__div0
 *
 * Note that on the ARM this does NOT happen
 *  at interrupt level, so it is easy to handle.
 * This works great.  8-14-2016
 */
void __div0 ( void )
{
	printf ("divide by zero");
	thr_block ( FAULT );
}
#endif


#ifdef HACK

#define MAX_TNAME       10

/* We have to save all registers here, since
 * an interrupt is entirely unpredictable.
 */
struct int_regs {
        int regs[17];
};

struct thread {
        struct int_regs iregs;          /* must be first */
        char name[MAX_TNAME];
#ifdef notdef
        struct jmp_regs regs;
        struct cont_regs cregs;         /* new for ARM */
        int prof;
        struct thread *next;            /* all threads */
        struct thread *wnext;           /* waiting list */
        enum thread_state state;
        enum thread_mode mode;          /* how to resume */
        enum console_mode con_mode;     /* type of console */
        char *stack;
        int stack_size;
        int pri;
        int delay;
        int rep_reload;
        int rep_count;
        struct thread *rep_next;        /* list of repeating threads */
        int overruns;
        int fault;              /* why we are suspended */
        char name[MAX_TNAME];
        int flags;
        struct thread *join;    /* who wants to join us */
        struct thread *yield;   /* who yielded to us */
#endif
};

struct thread *cur_thread;

/* from kyu - kyulib.c */
/* multi line dump, longword by longword.
 *  count gives lines on screen.
 *  (may get peculiar byte swapping).
 */
void
dump_l ( void *addr, int n )
{
        unsigned long *p;
        int i;

        p = (unsigned long *) addr;

        while ( n-- ) {
            printf ( "%08x  ", (long) addr );

            for ( i=0; i<4; i++ )
                printf ( "%08x ", *p++ );

            printf ( "\n" );
            addr += 16;
        }
}

/* For sanity when testing */
#define LIMITS

/* Dump this many words */
void
dump_ln ( void *addr, int nw )
{
#ifdef LIMITS
        if ( nw < 1 ) nw = 1;
        if ( nw > 1024 ) nw = 1024;
#endif

        dump_l ( addr, (nw+3) / 4 );
}

/* From kyu - symbols.c */

// #define THR_STACK_BASE  0x98000000
#define THR_STACK_BASE  0x40000000

/* Do a stack traceback -- ARM specific
 */
void
unroll_fp ( int *fp )
{
        int limit;
        // char *msg;

        /* could also check is fp ever moves to lower addresses on stack and stop */
        limit = 16;
        while ( limit > 0 && fp ) {
            if ( fp[0] < THR_STACK_BASE ) {
                // msg = mk_symaddr ( fp[0] );
                // printf ( "Called from %s -- %08x\n", msg, fp[0] );
                printf ( "Called from -- %08x\n", fp[0] );
                fp = (int *) fp[-1];
            } else {
                printf ( "Leaf routine\n" );
                fp = (int *) fp[0];
            }
            // printf ( "Called from %s -- %08x, (next fp = %08x)\n", msg, fp[0], fp[-1] );
            limit--;
        }
}


/* From kyu console.c */
void
panic ( char *msg )
{
        if ( msg )
            printf ( "PANIC: %s\n", msg );
        else
            printf ( "PANIC\n" );

/*        thr_block ( FAULT ); */
	spin ();

}


/* From kyu thread.c */

static long in_interrupt;

void
start_interrupt ( void )
{
        in_interrupt = 1;
}

void
finish_interrupt ( void )
{
        struct thread *tp;

#ifdef notdef
/* XXX - only for this experiment */
        if ( in_newtp ) {
            tp = in_newtp;
            in_newtp = (struct thread *) 0;
            in_interrupt = 0;
            change_thread ( tp, RSF_INTER );
            /* NOTREACHED */

            panic ( "finish_interrupt , change_thread" );
        }
#endif

        in_interrupt = 0;
        resume_i ( &cur_thread->iregs );
        /* NOTREACHED */

        panic ( "finish_interrupt , resume_i" );
}

/* This is rarely called, since we hardly ever actually
 * want to return from an exception to the faulted thread.
 */
void
finish_exception ( void )
{
        resume_i ( &cur_thread->iregs );
        /* NOTREACHED */

        panic ( "finish_exception , resume_i" );
}

struct arm_regs {
        long uregs[17];
};

#define ARM_sp          uregs[13]

void
show_thread_regs ( struct thread *tp )
{
        struct arm_regs *regs;

        regs = (struct arm_regs *) tp->iregs.regs;

        printf ( "\n" );
        show_regs ( regs );
        show_stack ( (unsigned long) regs->ARM_sp );
}

struct thread dummy;

void
init_thread ( void )
{
	cur_thread = &dummy;
}

#else
extern struct thread *cur_thread;
#endif

static vfptr data_abort_hook;

/* mark the offending thread and abandon it.
 *
 * For a data abort (which is far and away the common
 * issue on the ARM, the stack is perfectly healthy
 * and pertinent in fact.
 *
 * What we want to do is: thr_block ( FAULT );
 * but from interrupt level.
 *
 * An interrupt (or exception) saves all the registers
 * into "iregs", conveniently right at the start of
 * the thread structure.  An array of 17 on ARM.
 *
 * -----------------------
 *
 * To just return usually means the exception hits us again
 * immediately and we get a flood of stupid output.
 * Resetting the cpu (like U-boot) just puts us into a slower loop.
 * To spin is bad, but at least we get sensible output.
 */

// char *mk_symaddr(int);

void
evil_exception ( char *msg, int code )
{
	int pc;

	printf ( "%s in thread %s\n", msg, cur_thread->name );

	show_thread_regs ( cur_thread );

#define ARM_FP	11
#define ARM_PC	15
	pc = cur_thread->iregs.regs[ARM_PC];
	// printf ( "PC = %08x ( %s )\n", pc, mk_symaddr(pc) );
	printf ( "PC = %08x\n", pc );

	unroll_fp ( (int *) cur_thread->iregs.regs[ARM_FP] );

	/* Let code in thread.c handle this */
	// thr_suspend ( code );
	spin ();
}

void
data_abort_hookup ( vfptr new )
{
	data_abort_hook = new;
}

static int data_abort_flag;

/* If we don't bump the PC, we just return to faulted instruction
 * and get into a vicious loop
 */
void
data_abort_handler ( void )
{
	data_abort_flag = 1;
	cur_thread->iregs.regs[ARM_PC] += 4;
}

int
data_abort_probe ( unsigned long *addr )
{
	int val;

	data_abort_hookup ( data_abort_handler );

	data_abort_flag = 0;
	val = *addr;

	data_abort_hookup ( (vfptr) 0 );

	return data_abort_flag;
}

void do_undefined_instruction ( void )
{
	evil_exception ("undefined instruction", F_UNDEF);

	/* NOTREACHED */
	finish_exception ();
}

void do_software_interrupt ( void )
{
	evil_exception ("software interrupt", F_SWI);

	/* NOTREACHED */
	finish_exception ();
}

void do_prefetch_abort ( void )
{
	evil_exception ("prefetch abort", F_PABT);

	/* NOTREACHED */
	finish_exception ();
}

void do_data_abort ( void )
{
	if ( data_abort_hook ) {
	    (*data_abort_hook) ();
	    finish_exception ();
	} else
	    evil_exception ("data abort", F_DABT);
}

void do_not_used ( void )
{
	evil_exception ("not used", F_NU);

	/* NOTREACHED */
	finish_exception ();
}

/* evil for now */
void do_fiq ( void )
{
	evil_exception ("fast interrupt request", F_FIQ);

	/* NOTREACHED */
	finish_exception ();
}

/* -------------------------------------------- */

typedef void (*irq_fptr) ( void * );

struct irq_info {
	irq_fptr func;
	void *	arg;
};

static struct irq_info irq_table[NUM_INTS];

/* Here is the user routine to connect/disconnect
 * a C routine to an interrupt.
 */
void
irq_hookup ( int irq, irq_fptr func, void *arg )
{
	if ( irq < 0 || irq >= NUM_INTS )
	    panic ( "irq_hookup: not available" );

	/*
	printf ( "irq_hookup: %d, %08x, %d\n", irq, func, (int) arg );
	*/

	if ( func ) {
	    irq_table[irq].func = func;
	    irq_table[irq].arg = arg;
	    // intcon_ena ( irq );
	} else {
	    // intcon_dis ( irq );
	    irq_table[irq].func = (irq_fptr) 0;
	    irq_table[irq].arg = (void *) 0;
	}
}

#ifdef notdef
static void
special_debug ( void )
{
	printf ( "timer_int sp = %08x\n", get_sp () );
	printf ( "timer_int sr = %08x\n", get_cpsr () );
	printf ( "timer_int ssp = %08x\n", get_ssp () );
	printf ( "timer_int sr = %08x\n", get_cpsr () );
	printf ( "timer_int sp = %08x\n", get_sp () );
	spin ();
}
#endif


/* Interrupt handler, called at interrupt level
 * when the IRQ line indicates an interrupt.
 */
void do_irq ( void )
{
	int nint;
	struct thread *tp;

	if ( ! cur_thread )
	    panic ( "irq int, cur_thread" );

#ifdef notdef
	/* XXX */
	printf ( "\n" );
	printf ("Interrupt debug, sp = %08x\n", get_sp() );

	show_thread_regs ( cur_thread );
	printf ( "\n" );
	show_stack ( get_sp () );
	spin ();
#endif

	/* Tell Kyu thread system we are in an interrupt */
	start_interrupt ();

	// nint = intcon_irqwho ();
	nint = gic_irqwho ();

	/* Always swamps output, but useful in
	 * dire debugging situations.
	printf ( "Interrupt %d\n", nint );
	printf ( "#" );
	*/

	if ( ! irq_table[nint].func ) {
	    /* Probably unrelated to current thread.
	     * This is pretty severe - XXX
	     */
	    printf ("Unknown interrupt request: %d\n", nint );
	    show_thread_regs ( cur_thread );
	    spin ();
	}

	/* call the user handler
	 */
	(irq_table[nint].func)( irq_table[nint].arg );

	// intcon_irqack ();
	gic_irqack ( nint );

	/* Tell Kyu thread system we are done with an interrupt */
	finish_interrupt ();

	panic ( "do_irq, resume" );
}

/* THE END */
