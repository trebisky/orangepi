void printf ( char *, ... );

void ms_delay ( int );
void delay_x ( void );

void enable_irq ( void );
void gic_check ( void );

void led_init ( void );
void led_on ( void );
void led_off ( void );
void led_toggle ( void );
void status_on ( void );
void status_off ( void );
void status_toggle ( void );

void uart_init ( void );
void uart_puts ( char * );
void uart_putc ( char );

void timer_handler ( void );
