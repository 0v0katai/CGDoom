#include "cgdoom-frag.h"
#include "cgdoom.h"
#include "os.h"
#include "w_wad.h"

int CGD_Frag_Map(const char *lumpname, CGDoom_Frag *frag)
{
    int lump = W_CheckNumForName(lumpname);
    if(lump < 0)
        return 1;
    if(CGD_Options.WADMethod == CGDOOM_WAD_BFILE)
        return 1;

    lumpinfo_t *l = &lumpinfo[lump];
    int size = l->size;
    int position = l->position;
    int f = 0;

    while(f < CGDOOM_FRAG_ESTIMATE && size > 0) {
        int found = FindInFlash(&frag->data[f], size, position);
        frag->size[f] = found;

        size -= found;
        position += found;
        f++;
    }

    if(size) {
        I_Error("CGDoom_Frag_Map: failed on '%s' (size=%d, offset=%d), could only"
            " obtain %d", lumpname, l->size, l->position, l->size - size);
        return 1;
    }

    frag->size[f] = 0;
    return 0;
}

uint8_t CGD_Frag_u8(CGDoom_Frag const *frag, int offset)
{
    int f = 0;
    while(frag->size[f] != 0 && offset >= frag->size[f]) {
        offset -= frag->size[f];
        f++;
    }

    if(frag->size[f] == 0)
        return 0; /* read out-of-bounds */

    const uint8_t *data = frag->data[f];
    return data[offset];
}

short CGD_Frag_i16LE(CGDoom_Frag const *frag, int offset)
{
    uint8_t b1 = CGD_Frag_u8(frag, offset);
    uint8_t b2 = CGD_Frag_u8(frag, offset + 1);

    return (b2 << 8) | b1;
}

int CGD_Frag_i32LE(CGDoom_Frag const *frag, int offset)
{
    uint8_t b1 = CGD_Frag_u8(frag, offset);
    uint8_t b2 = CGD_Frag_u8(frag, offset + 1);
    uint8_t b3 = CGD_Frag_u8(frag, offset + 2);
    uint8_t b4 = CGD_Frag_u8(frag, offset + 3);

    return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}
