/* Driver for the H3 GIC
 *
 * The GIC is the ARM "Generic Interrupt Controller"
 * It has two sections, "cpu" and "dist"
 *
 * Tom Trebisky  1-4-2017, 10-20-2023
 */

#include "protos.h"

/* The H3 defines 157 interrupts (0-156)
 * We treat this as 160, more or less
 */
#define NUM_IRQ		157

#define IRQ_UART0	32

#define IRQ_TIMER0	50
#define IRQ_TIMER1	51

#define IRQ_R_TIMER0	65
#define IRQ_R_TIMER1	66
#define IRQ_R_TIMER2	74
#define IRQ_R_TIMER3	75

#define IRQ_ETHER	114

/* =============================== */

#define NUM_STATUS	16
#define NUM_CONFIG	10
#define NUM_TARGET	40
#define NUM_PRIO	40
#define NUM_MASK	5

struct h3_gic_dist {
	vu32 ctrl;		/* 0x00 */
	vu32 type;		/* 0x04 */
	vu32 iidr;		/* 0x08 */
	int __pad0[61];
	vu32 eset[NUM_MASK];	/* BG - 0x100 */
	int __pad1[27];
	vu32 eclear[NUM_MASK];	/* BG - 0x180 */
	int __pad2[27];
	vu32 pset[NUM_MASK];	/* BG - 0x200 */
	int __pad3[27];
	vu32 pclear[NUM_MASK];	/* BG - 0x280 */
	int __pad4[27];
	vu32 aset[NUM_MASK];	/* BG - 0x300 */
	int __pad5[27];
	vu32 aclear[NUM_MASK];	/* BG - 0x300 */
	int __pad55[27];
	vu32 prio[NUM_PRIO];	/* BG - 0x400 */
	int __pad6[216];
	vu32 target[NUM_TARGET];	/* 0x800 */
	int __pad7[216];
	vu32 config[NUM_CONFIG];	/* 0xc00 */
	int __pad77[54];
	vu32 status[NUM_STATUS];	/* 0xd00 - ISR */
	int __pad8[112];
	vu32 soft;		/* 0xf00 */
};

struct h3_gic_cpu {
	vu32 ctrl;		/* 0x00 */
	vu32 primask;		/* 0x04 */
	vu32 binpoint;	/* 0x08 */
	vu32 iack;		/* 0x0c */
	vu32 eoi;		/* 0x10 */
	vu32 run_pri;		/* 0x14 */
	vu32 high_pri;	/* 0x18 */
};

#define GIC_DIST_BASE	((struct h3_gic_dist *) 0x01c81000)
#define GIC_CPU_BASE	((struct h3_gic_cpu *) 0x01c82000)

#define	G0_ENABLE	0x01
#define	G1_ENABLE	0x02


#ifdef notdef
#define GIC_CPU_CTRL                    0x00
#define GIC_CPU_PRIMASK                 0x04
#define GIC_CPU_BINPOINT                0x08
#define GIC_CPU_INTACK                  0x0c
#define GIC_CPU_EOI                     0x10
#define GIC_CPU_RUNNINGPRI              0x14
#define GIC_CPU_HIGHPRI                 0x18

#define GIC_DIST_CTRL                   0x000
#define GIC_DIST_CTR                    0x004
#define GIC_DIST_ENABLE_SET             0x100
#define GIC_DIST_ENABLE_CLEAR           0x180
#define GIC_DIST_PENDING_SET            0x200
#define GIC_DIST_PENDING_CLEAR          0x280
#define GIC_DIST_ACTIVE_BIT             0x300
#define GIC_DIST_PRI                    0x400
#define GIC_DIST_TARGET                 0x800
#define GIC_DIST_CONFIG                 0xc00
#define GIC_DIST_SOFTINT                0xf00
#endif

void
gic_show_status ( void )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;
	u32 off;
	int i;

	printf ( "GIC base = %08x\n", gp );
	printf ( "GIC config = %08x\n", &gp->config[0] );
	printf ( "GIC status = %08x\n", &gp->status[0] );
	printf ( "GIC soft = %08x\n", &gp->soft );

	off = (u32) &gp->soft;
	off -= (u32) gp;
	printf ( "GIC off soft = %08x\n", off );

	for ( i=0; i<NUM_STATUS; i++ ) {
	    printf ( "ISR %d: %08x\n", i, gp->status[i] );
	}
}

void
gic_enable ( int irq )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;
	int x = irq / 32;
	unsigned long mask = 1 << (irq%32);

	gp->eset[x] = mask;
}

void
gic_unpend ( int irq )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;
	int x = irq / 32;
	unsigned long mask = 1 << (irq%32);

	gp->pclear[x] = mask;
}

static int first = 1;

