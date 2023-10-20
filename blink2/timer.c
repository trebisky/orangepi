/* This is a driver for the H3 Allwinner timer section
 *
 * Tom Trebisky  12-30-2016
 */

#include "protos.h"

int timer_count;

#ifdef notdef
struct h3_timer {
	volatile unsigned int irq_ena;		/* 00 */
	volatile unsigned int irq_status;	/* 04 */
	int __pad1[2];
	volatile unsigned int t0_ctrl;		/* 10 */
	volatile unsigned int t0_ival;		/* 14 */
	volatile unsigned int t0_cval;		/* 18 */
	int __pad2;
	volatile unsigned int t1_ctrl;		/* 20 */
	volatile unsigned int t1_ival;		/* 24 */
	volatile unsigned int t1_cval;		/* 28 */
};
#endif

struct h3_timer {
	vu32 irq_ena;		/* 00 */
	vu32 irq_status;	/* 04 */
	int __pad1[2];

	struct tregs {
	    vu32 ctrl;		/* 10, 20 */
	    vu32 ival;		/* 14, 24 */
	    vu32 cval;		/* 18, 28 */
	    int __pad;
	} regs[2];
};

/* There are more registers for AVS and Watchdog */

#define TIMER_BASE	( (struct h3_timer *) 0x01c20c00)

#define	CTL_ENABLE		0x01
#define	CTL_RELOAD		0x02		/* reload ival */
#define	CTL_SRC_32K		0x00
#define	CTL_SRC_24M		0x04

#define	CTL_PRE_1		0x00
#define	CTL_PRE_2		0x10
#define	CTL_PRE_4		0x20
#define	CTL_PRE_8		0x30
#define	CTL_PRE_16		0x40
#define	CTL_PRE_32		0x50
#define	CTL_PRE_64		0x60
#define	CTL_PRE_128		0x70

#define	CTL_SINGLE		0x80
#define	CTL_AUTO		0x00

#define ENA_T0			0x01
#define ENA_T1			0x02

/* The timer is a down counter,
 *  intended to generate periodic interrupts
 * There are two of these.
 * There is also a watchdog and
 * an "AVS" timer (Audio/Video Synchronization)
 *  neither of which are supported here.
 *
 * The datasheet says they are 24 bit counters, but
 *  experiment clearly shows T0 and T1 are 32 bit,
 *  so the 24 bit claim perhaps/probably applies
 *  to the watchdog, who can say?
 *  Foggy documentation at best.
 * 24 bits can hold values up to 16,777,215
 */

/* All indications are that this is a 32 bit counter running
 *  -- at 30,384 Hz when we ask for 32K
 *  -- at 23897172 when we ask for 24M
 *  (note that this is off by .0045 percent, but in
 *   actual fact we are using the same crystal to run
 *   the CPU so this is meaningless.
 */

#define CLOCK_24M	24000000
#define CLOCK_24M_MS	24000

void
timer_init ( int who, int hz )
{
	struct h3_timer *hp = TIMER_BASE;
	struct tregs *tp;

	tp = &hp->regs[who];

	tp->ctrl = 0;	/* stop the timer */

	if ( who == 0 ) {
	    hp->irq_ena = ENA_T0;
	} else {
	    hp->irq_ena = ENA_T1;
	}

	// hp->t0_ival = 0x00100000;
	// hp->t0_ival = 0x80000000;
	tp->ival = CLOCK_24M / hz;

	tp->ctrl = CTL_SRC_24M;
	tp->ctrl |= CTL_RELOAD;

	while ( tp->ctrl & CTL_RELOAD )
	    ;

	tp->ctrl |= CTL_ENABLE;

	/*
	printf ("  Timer I val: %08x\n", hp->t0_ival );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
	*/
}

#ifdef notdef
/* One shot, delay in milliseconds */
void
timer_one ( int delay )
{
	struct h3_timer *hp = TIMER_BASE;

	hp->t0_ival = CLOCK_24M_MS * delay;

	hp->t0_ctrl = 0;	/* stop the timer */
	hp->irq_ena = ENA_T0;

	hp->t0_ctrl = CTL_SRC_24M | CTL_SINGLE;
	hp->t0_ctrl |= CTL_RELOAD;
	while ( hp->t0_ctrl & CTL_RELOAD )
	    ;
	hp->t0_ctrl |= CTL_ENABLE;
}
#endif

void
timer_ack ( int who )
{
	struct h3_timer *hp = TIMER_BASE;

	if ( who == 0 )
	    hp->irq_status = ENA_T0;
	else
	    hp->irq_status = ENA_T1;
}

static int first = 1;

/* Called at interrupt level
 * "who" is 0 or 1
 */
void
timer_handler ( int who )
{
	if ( who == 0 ) {
	    timer_count++;
	    timer_ack ( who );

	    led_handler ();
	    return;
	}

	if ( first )
	    printf ( "Timer 1 interrupt\n" );
	first = 0;

	timer_ack ( who );
	led_handler ();
}

#ifdef notdef
void
timer_watch ( void )
{
	struct h3_timer *hp = TIMER_BASE;
	int val;
	int last;
	int del;

	val = hp->t0_cval;

	for ( ;; ) {
	    last = val;
	    val = hp->t0_cval;
	    /*
	    del = last - val;
	    printf ("  Timer: 0x%08x %d = %d\n", val, val, del );
	    */
	    printf ("  Timer: 0x%08x %d = %08x\n", val, val, hp->irq_status );
	    /*
	    if ( hp->irq_status )
		timer_ack ();
	    printf (" =Timer: 0x%08x %d = %08x\n", val, val, hp->irq_status );
	    */
	    // gig_delay ( 2 );
	    ms_delay ( 2000 );
	}
}
#endif

/* THE END */
