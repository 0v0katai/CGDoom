#ifndef CGDOOM_H
#define CGDOOM_H

// CGDoom-specific definitions that cover both calculator and native builds.

#include <stdint.h>
#include "libprof.h"

/* VRAM pointer and size */
extern uint16_t *VRAM;
#define WIDTH  384
#define HEIGHT 216

/* Description of a WAD file selectable by user. */
typedef struct
{
    char name[32];
    uint16_t path[7+32];
    int size;
} WADFileInfo;

/* CGDoom statistics */
extern struct CGD_Perf CGD_Perf;
extern struct CGD_Stats CGD_Stats;

// CGDoom options (some more are specified in platform.h)

/* Enable demos in the title screen (most are incompatible and look bad) */
extern int CGD_EnableDemos;
/* The selected WAD is a split Ultimate Doom WAD with a single episode (this
   exists on the fx-CG 50 and is incorrectly detected by normal logic) */
extern int CGD_SingleEpisodeUltimate;
/* Skip this amount of frames after every rendered frame (default 1) */
extern int CGD_Frameskip;
/* WAD file name (without .wad), used to avoid save file conflicts */
extern const char *CGD_WADFileName;

// Global variables interfacing with Doom itself.

/* Map and episode to start at when loading the game. */
extern int startmap, startepisode;

// Keyboard interface.

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
// Control systems
//---

void CGD_Cheat();
void CGD_SwitchClip();
void CGD_FreeMem();
void CGD_CycleFrameskip();
void CGD_CycleGamma();
void CGD_ProfilerResults();

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

struct CGD_Stats
{
   /* Fragments in the WAD file */
   int WADFragments;
   /* Lowest fragment address (hints at filesystem entry point) */
   void const *WADLowestFragment;
   /* Number of index hits during fragment search */
   int WADIndexHits;

   /* Total memory allocated with Z_Malloc (bytes) */
   uint32_t MemoryAllocated;

   /* Number of lumps loaded, number of lumps not loaded (addressed directly in
      ROM), number of unaligned lumps loaded */
   int LumpsLoaded;
   int LumpsReferenced;
   int UnalignedLumpsLoaded;

   /* The total size for these three catefories (bytes) */
   uint64_t LumpsLoadedTotal;
   uint64_t LumpsReferencedTotal;
   uint64_t UnalignedLumpsLoadedTotal;
};

#endif /* CGDOOM_H */
