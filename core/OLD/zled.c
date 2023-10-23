
void
led_init ( void )
{
	gpio_config ( GPIO_L, 10, GPIO_OUTPUT );
	gpio_config ( GPIO_A, 15, GPIO_OUTPUT );
}

/* Green LED */

void
led_on ( void )
{
	gpio_output ( GPIO_L, 10, 1 );
}

void
led_off ( void )
{
	gpio_output ( GPIO_L, 10, 0 );
}

static int l_status = 0;

void
led_toggle ( void )
{
	if ( l_status ) {
	    l_status = 0;
	    led_off ();
	} else {
	    l_status = 1;
	    led_on ();
	}
}

/* Red LED */

void
status_on ( void )
{
	gpio_output ( GPIO_A, 15, 1 );
}

void
status_off ( void )
{
	gpio_output ( GPIO_A, 15, 0 );
}

static int s_status = 0;

void
status_toggle ( void )
{
	if ( s_status ) {
	    s_status = 0;
	    status_off ();
	} else {
	    s_status = 1;
	    status_on ();
	}
}

/* ======================================================================== */

/* --------------------------------------- */

#define MS_300  50000000;

/* A reasonable delay for blinking an LED.
 * This began as a wild guess, but
 * in fact yields a 300ms delay
 * as calibrated below.
 */
static void
delay_x ( void )
{
        volatile int count = MS_300;

        while ( count-- )
            ;
}

#define MS_1    166667

static void
delay_ms ( int ms )
{
        volatile int count = ms * MS_1;

        while ( count-- )
            ;
}

/* Blink red and green in alternation */
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

