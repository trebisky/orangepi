/* gpio.c
 * Tom Trebisky  12-22-2016 7-17-2020
 */

#define CHIP_H5

void gpio_config ( int, int, int );
void gpio_pull ( int, int, int );

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

typedef volatile unsigned int vu32;
typedef unsigned int u32;

struct h3_gpio {
	vu32 config[4];
	vu32 data;
	vu32 drive[2];
	vu32 pull[2];
};

/* In theory each gpio has 32 pins, but they are actually populated like so.
 * So, there are no pins for B, H, and I !!
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

/* The config settings for each pin are in table 3-1 of the user manual */
/* Also in the GPIO section itself */
/* Most pins come up disabled */

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

void
uart_gpio_init ( void )
{
#ifdef CHIP_H3
	gpio_config ( GPIO_A, 4, H3_GPA_UART0 );
	gpio_config ( GPIO_A, 5, H3_GPA_UART0 );
	gpio_pull ( GPIO_A, 5, GPIO_PULL_UP );

#else	/* H5 */
	gpio_config ( GPIO_A, 4, H5_GPA_UART0 );
	gpio_config ( GPIO_A, 5, H5_GPA_UART0 );
	gpio_pull ( GPIO_A, 5, GPIO_PULL_UP );
#endif
}


/* A15 on PC, A20 on PC2 */
/* So this is correct and works with the H5 based PC2 */

#define STATUS_PIN	20
// #define STATUS_PIN	15

/* L10 is the same pin on PC and PC2 */
/* no luck so far making this blink on the PC2 */
/* I have tried this on two separate boards, no luck */
/* This light does come on briefly when linux boots */
#define PWR_PIN		10

void
led_init ( void )
{
	gpio_config ( GPIO_L, PWR_PIN, GPIO_OUTPUT );
	gpio_config ( GPIO_A, STATUS_PIN, GPIO_OUTPUT );
}

/* Configure all bits in a gpio as outputs
 * then toggle them.
 * This was tested on GPIO_A and works fine.
 */
void
hack_init ( int gpio )
{
	struct h3_gpio *gp = gpio_base[gpio];

	gp->config[0] = 0x11111111;
	gp->config[1] = 0x11111111;
}

void
hack_on ( int gpio )
{
	struct h3_gpio *gp = gpio_base[gpio];

	gp->data = 0xffffffff;
}

void
hack_off ( int gpio )
{
	struct h3_gpio *gp = gpio_base[gpio];

	gp->data = 0x0;
}

void
led_on ( void )
{
	gpio_output ( GPIO_L, PWR_PIN, 1 );
}

void
led_off ( void )
{
	gpio_output ( GPIO_L, PWR_PIN, 0 );
}

/* This is the red LED, it works fine */

void
status_on ( void )
{
	gpio_output ( GPIO_A, STATUS_PIN, 1 );
}

void
status_off ( void )
{
	gpio_output ( GPIO_A, STATUS_PIN, 0 );
}

/* A reasonable delay for blinking an LED */
void
delay_x ( void )
{
	volatile int count = 50000000;

	while ( count-- )
	    ;
}

/* For some reason, only the red LED is blinking
 * The red LED is the status LED.
 * The other LED (on Port L) does not respond.
 */
void
blink ( void )
{
	led_init ();
	hack_init ( GPIO_L );
	// hack_init ( GPIO_A );

	for ( ;; ) {
	    //led_off ();
	    hack_off ( GPIO_L );

	    // hack_off ( GPIO_A );
	    status_on ();
	    // uart_puts("OFF\n");

	    delay_x ();

	    // led_on ();
	    hack_on ( GPIO_L );

	    // hack_on ( GPIO_A );
	    status_off ();
	    // uart_puts("ON\n");

	    delay_x ();
	}

}
/* THE END */
