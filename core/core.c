/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * core.c
 *
 * Routines to start a second core on the Allwinner H3
 *
 * Almost all of this is taken from Kyu multicore.c
 *
 * Tom Trebisky  10/17/2023
 */

#include "protos.h"

void
core_main ( void )
{
}

/* ================================================================*/
/* ================================================================*/
/* ================================================================*/

#define CPUCFG_BASE     0x01f01c00
#define PRCM_BASE       0x01f01400

/* The CPUCFG registers are in a 1K section starting at 0x01f01c00
 * The ROM_START register is in here and is undocumented in the H3 datasheet.
 * See page 143 in the datasheet.
 * The overall address map is on page 84
 */

// #define ROM_START       ((volatile unsigned long *) 0x01f01da4)
#define ROM_START        ((volatile unsigned long *) (CPUCFG_BASE + 0x1A4))

/* We don't need these, but want to play with them */
// #define PRIVA        ((volatile unsigned long *) (CPUCFG_BASE + 0x1A8))
// #define PRIVB        ((volatile unsigned long *) (CPUCFG_BASE + 0x1AC))

#define GEN_CTRL        ((volatile unsigned long *) (CPUCFG_BASE + 0x184))

// #define DBG_CTRL1       ((unsigned long *) (CPUCFG_BASE + 0x1e4))

#define POWER_OFF       ((unsigned long *) (PRCM_BASE + 0x100))

/* We just pick a location in low memory (the on chip ram)
 */
#define SENTINEL        (volatile unsigned long *) 4

static void launch_core ( int );
static int wait_core ( void );

#define NUM_CORES       4

static int      core_run[NUM_CORES] = { 1, 0, 0, 0 };

/* Start a single core.
 * This is the public entry point.
 */
// h3_start_core ( int core, cfptr func, void *arg )
void
h3_start_core ( int core )
{
        int stat;

        /* We won't try to restart core 0 */
        if ( core < 1 || core >= NUM_CORES )
            return;

        if ( core_run[core] )
            return;

        *SENTINEL = 0xdeadbeef;

        /* Added 10-13-2018 */
        // core_func[core] = func;

        // printf ( "Starting core %d ...\n", core );
        launch_core ( core );
        // printf ( "Waiting for core %d ...\n", core );

        // watch_core ();
        stat = wait_core ();
        if ( ! stat ) {
            printf ( "** Core %d failed to start\n", core );
            return;
        }

        core_run[core] = 1;

        // if ( stat ) printf ( " Core %d verified to start\n", core );

        *ROM_START = 0;
}

/* This is in core_start.S */
extern void core_startup ( void );

/* Manipulate the hardware to power up a core
 */
static void
launch_core ( int core )
{
        volatile unsigned long *reset;
        unsigned long mask = 1 << core;

        reset = (volatile unsigned long *) ( CPUCFG_BASE + (core+1) * 0x40);
        // printf ( "-- reset = %08x\n", reset );

        // *ROM_START = (unsigned long) secondary_start;  /* in locore.S */
        *ROM_START = (unsigned long) core_startup;  /* in core_start.S */

        *reset = 0;                     /* put core into reset */

        *GEN_CTRL &= ~mask;             /* reset L1 cache */
        *POWER_OFF &= ~mask;            /* power on */

        // thr_delay ( 2 );
        // delay_ms ( 2 );
        ms_delay ( 2 );

        *reset = 3;                     /* take out of reset */
}

/* Most of the time a core takes 30 counts to start */
#define MAX_CORE        100

static int
wait_core ( void )
{
        volatile unsigned long *sent;
        int i;

        sent = SENTINEL;

        for ( i=0; i<MAX_CORE; i++ ) {
            if ( *sent == 0 ) {
                // printf ( "Core started in %d\n", i );
                return 1;
            }
        }
        return 0;
}

/* THE END */
