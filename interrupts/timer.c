/* This is a driver for the H3 Allwinner timer section
 *
 * Tom Trebisky  12-30-2016
 */

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
void
timer_init ( void )
{
	struct h3_timer *hp = TIMER_BASE;

	// hp->t0_ival = 0x00100000;
	hp->t0_ival = 0x80000000;

	hp->t0_ctrl = CTL_SRC_24M;
	hp->t0_ctrl |= CTL_RELOAD;
	while ( hp->t0_ctrl & CTL_RELOAD )
	    ;
	hp->t0_ctrl |= CTL_ENABLE;

	printf ("  Timer I val: %08x\n", hp->t0_ival );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
	printf ("  Timer C val: %08x\n", hp->t0_cval );
}

/* All indications are that this is a 32 bit counter running
 *  -- at 30,384 Hz when we ask for 32K
 *  -- at 23897172 when we ask for 24M
 *  (note that this is off by .0045 percent, but in
 *   actual fact we are using the same crystal to run
 *   the CPU so this is meaningless.
 */
void
timer_watch ( void )
{
	struct h3_timer *hp = TIMER_BASE;
	int i;
	int val;
	int last;
	int del;

	val = hp->t0_cval;

	for ( i=0; i<10; i++ ) {
	    last = val;
	    val = hp->t0_cval;
	    del = last - val;
	    printf ("  Timer: 0x%08x %d = %d\n", val, val, del );
	    ms_delay ( 1000 );
	}
}
/* THE END */
