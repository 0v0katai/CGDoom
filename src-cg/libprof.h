//---
//	libprof: A manual profiling library for gint
//
// THIS VERSION IS MODIFIED FOR CGDOOM/LIBFXCG.
//---

#ifndef LIBPROF_LIBPROF
#define LIBPROF_LIBPROF

#include <stdint.h>

//---
//	Initialization
//---

/* prof_init(): Initialize the profiler's timer */
int prof_init(void);

/* prof_quit(): Free the profiler's timer */
void prof_quit(void);

//---
//	Runtime time measurement
//---

#define TMU0_TCOR ((volatile uint32_t *)0xa4490008)
#define TMU0_TCNT ((volatile uint32_t *)0xa449000c)
#define TMU0_TCR  ((volatile uint16_t *)0xa4490010)
#define TMU_TSTR  ((volatile uint8_t  *)0xa4490004)

/* Context object, has an elasped delay and a recursion level. This object can
   be created on the stack of a function that measures its execution time,
   except if the function is recursive, in which case it should be either
   static or global. */
typedef struct prof_t
{
	uint32_t rec;
	uint32_t elapsed;
} prof_t;

/* prof_make(): Create a new context object
   A context can be cleared by assigning it prof_make() again. */
#define prof_make() ((prof_t){ 0, 0 })

/* Timer counter */
#define PROF_TCNT TMU0_TCNT

/* prof_enter(): Start counting time for a function
   This macro should be called at the start of the context scope. If the
   function was already executing then the deepest instance in the stack is
   used instead of creating a new counter. */
#define prof_enter(prof) {				\
	if(!prof.rec++) prof.elapsed += *PROF_TCNT;	\
}

/* prof_leave(): Stop counting time for a function
   This should be called at the end of the context scope; it only actually
   stops if there is no deeper instance of the context in the stack. If there
   are not as exactly as many prof_leave()'s as prof_enter()'s then the
   resulting time measure will not be relevant at all. */
#define prof_leave(prof) {				\
	if(!--prof.rec) prof.elapsed -= *PROF_TCNT;	\
}

/* prof_exec(): Measure a single block of code
   This operation can be used when profiling is not required, and instead
   libprof is used to measure the performance of a single bit of code. Use it
   like this:

       uint32_t elasped_us = prof_exec({
           exec_code();
       }); */
#define prof_exec(code) ({		\
	prof_t prof = prof_make();	\
	prof_enter(prof);		\
	code;				\
	prof_leave(prof);		\
	prof_time(prof);		\
})

//---
//	Post-measurement analysis
//---

/* prof_time(): Time spent in a given context, in microseconds
   Should only be called when the context is not currently executing. */
uint32_t prof_time(prof_t prof);

#endif /* LIBPROF_LIBPROF */
