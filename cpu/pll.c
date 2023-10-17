/* Access to the CCU and PLL for cpu clock
 * in the Orange Pi (Allwinner H3 chip)
 *
 * From Kyu: orange_pi/ccu.c
 *
 * Tom Trebisky  10-17-2023
 */

struct h3_ccu {
        volatile unsigned int pll_cpux_ctrl;    /* 00 */
        int __pad0[3];
        int __pad1[16];
        volatile unsigned int cpux_ax1_cfg;     /* 50 */
        int __pad2[3];
        volatile unsigned int gate0;            /* 60 */
        volatile unsigned int gate1;            /* 64 */
        volatile unsigned int gate2;            /* 68 */
        volatile unsigned int gate3;            /* 6c */
};

#define CCU_BASE (struct h3_ccu *) 0x01c20000

#define F_24M   (24 * 1000 * 1000)

/* bits in the pll_cpux_ctrl register */
#define CPUX_PLL_ENABLE         0x80000000
#define CPUX_PLL_LOCK           0x10000000
#define CPUX_SDM_ENA            1<<24   /* no idea what this is, leave it 0 */

/* ==================================================== */
/* ==================================================== */

void
set_cpu_clock_1008 ( void )
{
        struct h3_ccu *cp = CCU_BASE;
        int clock = 1008;
        int new_n, new_k, new_pll;
        int tmo;

        new_n = clock / 24;
        new_k = 1;
        if ( new_n > 32 ) {
            new_n /= 2;
            new_k = 2;
        }

        new_pll = CPUX_PLL_ENABLE
            | (new_n-1) << 8
            | (new_k-1) << 4;

        cp->pll_cpux_ctrl = new_pll;
        printf ( "new CPU pll_cpux_ctrl = %08x\n", cp->pll_cpux_ctrl );

        for ( tmo = 1000; tmo; tmo-- )
            if ( cp->pll_cpux_ctrl & CPUX_PLL_LOCK )
                break;

        // This reports 0, which means the delay caused by the printf
        // above to show the PLL reg value is plenty.
        // printf ( "new CPU pll timeout counter = %d\n", 1000 - tmo );

        if ( tmo == 0 )
            printf ( "new CPU setting failed (timed out)\n" );
}

/* Return CPU clock rate in Hz */
unsigned int
get_cpu_clock ( void )
{
        struct h3_ccu *cp = CCU_BASE;
        int source;
        unsigned int val;
        int n, k, m;
        unsigned int freq;

        printf ( "CCU cpux_ax1_cfg = %08x\n", cp->cpux_ax1_cfg );
        printf ( "CCU pll_cpux_ctrl = %08x\n", cp->pll_cpux_ctrl );

        source = (cp->cpux_ax1_cfg >> 16) & 0x3;
        if ( source == 0 )
            return 32768;               /* LOSC */
        if ( source == 1 )
            return F_24M;               /* OSC24M */

        /* the usual case is "source = 2" where the PLL yields the clock */
        /* I see 0x90001b21 for the PLL register, so
         *  n = 0x1b + 1 = 28, k = 2 + 1 = 3, m = 1 + 1 = 2;
         *  freq = 1008000000 = 24M * 42
         */
        val = cp->pll_cpux_ctrl;
        n = ((val>>8) & 0x1f) + 1;
        k = ((val>>4) & 0x03) + 1;
        m = (val & 0x3) + 1;
        freq = F_24M * n * k / m;

        printf ( "CCU n, k, m = %d %d %d\n", n, k, m );

        printf ( "clock source: %d\n", source );

        // printf ( "PLL %08x\n", val );
        printf ( "CPU clock: %d\n", freq );
        return freq;
}

/* THE END */
