int printf ( const char *, ... );

void ms_delay ( int );
void delay_x ( void );
void blink ( void );

void enable_irq ( void );
void gic_check ( void );

void led_init ( void );
void led_on ( void );
void led_off ( void );
void led_toggle ( void );
void status_on ( void );
void status_off ( void );
void status_toggle ( void );

void serial_init ( int );
void serial_puts ( char * );
void serial_putc ( char );

int ccnt_read ( void );

// void timer_handler ( void );

typedef unsigned int u32;
typedef volatile unsigned int vu32;

/* THE END */
