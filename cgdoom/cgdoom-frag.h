#ifndef CGDOOM_FRAG_H
#define CGDOOM_FRAG_H

#include <stdint.h>

/* CGDoom's abstracted access to fragmented lumps

   In CGDoom, the main factor limiting compatibility is the amount of available
   heap. Therefore, it is beneficial to reduce the pressure on the heap by
   using lumps directly from ROM. For non-fragmented lumps it's mostly
   straightforward and only involves alignment independence, but for fragmented
   lumps it's quite more involved.

   This header offers an interface that can be used to access fragmented lumps
   in ROM by deferring every access to an intermediate function that looks up
   the proper lump fragment.

   This is /very/ slow, without a doubt, but speed is not always a limiting
   factor. One of the main uses for this is displaying full-screen patches
   without loading them, which occurs in the main menu and in the intermission
   screens. The latter is the main focus, since the intermission picture needs
   to be loaded on top of level data, which may leave the heap both full and
   fragmented.

   This interface exists just in case it is be useful for other lumps. */

/* Maximum number of fragments that we expect in a lump (+1 for terminator) */
#define CGDOOM_FRAG_ESTIMATE 33

/* Fragmented lump lookup table */
typedef struct
{
    const void *data[CGDOOM_FRAG_ESTIMATE];
    int size[CGDOOM_FRAG_ESTIMATE+1];

} CGDoom_Frag;

/* Compute the lookup table for a lump; if successful, returns 0 */
int CGD_Frag_Map(const char *lumpname, CGDoom_Frag *frag);

/* Read different types within the lump */
uint8_t CGD_Frag_u8(CGDoom_Frag const *frag, int offset);
short CGD_Frag_i16LE(CGDoom_Frag const *frag, int offset);
int CGD_Frag_i32LE(CGDoom_Frag const *frag, int offset);

#endif /* CGDOOM_FRAG_H */
