#include "cgdoom-frag.h"
#include "os.h"
#include "w_wad.h"

int CGD_Frag_Map(const char *lumpname, CGD_Frag *frag)
{
    int lump = W_CheckNumForName(lumpname);
    if(lump < 0)
        return 1;

    lumpinfo_t *l = &lumpinfo[lump];
    int size = l->size;
    int position = l->position;
    int f = 0;

    while(f < CGD_FRAG_ESTIMATE && size > 0) {
        int found = FindInFlash(&frag->data[f], size, position);
        frag->size[f] = found;

        size -= found;
        position += found;
        f++;
    }

    if(size) {
        I_Error("CGD_Frag_Map: failed on '%s' (size=%d, offset=%d), could only"
            " obtain %d", lumpname, l->size, l->position, l->size - size);
        return 1;
    }

    frag->size[f] = 0;
    return 0;
}

byte CGD_Frag_u8(CGD_Frag const *frag, int offset)
{
    int f = 0;
    while(frag->size[f] != 0 && offset >= frag->size[f]) {
        offset -= frag->size[f];
        f++;
    }

    if(frag->size[f] == 0)
        return 0; /* read out-of-bounds */

    const byte *data = frag->data[f];
    return data[offset];
}

short CGD_Frag_i16LE(CGD_Frag const *frag, int offset)
{
    byte b1 = CGD_Frag_u8(frag, offset);
    byte b2 = CGD_Frag_u8(frag, offset + 1);

    return (b2 << 8) | b1;
}

int CGD_Frag_i32LE(CGD_Frag const *frag, int offset)
{
    byte b1 = CGD_Frag_u8(frag, offset);
    byte b2 = CGD_Frag_u8(frag, offset + 1);
    byte b3 = CGD_Frag_u8(frag, offset + 2);
    byte b4 = CGD_Frag_u8(frag, offset + 3);

    return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}
