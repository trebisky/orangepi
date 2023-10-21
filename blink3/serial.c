/* serial.c
 * super simple "driver" for the H3 uart.
 *
 * Tom Trebisky  12-22-2016
 */

#include "protos.h"

#define UART0_BASE	0x01C28000
#define UART_BASE	((struct h3_uart *) UART0_BASE)

struct h3_uart {
	vu32 data;	/* 00 - Rx/Tx data */
	vu32 ier;	/* 04 - interrupt enables */
	vu32 iir;	/* 08 - interrupt ID / FIFO control */
	vu32 lcr;	/* 0c - line control */
	vu32 mcr;	/* 10 - modem control */
	vu32 lsr;	/* 14 - line status */
	vu32 msr;	/* 18 - modem status */
	vu32 scr;       /* 1c - scratch register */
        int _pad0[23];
        vu32 stat;      /* 7c - uart status */
        vu32 flvl;      /* 80 - fifo level */
        vu32 rfl;       /* 84 - RFL */
        int _pad1[7];
        vu32 halt;      /* A4 - Tx halt */

};

#define divisor_msb	ier
#define divisor_lsb	data

/* It looks like the basic clock is 24 Mhz
 * We need 16 clocks per character.
 */
#define BAUD_115200    (0xD) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */

/* bits in the lsr */
#define RX_READY	0x01
#define TX_READY	0x40
#define TX_EMPTY	0x80

/* bits in the ier */
#define IE_RDA		0x01	/* Rx data available */
#define IE_TXE		0x02	/* Tx register empty */
#define IE_LS		0x04	/* Line status */
#define IE_MS		0x08	/* Modem status */


#define LCR_DATA_5	0x00	/* 5 data bits */
#define LCR_DATA_6	0x01	/* 6 data bits */
#define LCR_DATA_7	0x02	/* 7 data bits */
#define LCR_DATA_8	0x03	/* 8 data bits */

#define LCR_STOP	0x04	/* extra (2) stop bits, else: 1 */
#define LCR_PEN		0x08	/* parity enable */

#define LCR_EVEN	0x10	/* even parity */
#define LCR_STICK	0x20	/* stick parity */
#define LCR_BREAK	0x40

#define LCR_DLAB	0x80	/* divisor latch access bit */

#define IIR_MODEM       0x0     /* modem status, clear by reading modem status */
#define IIR_NONE        0x1     /* why isn't "none" zero ? */
#define IIR_TXE         0x2     /* THR empty, clear by reading IIR or write to THR */
#define IIR_RDA         0x4     /* rcvd data available, clear by reading it. */
#define IIR_LINE        0x6     /* line status, clear by reading line status. */
#define IIR_BUSY        0x7     /* busy detect, clear by reading modem status. */

/* 8 bits, no parity, 1 stop bit */
#define LCR_SETUP	LCR_DATA_8

void uart_gpio_init ( void );
void uart_clock_init ( void );

void
serial_init ( void )
{
	struct h3_uart *up = UART_BASE;

	uart_gpio_init();
	uart_clock_init();

	up->ier = 0;
	up->lcr = LCR_DLAB;

	up->divisor_msb = 0;
	up->divisor_lsb = BAUD_115200;

	up->lcr = LCR_SETUP;

	// Why do this if we aren't using interrupts?
	// up->ier = IE_RDA | IE_TXE;
}

/* Called at interrupt level */
void
uart_handler ( void )
{
	struct h3_uart *up = UART_BASE;
	int iir;
	int stat;

	iir = up->iir & 0xf;

	/* On every startup, one of these is pending.
	 * The only way to clear it seems to be to
	 * do this -- handle the interrupt and then
	 * reading the status register clears it
	 */
	if ( iir == IIR_BUSY ) {
	    stat = up->stat;
	    printf ( "Uart BUSY interrupt\n" );
	}
}

void
serial_putc ( char c )
{
	struct h3_uart *up = UART_BASE;

	while ( !(up->lsr & TX_READY) )
	    ;
	up->data = c;
}

void
serial_puts ( char *s )
{
	while ( *s ) {
	    if (*s == '\n')
		serial_putc('\r');
	    serial_putc(*s++);
	}
}

void
serial_check ( int num )
{
	struct h3_uart *up = UART_BASE;

	printf ( " Uart: lsr/ier/iir %02x %02x %02x  %d\n", up->lsr, up->ier, up->iir, num );
}

int
serial_rx_status ( void )
{
	struct h3_uart *up = UART_BASE;

	return up->lsr & RX_READY;
}

int
serial_read ( void )
{
	struct h3_uart *up = UART_BASE;

	return up->data;
}

/* THE END */
