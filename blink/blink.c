/* Simple program to spit out something on the Orange Pi PC console uart
 * Also blinks the LED !!
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

/* ---------------------- */

#define GPIO_A    0
#define GPIO_B    1
#define GPIO_C    2
#define GPIO_D    3
#define GPIO_E    4
#define GPIO_F    5
#define GPIO_G    6
#define GPIO_H    7
#define GPIO_I    8
#define GPIO_L    9	/* R_PIO */

struct h3_gpio {
	volatile unsigned long config[4];
	volatile unsigned long data;
	volatile unsigned long drive[2];
	volatile unsigned long pull[2];
};

/* In theory each gpio has 32 pins, but they are actually populated like so.
 */
// static int gpio_count[] = { 22, 0, 19, 18, 16, 7, 14, 0, 0, 12 };

static struct h3_gpio * gpio_base[] = {
    (struct h3_gpio *) 0x01C20800,		/* GPIO_A */
    (struct h3_gpio *) 0x01C20824,		/* GPIO_B */
    (struct h3_gpio *) 0x01C20848,		/* GPIO_C */
    (struct h3_gpio *) 0x01C2086C,		/* GPIO_D */
    (struct h3_gpio *) 0x01C20890,		/* GPIO_E */
    (struct h3_gpio *) 0x01C208B4,		/* GPIO_F */
    (struct h3_gpio *) 0x01C208D8,		/* GPIO_G */
    (struct h3_gpio *) 0x01C208FC,		/* GPIO_H */
    (struct h3_gpio *) 0x01C20920,		/* GPIO_I */

    (struct h3_gpio *) 0x01F02c00,		/* GPIO_L */
};

/* Only A, G, and R can interrupt */

/* GPIO pin function config (0-7) */
#define GPIO_INPUT        (0)
#define GPIO_OUTPUT       (1)
#define H3_GPA_UART0      (2)
#define H5_GPA_UART0      (2)
#define GPIO_DISABLE      (7)

/* GPIO pin pull-up/down config (0-3)*/
#define GPIO_PULL_DISABLE	(0)
#define GPIO_PULL_UP		(1)
#define GPIO_PULL_DOWN		(2)
#define GPIO_PULL_RESERVED	(3)

/* There are 4 config registers,
 * each with 8 fields of 4 bits.
 */
void
gpio_config ( int gpio, int pin, int val )
{
	struct h3_gpio *gp = gpio_base[gpio];
	int reg = pin / 8;
	int shift = (pin & 0x7) * 4;
	int tmp;

	tmp = gp->config[reg] & ~(0xf << shift);
	gp->config[reg] = tmp | (val << shift);
}

/* There are two pull registers,
 * each with 16 fields of 2 bits.
 */
void
gpio_pull ( int gpio, int pin, int val )
{
	struct h3_gpio *gp = gpio_base[gpio];
	int reg = pin / 16;
	int shift = (pin & 0xf) * 2;
	int tmp;

	tmp = gp->pull[reg] & ~(0x3 << shift);
	gp->pull[reg] = tmp | (val << shift);
}

void
gpio_output ( int gpio, int pin, int val )
{
	struct h3_gpio *gp = gpio_base[gpio];

	if ( val )
	    gp->data |= 1 << pin;
	else
	    gp->data &= ~(1 << pin);
}

int
gpio_input ( int gpio, int pin )
{
	struct h3_gpio *gp = gpio_base[gpio];

	return (gp->data >> pin) & 1;
}

/* These are registers in the CCM (clock control module)
 */
#define CCM_GATE	((unsigned long *) 0x01c2006c)
#define CCM_RESET4	((unsigned long *) 0x01c202d8)

#define GATE_UART0	0x0001000
#define GATE_UART1	0x0002000
#define GATE_UART2	0x0004000
#define GATE_UART3	0x0008000

#define RESET4_UART0	0x0001000
#define RESET4_UART1	0x0002000
#define RESET4_UART2	0x0004000
#define RESET4_UART3	0x0008000

/* This is probably set up for us by U-boot,
 * true bare metal would need this.
 */
void
uart_clock_init ( void )
{
	*CCM_GATE |= GATE_UART0;
	*CCM_RESET4 |= RESET4_UART0;
}

#define CHIP_H3

void
uart_gpio_init ( void )
{
#ifdef CHIP_H3
	gpio_config ( GPIO_A, 4, H3_GPA_UART0 );
	gpio_config ( GPIO_A, 5, H3_GPA_UART0 );
	gpio_pull ( GPIO_A, 5, GPIO_PULL_UP );

#else	/* H5 */
	gpio_config ( GPIO_A, 4, SUN50I_H5_GPA_UART0 );
	gpio_config ( GPIO_A, 5, SUN50I_H5_GPA_UART0 );
	gpio_pull ( GPIO_A, 5, GPIO_PULL_UP );
#endif
}

void
led_init ( void )
{
	gpio_config ( GPIO_L, 10, GPIO_OUTPUT );
	gpio_config ( GPIO_A, 15, GPIO_OUTPUT );
}

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

#define BAUD_115200    (0xD) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */

#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1       (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

struct h3_uart {
	volatile unsigned int data;	/* 00 */
	volatile unsigned int ier;	/* 04 */
	volatile unsigned int iir;	/* 08 */
	volatile unsigned int lcr;	/* 0c */
	int _pad;
	volatile unsigned int lsr;	/* 04 */
};

#define dlh	ier
#define dll	data

#define UART0_BASE	0x01C28000
#define UART_BASE	((struct h3_uart *) UART0_BASE)

#define TX_READY	0x40

void
uart_init ( void )
{
	struct h3_uart *up = UART_BASE;

	uart_gpio_init();
	uart_clock_init();

	up->lcr = 0x80;		/* select dll dlh */

	up->dlh = 0;
	up->dll = BAUD_115200;

	up->lcr = LC_8_N_1;
}

void
uart_putc ( char c )
{
	struct h3_uart *up = UART_BASE;

	while ( !(up->lsr & TX_READY) )
	    ;
	up->data = c;
}

void
uart_puts ( char *s )
{
	while ( *s ) {
	    if (*s == '\n')
		uart_putc('\r');
	    uart_putc(*s++);
	}
}

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
	uart_puts("Eat more fish!\n");

	blink ();

	/* NOTREACHED */

	uart_puts(" .. Spinning\n");

	/* spin */
	for ( ;; )
	    ;

}

/* THE END */
