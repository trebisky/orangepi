/* This is a collection of experiments pertaining to
 * delays on the Orange Pi PC.
 * Also experimenting with inline assembly,
 * as well as starting to fiddle with the timer.
 *
 * To fully utilize the timer will require getting
 *  interrupts going, which means attacking the ARM GIC device,
 *  which is no small task.
 *
 * Tom Trebisky  12-30-2016
 */

void main ( void );
void printf ( char *, ... );

/* Since we don't have an assembly language startup or a clever LDS script,
 * we rely on this being at the start of the linked code and being
 * the first thing executed.  Note that we simply use the stack handed
 * to us by U-Boot.
 */
void
start_me ( void )
{
	main ();
}

/* Something in the eabi library for gcc wants this */
int
raise (int signum)
{
	return 0;
}

/* --------------------------------------- */

void led_init ( void );
void led_on ( void );
void led_off ( void );
void status_on ( void );
void status_off ( void );

void uart_init ( void );
void uart_puts ( char * );

#define MS_300	50000000;

/* A reasonable delay for blinking an LED.
 * This began as a wild guess, but
 * in fact yields a 300ms delay
 * as calibrated below.
 */
void
delay_x ( void )
{
	volatile int count = MS_300;

	while ( count-- )
	    ;
}

#define MS_1	166667

void
delay_ms ( int ms )
{
	volatile int count = ms * MS_1;

	while ( count-- )
	    ;
}

void
blink ( void )
{
	led_init ();

	for ( ;; ) {
	    led_off ();
	    status_on ();
	    // uart_puts("OFF\n");
	    delay_x ();
	    led_on ();
	    status_off ();
	    // uart_puts("ON\n");
	    delay_x ();
	}

}

#define ROM_START       ((unsigned long *) 0x01f01da4)

void
show_stuff ( void )
{
	unsigned long id;
	unsigned long sp;

	id = 1<<31;
	printf ( "test: %08x\n", id );
	id = 0xdeadbeef;
	printf ( "test: %08x\n", id );

        asm volatile ("add %0, sp, #0" : "=r" (sp));
	printf ( "sp: %08x\n", sp );

        asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (id));

	printf ( "ARM id register: %08x\n", id );

	id = *ROM_START;
	printf ( "rom before: %08x\n", id );

	*ROM_START = (unsigned long) blink;

	id = *ROM_START;
	printf ( "rom after: %08x\n", id );
}

#define PMCR_ENABLE     0x01    /* enable all counters */
#define PMCR_EV_RESET   0x02    /* reset all event counters */
#define PMCR_CC_RESET   0x04    /* reset CCNT */
#define PMCR_CC_DIV     0x08    /* divide CCNT by 64 */
#define PMCR_EXPORT     0x10    /* export events */
#define PMCR_CC_DISABLE 0x20    /* disable CCNT in non-invasive regions */

/* There are 4 counters besides the CCNT (we ignore them at present) */
#define CENA_CCNT       0x80000000
#define CENA_CTR3       0x00000008
#define CENA_CTR2       0x00000004
#define CENA_CTR1       0x00000002
#define CENA_CTR0       0x00000001

void
ccnt_enable ( int div64 )
{
        int val;

        // val = get_pmcr ();
	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(val) );
	printf ( " PMCR = %08x\n", val );
        val |= PMCR_ENABLE;
        if ( div64 )
            val |= PMCR_CC_DIV;
        // set_pmcr ( val );
	asm volatile ("mcr p15, 0, %0, c9, c12, 0" : : "r"(val) );

	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(val) );
	printf ( " PMCR = %08x\n", val );

        // set_cena ( CENA_CCNT );
	val = CENA_CCNT;
	asm volatile ("mcr p15, 0, %0, c9, c12, 1" : : "r"(val) );

	asm volatile ("mrc p15, 0, %0, c9, c12, 1" : "=r"(val) );
	printf ( " CENA = %08x\n", val );
}


void
ccnt_reset ( void )
{
	int val;

        // set_pmcr ( get_pmcr() | PMCR_CC_RESET );
	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(val) );
	val |= PMCR_CC_RESET;
	asm volatile ("mcr p15, 0, %0, c9, c12, 0" : : "r"(val) );
}

static inline int 
ccnt_read ( void )
{
	int count;

	asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count) );
	return count;
}

#define GIG	1000000000
#define MEG	1000000

void
gig_delay ( void )
{
	int count;

	ccnt_reset ();

	for ( ;; ) {
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count) );
	    if ( count > GIG )
		break;
	}
}

/* OK for up to 16 seconds at 1 Ghz clock */
void
ms_delay ( int ms )
{
	int target = ms * MEG;
	int count;

	ccnt_reset ();

	for ( ;; ) {
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count) );
	    if ( count >= target )
		break;
	}
}

static inline void
spin ( void )
{
	for ( ;; )
	    ;
}

/* --------------------------------------- */

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

void
main ( void )
{
	int count1;
	int count2;
	int tick = 0;
	int val;
	int msecs;

	uart_init();

	uart_puts("\n" );
	// uart_puts("Eat more fish!\n");
	printf ("Eat more fish!\n");

	status_on ();

	/* Read SCR (secure config register) */
	asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r"(val) );
	printf ( " SCR = %08x\n", val );

	ccnt_enable ( 0 );
	ccnt_reset ();

	show_stuff ();

	// launch ( blink );
	// launch_core ( 1, blink );
	// blink ();

#ifdef notdef
	/* Stopwatch shows that this yields exactly a 10 second delay,
	   so the processor (as handed to use by U-Boot) is running
	   at 1.0 Ghz
	 */
	printf ( "Start\n" );
	for ( val=0; val<10; val++ )
	    gig_delay ();
	printf ( " ...... DONE\n" );
#endif

	timer_init ();
	timer_watch ();
	spin ();

#ifdef notdef
	/* The game here is to calibrate my silly delay_x() function.
	 * It turns out it gives a 300 ms delay.
	 */
	for ( ;; ) {
	    ccnt_reset ();
	    delay_x ();
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count1) );
	    printf ( " Count: (%d) %d %08x\n", tick++, count1, count1 );
	    msecs = count1 / MEG;
	    printf ( " Milliseconds: %d\n", msecs );
	}
#endif

	spin ();

	for ( ;; ) {
	    delay_x ();
	    ccnt_reset ();
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count1) );
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count2) );
	    printf ( " Count: (%d) %d %08x\n", tick++, count1, count1 );
	    printf ( " Count+ (%d) %d %08x\n", tick++, count2, count2 );
	}

	for ( ;; ) {
	    // ccnt_reset ();
	    delay_x ();
	    delay_x ();
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count1) );
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count2) );
	    printf ( " Count: (%d) %d %08x\n", tick++, count1, count1 );
	    printf ( " Count+ (%d) %d %08x\n", tick++, count2, count2 );
	}

	uart_puts(" .. Spinning\n");

	/* spin */
	for ( ;; )
	    ;

}

/* THE END */
