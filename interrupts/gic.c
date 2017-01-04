/* Driver for the H3 GIC
 *
 * The GIC is the ARM "Generic Interrupt Controller"
 * It has two sections, "cpu" and "dist"
 *
 * Tom Trebisky  1-4-2017
 */

#define GIC_DIST_BASE	0x01c81000
#define GIC_CPU_BASE	0x01c82000

struct h3_gic_dist {
	volatile unsigned long data;
};

struct h3_gic_cpu {
	volatile unsigned long data;
};

void
gic_init ( void )
{
}

/* THE END */
