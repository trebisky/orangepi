/* Simple program to print things on the Orange Pi PC console uart
 *
 * Tom Trebisky  12-22-2016
 */

void main ( void );

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

/* A reasonable delay for blinking an LED */
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

void
main ( void )
{
	uart_init();

	uart_puts("\n" );
	// uart_puts("Eat more fish!\n");
	printf ("Eat more fish!\n");


	// launch ( blink );
	// launch_core ( 1, blink );
	blink ();

	uart_puts(" .. Spinning\n");

	/* spin */
	for ( ;; )
	    ;

}

/* THE END */
