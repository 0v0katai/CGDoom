#include "libprof.h"

#define CPG_FRQCRA (*(volatile uint32_t *)0xa4150000)
#define CPG_FLLFRQ (*(volatile uint32_t *)0xa4150050)
#define CPG_PLLCR  (*(volatile uint32_t *)0xa4150024)

/* Current frequency of Pphi */
static int Pphi;

/* prof_init(): Initialize the profiler's timer */
int prof_init(void)
{
	*TMU0_TCOR = 0xffffffff;
	*TMU0_TCNT = 0xffffffff;
	*TMU0_TCR = 0x0000; /* UNIE=0 Pphi/4 */
	*TMU_TSTR |= 1;

	/* Determine the Pphi frequency based on the CPG configuration */
	int pll = CPG_PLLCR & 0x4000 /* if PLLE: STC+1 */
		? ((CPG_FRQCRA & 0x3f000000) >> 24) + 1
		: 1;
	int fll = CPG_PLLCR & 0x1000 /* if FLLE: FLF >> (SELXM != 0) */
		? (CPG_FLLFRQ & 0x07ff) >> ((CPG_FLLFRQ & 0xc000) != 0)
		: 1;
	/* Shift by P1FC+1 places */
	Pphi = (32768 * fll * pll) >> ((CPG_FRQCRA & 0x0000000f) + 1);

	return 0;
}

/* prof_quit(): Free the profiler's timer */
void prof_quit(void)
{
	*TMU_TSTR &= ~1;
}

/* prof_time(): Time spent in a given context, in microseconds */
uint32_t prof_time(prof_t prof)
{
	return ((uint64_t)prof.elapsed * 4 * 1000000) / Pphi;
}
