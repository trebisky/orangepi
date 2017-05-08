/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * traps.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  5/7/2017
 *
 */

static void
puts ( char *msg )
{
        serial_puts ( msg );
}

/* From Kyu prf.c */

#define HEX(x)  ((x)<10 ? '0'+(x) : 'A'+(x)-10)
#define PUTCHAR(x)      if ( buf <= end ) *buf++ = (x)

static char *
shex2( char *buf, char *end, int val )
{
        PUTCHAR( HEX((val>>4)&0xf) );
        PUTCHAR( HEX(val&0xf) );
        return buf;
}

static char *
shex8( char *buf, char *end, int val )
{
        buf = shex2(buf,end,val>>24);
        buf = shex2(buf,end,val>>16);
        buf = shex2(buf,end,val>>8);
        return shex2(buf,end,val);
}

/*
 * Printn prints a number n in base b.
 * We don't use recursion to avoid deep kernel stacks.
 */
static char *
sprintn ( char *buf, char *end, int n, int b)
{
        char prbuf[11];
        register char *cp;

        if (b == 10 && n < 0) {
            PUTCHAR('-');
            n = -n;
        }
        cp = prbuf;

        do {
            *cp++ = "0123456789ABCDEF"[n%b];
            n /= b;
        } while (n);

        do {
            PUTCHAR(*--cp);
        } while (cp > prbuf);

        return buf;
}

/* Handy - print string and decimal integer */
static void
print_num ( char *str, int arg )
{
	char buf[64];
	char *p;

	puts ( str );
	puts ( " " );
	p = sprintn ( buf, &buf[64], arg, 10 );
	*p = '\0';
	puts ( buf );
	puts ( "\n" );
}

/* Handy - print string and hex value */
static void
print_hex ( char *str, int arg )
{
	char buf[9];
	puts ( str );
	puts ( " " );
	shex8 ( buf, &buf[9], arg );
	buf[8] = '\0';
	puts ( buf );
	puts ( "\n" );
}

/* Handy - print string and another string */
static void
print_str ( char *str, char *arg )
{
	puts ( str );
	puts ( " " );
	puts ( arg );
	puts ( "\n" );
}

/* ------------------------------------- */
/* ------------------------------------- */

void trap ( char *who )
{
	int cpu;

	puts ( "---------------------- TRAP ------------------------\n" );
	puts ( "---------------------- TRAP ------------------------\n" );

	/* Read processor affinity register */
        asm volatile("mrc 15, 0, %0, cr0, cr0, 5" : "=r" (cpu) : : "cc");
        cpu &= 0x3;
	print_num ( "Core: ", cpu );

	print_str ( "  type:", who );
	print_hex ( "TEST: ", 0xc0ffee );
	print_num ( "NUM: ", 1234 );

	puts ( "---------------------- TRAP ------------------------\n" );
	puts ( "---------------------- TRAP ------------------------\n" );

	puts ( "Halted (spinning)\n" );
	for ( ;; ) ;
}

void trap_ui ( void ) { trap ( "undefined instruction" ); }
void trap_si ( void ) { trap ( "software interrupt" ); }
void trap_pa ( void ) { trap ( "prefetch abort" ); }
void trap_da ( void ) { trap ( "data abort" ); }
void trap_nu ( void ) { trap ( "not used" ); }
void trap_irq ( void ) { trap ( "IRQ" ); }
void trap_fiq ( void ) { trap ( "FIQ" ); }

/* Hook for gcc arithmetic for divide by zero */
void raise ( void ) {}

/* THE END */
