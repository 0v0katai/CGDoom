#ifndef CGDOOM_H
#define CGDOOM_H

#include "cgdoom-kbd.h"
#include "platform.h"
#include "libprof.h"
#include <stdint.h>

//---
// Memory layout
//---

/* Size of VRAM buffers */
#define WIDTH   384
#define HEIGHT  216
/* Main and secondary VRAMs (both supplied by the OS) */
extern uint16_t *VRAM;
extern uint16_t *SecondaryVRAM;

//---
// Filesystem
//---

/* Description of a WAD or demo file selectable by user. */
typedef struct
{
    char name[32];
    uint16_t path[7+32];
    int size;

} CGDoom_FileInfo;

/* Description of a file write that has been delayed. */
typedef struct
{
   char filename[32];
   const void *data;
   int size;

} CGDoom_DelayedFileWrite;

/* IDs for delayed saves */
enum {
   CGD_DELAYEDSAVES_SAVES = 0, /* 6 entries */
   CGD_DELAYEDSAVES_DEMO = 6,
   CGD_DELAYEDSAVES_COUNT,
};
/* Save Game operations delayed until closing the add-in (one per save) */
extern CGDoom_DelayedFileWrite CGD_DelayedSaves[CGD_DELAYEDSAVES_COUNT];

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

//---
// CGDoom options
//---

/* The settings file is text-based and probably doesn't ever need to change
   even if settings are added or removed. This is just in case. */
#define CGDOOM_SETTINGS_FILE_VERSION 1

typedef struct {
    /* Enable technical detail screens. These screens are shown at the start
       and end of the game and display some statistics and debug data. */
    int DeveloperInfo;
    /* File access method (see above). */
    int WADMethod;
   /* Whether to trust unaligned lumps. If this is set, all non-fragmented
      lumps will be accessed from ROM. Otherwise, lumps that are not 4-aligned
      will be loaded to the heap even if they are available. This option is
      relevant only if WADMethod == CGDOOM_WAD_MMAP. */
    int TrustUnalignedLumps;
    /* Skip title screen and get straight to gameplay. */
    int AutoStart;
    /* Enable demos in the title screen (most are for previous versions of
       the engine and just look bad). */
    int EnableDemos;
    /* Enable the use of experimental memory. */
    int EnableExperimentalMemory;
    /* Custom controls. */
    CGDoom_Keymap Keymap;

} CGDoom_Options;

extern CGDoom_Options CGD_Options;

// CGDoom settings that affect the game

/* The selected WAD is a split Ultimate Doom WAD with a single episode (this
   exists on the fx-CG 50 and is incorrectly detected by normal logic) */
extern int CGD_SingleEpisodeUltimate;
/* Skip this amount of frames after every rendered frame (default 1) */
extern int CGD_Frameskip;
/* WAD file name (without .wad), used to name save and demo files */
extern const char *CGD_WADFileName;
/* Name of record demo file */
extern const char *CGD_RecordDemoName;

//---
// Control functions (mapped to special keys)
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

typedef struct
{
   prof_t DynamicAllocation;
   prof_t GraphicsRendering;
   prof_t DisplayInterface;
   prof_t LumpLoading;
   prof_t UnalignedLumpLoading;

} CGDoom_Perf;

extern CGDoom_Perf CGD_Perf;

//---
// Game and debugging statistics
//---

typedef struct
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

} CGDoom_Stats;

extern CGDoom_Stats CGD_Stats;

#endif /* CGDOOM_H */
