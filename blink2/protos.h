void printf ( const char *, ... );

void ms_delay ( int );
void delay_x ( void );

void enable_irq ( void );

void gic_init ( void );
void gic_check ( void );
void gic_show_status ( void );

void led_init ( void );
void led_on ( void );
void led_off ( void );
void led_toggle ( void );
void status_on ( void );
void status_off ( void );
void status_toggle ( void );

void serial_init ( void );
void serial_puts ( char * );
void serial_putc ( char );

void timer_init ( int, int );
void timer_handler ( int );
void led_handler ( void );

/* At one time I had a bad habit of using "long"
 * for 32 bit values.  This is fine with arm32,
 * but when you port code the ARM 64 it yields misery.
 */
typedef unsigned int u32;
typedef volatile unsigned int vu32;
