#ifndef CGDOOM_H
#define CGDOOM_H

#include "platform.h"
#include "libprof.h"
#include <stdint.h>

// CGDoom-specific definitions that cover both calculator and native builds.

#define SETTINGS_FILE_VERSION 1

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
} CGD_WADFileInfo;

/* Description of a file write that has been delated. */
typedef struct
{
   char filename[32];
   const void *data;
   int size;
} CGD_DelayedFileWrite;

/* File access method (numbering is used in checkbox; keep as is) */
enum {
    /* Use BFile (100% accurate but slows down the game quite a bit because of
       reads happening all the time; mostly a good reference for testing) */
    CGDOOM_WAD_BFILE = 0,
    /* Search fragments in physical ROM when loading the game, and copy by hand
       from ROM to RAM during accesses (much faster) */
    CGDOOM_WAD_MMAP = 1,
};

/* Delay file saves until exit to avoid quitting immediately */
#define CGDOOM_DELAY_SAVES

/* CGDoom statistics */
extern struct CGD_Perf CGD_Perf;
extern struct CGD_Stats CGD_Stats;

// CGDoom options (some more are specified in platform.h)

/* File access method */
extern int CGD_WADMethod;
/* Enable demos in the title screen (most are incompatible and look bad) */
extern int CGD_EnableDemos;
/* The selected WAD is a split Ultimate Doom WAD with a single episode (this
   exists on the fx-CG 50 and is incorrectly detected by normal logic) */
extern int CGD_SingleEpisodeUltimate;
/* Skip this amount of frames after every rendered frame (default 1) */
extern int CGD_Frameskip;
/* Whether to trust unaligned lumps. If this is set, all non-fragmented lumps
   will be accessed from ROM. Otherwise, lumps that are not 4-aligned will be
   loaded to the heap even if they are available. (CGDOOM_WAD_MMAP) */
extern int CGD_TrustUnalignedLumps;

// Global variables interfacing with Doom itself.

/* Map and episode to start at when loading the game. */
extern int startmap, startepisode;
/* WAD file name (without .wad), used to avoid save file conflicts */
extern const char *CGD_WADFileName;
/* Save Game operations delayed until closing the add-in (one per save) */
extern CGD_DelayedFileWrite CGD_DelayedSaves[6];
/* Name of record demo file */
extern const char *CGD_RecordDemoName;


// Keyboard interface.

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