void
gic_handler ( void )
{
	struct h3_gic_cpu *cp = GIC_CPU_BASE;
	int irq;
	//int cur_sp;

	irq = cp->iack;

	// printf ( "GIC handler %d\n", irq );

	//asm volatile ("add %0, sp, #0" : "=r"(cur_sp) );
	//printf ( "GIC: sp = %08x\n", cur_sp );

	/* Do we need to EOI the spurious ? */
	if ( irq == 1023 ) {
	    serial_putc ( 'X' );
	    serial_putc ( '\n' );
	    // return;
	}

#ifdef notdef
	if ( first ) {
	    printf ( "In gic_handler (first)\n" );
	    gic_show_status ();
	    first = 0;
	} else
	    gic_show_status ();
#endif

	/* The above shows:
	 * (handling IRQ_TIMER1 = 51)
	 * indeed the "8" is bit 51.
	 * We always see the "1" (in bit 32)
	 * IRQ 32 is UART0 ...
	ISR 0: 00000000
	ISR 1: 00080001
	 */

	if ( irq == IRQ_TIMER0 )
	    timer_handler ( 0 );
	if ( irq == IRQ_R_TIMER0 )
	    timer_handler ( 0 );

	if ( irq == IRQ_TIMER1 )
	    timer_handler ( 1 );
	if ( irq == IRQ_R_TIMER1 )
	    timer_handler ( 1 );

	if ( irq == IRQ_UART0 ) {
	    printf ( "Uart interrupt\n" );
	    uart_handler ();
	}

	cp->eoi = irq;
	// gic_unpend ( IRQ_TIMER0 );
	gic_unpend ( irq );

	// serial_putc ( '.' );
	// serial_putc ( '\n' );

	// printf ( "GIC iack = %08x\n", irq );
	// ms_delay ( 200 );
}

void
gic_init ( void )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;
	struct h3_gic_cpu *cp = GIC_CPU_BASE;
	unsigned long *p;
	int i;

	// printf ( "GIC init\n" );

#ifdef notdef
	p = (unsigned long *) & gp->target;
	printf ( "GIC target = %08x\n", p );
	p = (unsigned long *) & gp->config;
	printf ( "GIC config  = %08x\n", p );
	p = (unsigned long *) & gp->soft;
	printf ( "GIC soft = %08x\n", p );

	/* yields: GIC iidr = 0100143B
	 * The 43B part is correct for ARM.
	printf ( "GIC iidr = %08x\n", gp->iidr );
	 */
#endif

	/* Initialize the distributor */
	gp->ctrl = 0;

	/* make all SPI level triggered */
	for ( i=2; i<NUM_CONFIG; i++ )
	    gp->config[i] = 0;

	for ( i=8; i<NUM_TARGET; i++ )
	    gp->target[i] = 0x01010101;

	for ( i=8; i<NUM_PRIO; i++ )
	    gp->prio[i] = 0xa0a0a0a0;

	for ( i=1; i<NUM_MASK; i++ )
	    gp->eclear[i] = 0xffffffff;

	for ( i=0; i<NUM_MASK; i++ )
	    gp->pclear[i] = 0xffffffff;

	/* Allow either timer */
	gic_enable ( IRQ_TIMER0 );
	gic_enable ( IRQ_TIMER1 );

	/* Someday we will want this */
	gic_enable ( IRQ_UART0 );

	gp->ctrl = G0_ENABLE;

	/* ** now initialize the per CPU stuff.
	 *  XXX - the following will need to be done by each CPU
	 *  when we get multiple cores running.
	 */

	/* enable all SGI, disable all PPI */
	gp->eclear[0] = 0xffff0000;
	gp->eset[0]   = 0x0000ffff;

	/* priority for PPI and SGI */
	for ( i=0; i<8; i++ )
	    gp->prio[i] = 0xa0a0a0a0;

	cp->primask = 0xf0;
	cp->ctrl = 1;
}

extern volatile int timer_count;

/* This is TIMER 0 */
#define TIMER_MASK	0x40000

void
gic_check ( void )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;

	printf ( " GIC pending: %08x %08x %d\n", gp->pset[0], gp->pset[1], timer_count );
}

void
gic_poll ( void )
{
	struct h3_gic_dist *gp = GIC_DIST_BASE;

	for ( ;; ) {
	    // ms_delay ( 2000 );
	    if ( gp->pset[1] & TIMER_MASK ) {
		gic_check ();
		gic_handler ();
		printf ( "+GIC pending: %08x %08x %d\n", gp->pset[0], gp->pset[1], timer_count );
	    }
	}
}

void
gic_watch ( void )
{

	for ( ;; ) {
	    delay_x ();
	    delay_x ();
	    gic_check ();
	}
}

/* THE END */
