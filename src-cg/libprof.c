#include "libprof.h"

/* prof_init(): Initialize the profiler's timer */
int prof_init(void)
{
	*TMU0_TCOR = 0xffffffff;
	*TMU0_TCNT = 0xffffffff;
	*TMU0_TCR = 0x0000; /* UNIE=0 Pphi/4 */

	*TMU_TSTR |= 1;
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
	/* Hardcoded for the default overclock settings */
	int Pphi = 29491200;
	return ((uint64_t)prof.elapsed * 4 * 1000000) / Pphi;
}
