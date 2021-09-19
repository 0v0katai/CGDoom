#ifndef CGDOOM_ALLOC_H
#define CGDOOM_ALLOC_H

#include <stddef.h>

/* The simple CGDoom allocator from SPU2 memory

   In CGDoom, the main bottleneck is memory. Speed is a concern but the CG-50
   is good enough at it that most titles and levels are pretty playable.
   However, a level that doesn't load is never playable. Therefore, memory
   limits cause more problems to the user experience than any other problem.

   To deal with this, several tools have been used; mainly the Doom allocator
   in z_zone.c has been extended to support multiple zones, which are supplied
   in the modified I_ZoneBase() function. This, and of course different memory
   areas have been freed up of whatever data they held in order to be used as
   heap.

   However, there are some areas that cannot be included there. Even the OS
   heap can be used as a default in Z_Malloc() with some effort, but one of
   the resources escapes even these options: SPU2 memory.

   I don't want to delve into the specifics of SPU2 memory as it's extremely
   strange; there are only two things that you should know about it:
   * There is a 160 kiB area called PRAM0 that only supports 32-bit accesses.
   * There is one 168 kiB area and two 48 kiB areas, called XRAM0, YRAM0 and
     YRAM1, that only support 32-bit accesses and every access only addresses
     24 bits of actual memory (so they span 224 kiB and 64 kiB of pointers).

   PRAM0 can be used fairly easily but we must guarantee that only 32-bit
   accesses are used. This means it's restricted to arrays of pointers, ints,
   and fixed_t mainly. In addition to the data though, the control structures
   of the heap must also use only 32-bit accesses, which would require pretty
   large changes in Z_Malloc.

   Instead, CGDoom provides a very, very simple heap structure on PRAM0. This
   is a trivial doubly-linked list with merging, intended to move out a handful
   of static buffers out of the main heap. Its use is voluntarily marginal.

   For stability, the allocator defaults to Z_Malloc() on failure. This is
   because a number of arrays that we direct to PRAM0 have variable size (like
   the WAD lump cache) and this extra flexibility is required to consistently
   work on a variety of WADs. (Diversions to the OS heap in previous versions
   of CGDoom had such problems, and I myself moved the lump cache to Z_Malloc
   because it didn't fit in some games.)  */

/* Initialize the area. */
void CGD_PRAM_Init(void *start, void *end);

/* Allocation functions. */
void *CGD_PRAM_Malloc(size_t size);
void CGD_PRAM_Free(void *ptr);
void *CGD_PRAM_Zalloc(size_t size);

#endif /* CGDOOM_ALLOC_H */
