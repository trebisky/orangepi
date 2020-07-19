/* uart.c
 * Tom Trebisky  12-22-2016 7-17-2020
 */

#include "protos.h"

typedef volatile unsigned int vu32;
typedef unsigned int u32;

/* These are registers in the CCM (clock control module)
 */
#define CCM_GATE	((u32 *) 0x01c2006c)
#define CCM_RESET4	((u32 *) 0x01c202d8)

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
	int _pad;			/* 10 */
	volatile unsigned int lsr;	/* 14 */
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

/* THE END */
