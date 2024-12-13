//---
// This is a dummy version of libprof for Linux with clock_gettime(2).
//---

#ifndef LIBPROF_LIBPROF
#define LIBPROF_LIBPROF

#include <stdint.h>
#include <time.h>

#define prof_init()
#define prof_quit()

typedef struct prof_t
{
    int64_t elapsed; /* in nanoseconds, generally */
    uint32_t rec;
} prof_t;

#define prof_make() ((prof_t){ 0, 0 })

#define prof_current_time() 0

#define prof_enter(prof)

#define prof_leave(prof)

#define prof_exec(code) ({      \
    prof_t prof = prof_make();  \
    prof_enter(prof);       \
    code;               \
    prof_leave(prof);       \
    prof_time(prof);        \
})

#define prof_time(prof) 1

#endif /* LIBPROF_LIBPROF */
