//---
// This is a dummy version of libprof for Linux with clock_gettime(2).
//---

#ifndef LIBPROF_LIBPROF
#define LIBPROF_LIBPROF

#include <inttypes.h>
#include <time.h>

#define prof_init()
#define prof_quit()

typedef struct prof_t
{
    int64_t elapsed; /* in nanoseconds, generally */
    uint32_t rec;
} prof_t;

#define prof_make() ((prof_t){ 0, 0 })

uint64_t prof_current_time(void);

#define prof_enter(prof) { \
    if(!prof.rec++) prof.elapsed += prof_current_time(); \
}

#define prof_leave(prof) { \
    if(!--prof.rec) prof.elapsed -= prof_current_time(); \
}

#define prof_exec(code) ({      \
    prof_t prof = prof_make();  \
    prof_enter(prof);       \
    code;               \
    prof_leave(prof);       \
    prof_time(prof);        \
})

uint32_t prof_time(prof_t prof);

#endif /* LIBPROF_LIBPROF */
