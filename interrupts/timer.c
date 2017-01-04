/* This is a driver for the H3 Allwinner timer section
 *
 * Tom Trebisky  12-30-2016
 */

void printf ( char *, ... );

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

#define IRQ_T0			0x01
#define IRQ_T1			0x02

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

void
timer_init ( int hz )
{
	struct h3_timer *hp = TIMER_BASE;

	// hp->t0_ival = 0x00100000;
	// hp->t0_ival = 0x80000000;
	hp->t0_ival = CLOCK_24M / hz;

	hp->t0_ctrl = CTL_SRC_24M;
	hp->t0_ctrl |= CTL_RELOAD;
	while ( hp->t0_ctrl & CTL_RELOAD )
	    ;
	hp->t0_ctrl |= CTL_ENABLE;

	hp->irq_ena = IRQ_T0;

	/*
	printf ("  Timer I val: %08x\n", hp->t0_ival );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
	*/
}

void
timer_ack ( void )
{
	struct h3_timer *hp = TIMER_BASE;

	hp->irq_status = IRQ_T0;
}

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

/* THE END */
