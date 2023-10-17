/* The game here is to get interrupts happening
 *   on the Orange Pi PC.
 *
 * This is mostly about writing a driver for the ARM GIC device.
 * The interrupt source in question is TIMER 0, which
 * I have running to interrupt at 10 Hz.
 *
 * There is some cruft hanging around from the original
 *  timer experiments.
 *
 * Tom Trebisky  1-4-2017
 */

#include "protos.h"

void main ( void );

/* Something in the eabi library for gcc wants this */
int
raise (int signum)
{
	return 0;
}

/* --------------------------------------- */

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
	// printf ( " PMCR = %08x\n", val );
        val |= PMCR_ENABLE;
        if ( div64 )
            val |= PMCR_CC_DIV;
        // set_pmcr ( val );
	asm volatile ("mcr p15, 0, %0, c9, c12, 0" : : "r"(val) );

	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(val) );
	// printf ( " PMCR = %08x\n", val );

        // set_cena ( CENA_CCNT );
	val = CENA_CCNT;
	asm volatile ("mcr p15, 0, %0, c9, c12, 1" : : "r"(val) );

	asm volatile ("mrc p15, 0, %0, c9, c12, 1" : "=r"(val) );
	// printf ( " CENA = %08x\n", val );
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
gig_delayX ( void )
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
gig_delay ( int secs )
{
	int count;
	int limit = secs * GIG;

	ccnt_reset ();

	for ( ;; ) {
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count) );
	    if ( count > limit )
		break;
	}
}

void
ms_delay ( int ms )
{
	int count;
	int limit = ms * MEG;

	ccnt_reset ();

	for ( ;; ) {
	    asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r"(count) );
	    if ( count > limit )
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

void timer_init ( int );
void timer_watch ( void );
void gic_watch ( void );

void gic_init ( void );

extern volatile int timer_count;

void
main ( void )
{
	int count1;
	int count2;
	int tick = 0;
	int val;
	int msecs;
	int i;
	int last_count;
	int cur_sp;

	uart_init();

	uart_puts("\n" );
	// uart_puts("Eat more fish!\n");
	printf ("Interrupt EZ demo starting 10-2023\n");

	/* A printf inside the interrupt shows:
	 * GIC: sp = 57FFFFC0
	 * This shows:
	 * SP = 4FFFFFF0
	 */

	// asm volatile ("add %0, sp, #0" : "=r"(cur_sp) );
	// printf ( "SP = %08x\n", cur_sp );

	led_init ();
	// status_on ();
	status_off ();
	// led_off ();

	ccnt_enable ( 0 );
	ccnt_reset ();

#ifdef notdef
	/* Read SCR (secure config register) */
	asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r"(val) );
	printf ( " SCR = %08x\n", val );

	printf ( " CCNT = %08x\n", ccnt_read() );
	printf ( " CCNT = %08x\n", ccnt_read() );
	printf ( " CCNT = %08x\n", ccnt_read() );

	show_stuff ();

	// launch ( blink );
	// launch_core ( 1, blink );
	// blink ();
#endif

	gic_init ();

	timer_init ( 10 );
	// timer_init ( 20 );
	// timer_one ( 2000 );

	timer_count = 0;
	last_count = timer_count;

	printf ( "Enabling IRQ\n" );
	ms_delay ( 100 );

	enable_irq ();

	// timer_watch ();
	// gic_watch ();

	/* I was printing the stack to check for corruption,
	 * but never saw anything out of the ordinary
	 */
	for ( ;; ) {
	    ms_delay ( 500 );
	    // asm volatile ("add %0, sp, #0" : "=r"(cur_sp) );
	    // printf ( "Count: %5d sp = %08x\n", timer_count, cur_sp );
	    printf ( "Interrupt count: %5d\n", timer_count );
	}

#ifdef notdef
	for ( ;; ) {
	    ms_delay ( 500 );
	    printf ( "Count: %5d\n", timer_count );
	    /*
	    if ( timer_count > last_count ) {
		printf ( "Count: %5d\n", timer_count );
	 	last_count = timer_count;
	    }
	    */
	    /*
	    if ( uart_rx_status() ) {
		val = uart_read ();
		printf ( "Got: %02x\n", val );
	    }
	    */
	}
#endif

	for ( i=0; ; i++ ) {
	    ms_delay ( 500 );
	    gic_check ();
	    // if ( timer_count > last_count ) {
	// 	last_count = timer_count;
	// 	timer_one ( 1200 );
	  //   }
	    /*
	    uart_check ( i );
	    if ( uart_rx_status() ) {
		val = uart_read ();
		printf ( "Got: %02x\n", val );
	    }
	    */
	}

	/*
	uart_puts(" .. Spinning\n");
	spin ();
	*/

#ifdef notdef
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

#endif
}

/* THE END */
