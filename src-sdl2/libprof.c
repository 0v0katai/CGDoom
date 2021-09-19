#include "libprof.h"

uint64_t prof_current_time(void)
{
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	return (uint64_t)tp.tv_sec * 1000000000ull + tp.tv_nsec;
}

uint32_t prof_time(prof_t prof)
{
	return -prof.elapsed / 1000;
}
