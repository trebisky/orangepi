/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * floater.c for the Orange Pi PC and PC Plus
 *
 * Tom Trebisky  5/13/2017
 *
 */

static void fail ( void );
static int sqrt_i ( int );
static int sqrt_d ( int );

void
arm_float ( void )
{
        int val;
        int num = 2;
	double fval = 5.0;

	// This should cause a trap
        // fail ();

	// Enable the vector floating point unit
        fp_enable ();

	fval /= 3.0;
	val = fval * 10000;
	print_num ( "5/3 is:", val );

        val = sqrt_i ( num );
        // printf ( "Square root of %d is %d\n", num, val );
	print_num ( "Square root is:", val );

        val = sqrt_d ( num );
        // printf ( "Square root of %d is %d\n", num, val );
	print_num ( "Square root is:", val );
}

#ifdef notdef
/* We want to trigger some kind of fault.
 * Amazingly, ANY address on the Orange Pi is readable.
 * We do this to ensure that our exception handling code works.
 */
static void
fail ( void )
{
	puts ( "Preparing to fail ...\n" );

	/* This don't work !!
	long *p = (long *) 0xa0000000;
	*p = 0;
	 */

	// this is a handy thing gcc provides.
	// it yields an undefined instruction trap.
	//__builtin_trap ();

	// this yields an undefined instruction.
	// this is supposed to be an ARM
	// permanently undefined instruction encoding.
	//    0xf7fXaXXX
	asm volatile (".word 0xf7f0a000\n");

	// We can use this to test the fault routine
	// trap_ui ();

	/* We better not see this message */
	puts ( "All done failing.\n" );
}
#endif

static int
sqrt_i ( int arg )
{
        float farg = arg;
        float root;

        asm volatile ("vsqrt.f32 %0, %1" : "=w" (root) : "w" (farg) );

        return 10000 * root;
}

static int
sqrt_d ( int arg )
{
        double farg = arg;
        double root;

        asm volatile ("vsqrt.f64 %0, %1" : "=w" (root) : "w" (farg) );

        return 10000 * root;
}

/* THE END */
