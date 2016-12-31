/* This is a collection of experiments pertaining to
 * delays on the Orange Pi PC
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

/* A reasonable delay for blinking an LED.
 * This began as a wild guess, but
 * in fact yields a 300ms delay
 * as calibrated below.
 */
void
delay_x ( void )
{
	volatile int count = 50000000;

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

static inline void
spin ( void )
{
	for ( ;; )
	    ;
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

	for ( ;; ) {
	    ccnt_reset ();
	    delay_x ();
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count1) );
	    printf ( " Count: (%d) %d %08x\n", tick++, count1, count1 );
	    msecs = count1 / MEG;
	    printf ( " Milliseconds: %d\n", msecs );
	}

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
