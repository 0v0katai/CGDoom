#ifndef CGDOOM_H
#define CGDOOM_H

/* CGDoom-specific definitions that cover both calculator and native builds. */

#include <stdint.h>
#include "libprof.h"

/* VRAM pointer and size */
extern uint16_t *VRAM;
#define WIDTH  384
#define HEIGHT 216

/* Description of a WAD file selectable by user. */
typedef struct
{
    char name[16];
    uint16_t path[23];
    int size;
} WADFileInfo;

// TODO: Move developer info here

/* Global variables interfacing with Doom itself. */

/* Map and episode to start at when loading the game. */
extern int startmap, startepisode;

/* Keyboard interface. */

// Special key names for cheats, debugs, etc (completes doomdef.h).
#define SKEY_CHEAT       0x10001
#define SKEY_DECVP       0x10002
#define SKEY_INCVP       0x10003
#define SKEY_NOCLIP      0x10004
#define SKEY_GAMMA       0x10005
#define SKEY_FREEMEM     0x10006
#define SKEY_FPSCOUNTER  0x10007
#define SKEY_FRAMESKIP   0x10008
#define SKEY_PROFILER    0x10009

// Scan keyboard (the previous state is also retained).
void UpdateKeyboardState(void);

// Check if a Doom key has just been pressed, or released.
int KeyWasJustPressed(int key);
int KeyWasJustReleased(int key);

//---
// Performance metrics
//---

struct CGD_Perf
{
   prof_t DynamicAllocation;
   prof_t GraphicsRendering;
   prof_t DisplayInterface;
   prof_t LumpLoading;
   prof_t UnalignedLumpLoading;
};

#endif /* CGDOOM_H */
