/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * serial.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  12-22-2016
 * Tom Trebisky  1/19/2017
 *
 * serial.c
 * super simple "driver" for the H3 uart.
 */

#define UART0_BASE	0x01C28000
#define UART_BASE	((struct h3_uart *) UART0_BASE)

struct h3_uart {
	volatile unsigned int data;	/* 00 - Rx/Tx data */
	volatile unsigned int ier;	/* 04 - interrupt enables */
	volatile unsigned int iir;	/* 08 - interrupt ID / FIFO control */
	volatile unsigned int lcr;	/* 0c - line control */
	volatile unsigned int mcr;	/* 10 - modem control */
	volatile unsigned int lsr;	/* 14 - line status */
	volatile unsigned int msr;	/* 18 - modem status */
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

/* 8 bits, no parity, 1 stop bit */
#define LCR_SETUP	LCR_DATA_8

void uart_gpio_init ( void );
void uart_clock_init ( void );

/* XXX - Ignores baud rate argument */
void
serial_init ( int baud )
{
	struct h3_uart *up = UART_BASE;

	uart_gpio_init();
	uart_clock_init();

	up->ier = 0;
	up->lcr = LCR_DLAB;

	up->divisor_msb = 0;
	up->divisor_lsb = BAUD_115200;

	up->lcr = LCR_SETUP;

	up->ier = IE_RDA | IE_TXE;
}

void
serial_putc ( char c )
{
	struct h3_uart *up = UART_BASE;

	while ( !(up->lsr & TX_READY) )
	    ;
	up->data = c;

	if ( c == '\n' )
	    serial_putc ( '\r' );
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

#ifdef notdef
void
serial_check ( int num )
{
	struct h3_uart *up = UART_BASE;

	printf ( " Uart: lsr/ier/iir %02x %02x %02x  %d\n", up->lsr, up->ier, up->iir, num );
}
#endif

int
serial_getc ( void )
{
	struct h3_uart *up = UART_BASE;

	while ( ! (up->lsr & RX_READY) )
	    ;
	return up->data;
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
