/* The purpose of thie "bare metal" program is to figure out
 * how to get the H5 generating interrupts.
 * This began as h5_hello on 7-17-2020
 * This can write to the UART and also blink the LED.
 * Tom Trebisky  12-22-2016 7-17-2020
 *
 * Here is a play by play as experimenting procedes.
 *
 * 0 - the "power" LED does not respond or blink.
 *  This is not the focus of this exercise, but sorting
 *  this out may produce useful insights.
 *
 * --- As a first experiment, I tried configuring and then
 *  toggling ALL the bits in GPIO_L -- with no results.
 *  This was to check if the LED was miswired or misdocumented.
 * --- So I am busy looking over datasheets with the theory
 *  that the "R_GPIO" which along with a bunch of other "R_"
 *  peripherals does not have its clock gated on or some such.
 *  This is probably true, but I have yet to find a link between
 *  that and the interrupt problem.  The R_GPIO is at a base
 *  address of 0x01f02c00, while all the other GPIO devices
 *  have base addresses of 0x1c20xxxx.  The timer I use is also
 *  at 0x1c200c00, in the same section, which the block diagram
 *  shows as on the APB1 bus (The R gpio is on the APBS bus).
 *  It could be instructive to play with the WDOG, which is also
 *  in the "R_" section.
 *  There is an R_TIMER at 0x01f00800, but I have never tried to
 *  do anything with it.
 * --- What I am tempted to do is to look at U-Boot sources.
 *  I have no trouble accessing R_GPIO on the H3 boards, so maybe
 *  I can spot the difference in initialization.
 *
 * ----------------------------------------------------------------
 * ----------------------------------------------------------------
 *
 * Now for troubleshooting interrupts ...
 *
 * 1 - added vectors and tested that the table was active
 *  by generating an exception, which was fairly easy
 *  to get to work.
 * 2 - the next question is the GIC.  Do we use the same GIC
 *  as for the H3, or the v2.0 GIC (GIC400) that we have
 *  working for the armv8 Fire3 board.  It would seem that
 *  we wan the GIC400.  The other question is whether we
 *  have to power on this thing and activate the clock for it,
 *  as we do for so many other peripherals.  Note that we
 *  inherit this setup from U-Boot for the uart and the
 *  GPIO section that drives the LED.
 * 3 - probably the next thing to try is to software interrupt
 *  capability of the GIC itself.
 */

#include "protos.h"

void int_init ( void );
void mk_fault ( void );

#define INT_unlock      asm volatile("msr DAIFClr, #3" : : : "cc")
#define INT_lock        asm volatile("msr DAIFSet, #3" : : : "cc")

void
main ( void )
{
	int_init ();
	uart_init();

	uart_puts("\n" );
	uart_puts("Starting interrupt test!\n");

	INT_unlock;

	uart_puts("\n" );

	// mk_fault ();

	uart_puts(" .. Blinking\n");
	blink ();

	uart_puts(" .. Spinning\n");

	/* spin */
	for ( ;; )
	    ;

}

/* On the Orange Pi, we can apparently read any address
 * without a fault, including zero.  But if the unaligned
 * fault bit is set, we get a fault reading at an address
 * like "1", which we use to verify that our vector table
 * is indeed the one in effect.
 */
void
mk_fault ( void )
{
	long *p;
	long test;

#ifdef notdef
	/* This works fine */
	uart_puts ( "zero\n" );
	p = (long *) 0x0;
	test = *p;
#endif

	uart_puts ( "one\n" );
	p = (long *) 0x1;
	test = *p;

#ifdef notdef
	uart_puts ( "two\n" );
	p = (long *) 0x2;
	test = *p;

	uart_puts ( "three\n" );
	p = (long *) 0x3;
	test = *p;
#endif
}

// ------------------------------------
// interrupt stuff follows

#define NUM_IREGS       34

typedef	unsigned long			reg_t;


struct int_regs {
        reg_t regs[NUM_IREGS];
};

/* We really only need one of these, but we provide
 * some elbow room in case weird things happen.
 */
static struct int_regs bogus_thread[8];

void *cur_thread;

// void do_irq ( void )
void irq_handler ( void )
{
	uart_puts ( "IRQ !\n" );
}

void
fault_handler ( unsigned exc )
{
	uart_puts ( "Fault !\n" );
	// printf ( "Fault ! (%d)\n", exc );
	uart_puts ( "spinning after fault\n" );
	for ( ;; )
	    ;
}

void
int_init ( void )
{
	cur_thread = bogus_thread;
}

/* THE END */
