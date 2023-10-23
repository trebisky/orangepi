/* Driver for the LED on the Orange Pi board
 *
 * Tom Trebisky  10-22-2023
 */

#include "protos.h"

/* Green LED */

static int g_status = 0;

void
green_toggle ( void )
{
	if ( g_status ) {
	    g_status = 0;
	    green_off ();
	} else {
	    g_status = 1;
	    green_on ();
	}
}

/* Red LED */

static int r_status = 0;

void
red_toggle ( void )
{
	if ( r_status ) {
	    r_status = 0;
	    red_off ();
	} else {
	    r_status = 1;
	    red_on ();
	}
}

/* This is the "heart" of the blink3 demo.
 * The idea is to let timer 0 run continuously,
 * and it is it's job to start each blink.
 * timer 1 determines the duration of the blink
 * turning it off when done.
 * We blink the two LED in alternation.
 *
 * 10 ms is visible, but rather faint.
 * 50 ms is just fine
 */

// #define DURATION	200
// #define DURATION	50
#define DURATION	100

static int led_state = 0;

void
blink_setup ( void )
{
	// printf ( "LED setup called\n" );
	red_on ();
	green_off ();
	led_state = 0;

	/* 2 Hz */
	timer_repeat ( 0, 2 );
}

/* This version ignores argument,
 * expects only a periodic interrupt
 * from timer 0
 */
void
led_handler ( int who )
{
	if ( led_state == 0 ) {
	    led_state = 1;
	    red_off ();
	    green_on ();
	} else {
	    led_state = 0;
	    red_on ();
	    green_off ();
	}
}

#ifdef notdef
/* Called at interrupt level to blink both LEDs */
void
led_handler ( int who )
{
	// printf ( "Ding: %d\n", who );

	if ( who == 1 ) {
	    if ( led_state == 0 )
		led_off ();
	    else
		status_off ();
	    return;
	}

	/* who == 0 */
	if ( led_state == 0 ) {
	    led_state = 1;
	    status_on ();
	} else {
	    led_state = 0;
	    led_on ();
	}

	timer_one ( 1, DURATION );
}
#endif

/* THE END */
