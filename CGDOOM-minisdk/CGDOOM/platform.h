/* <platform.h> file for CASIO Graph 90+E / fx-CG 50 hardware */
#ifndef PLATFORM_H
#define PLATFORM_H

//---
// WAD file access in Flash
//---

/* File access method (numbering is used in checkbox; keep as is) */
enum {
    /* Use BFile (100% accurate but slows down the game quite a bit because of
       reads happening all the time; mostly a good reference for testing) */
    CGDOOM_WAD_BFILE = 0,
    /* Search fragments in physical ROM when loading the game, and copy by hand
       from ROM to RAM during accesses (much faster) */
    CGDOOM_WAD_MMAP = 1,
};

/* Settings for file mappings: traverse the whole 32-MiB Flash */
#define FLASH_START ((const void *)0xA0000000)
#define FLASH_END   ((const void *)0xA2000000)
/* Where we expect the file system to start, approximately (this region is
   searched first to hit sectors more quickly) */
#define FLASH_FS_HINT ((const void *)0xA0C00000)
/* Flash too, but cached; slower for sector searches but much faster for actual
   data loads while in-game */
#define FLASH_CACHED_START ((const void *)0x80000000)
#define FLASH_CACHED_END   ((const void *)0x82000000)

/* Storage unit is a cluster of 512 bytes; Fugue tries to use clusters of 4 kiB
   (8 sectors) but in exceptional circumstances cluster alignment can be lost
   (such as when sectors are dead) */
#define FLASH_PAGE_SIZE 512
#define FLASH_PAGE_SIZE_LOG2 9
#define FLASH_PAGE_COUNT ((FLASH_END-FLASH_START) / FLASH_PAGE_SIZE)

/* Size of Bfile reads; performance is good when it's at least a cluster */
#define FLASH_BFILE_UNIT 4096

/* Whether to index ROM sectors most likely to have data to use in sector
   searches (comment out to disable) */
#define FLASH_INDEX
/* Index contains 4 kiB cluster from FLASH_FS_HINT to FLASH_END; fragments are
   almost always 4 kiB-aligned, and only occasionally not */
#define FLASH_INDEX_SIZE ((FLASH_END-FLASH_FS_HINT) / 4096)

//---
// Display driver access
//---

/* Whether to use direct-DD access rather than intermediate VRAM. Enabling this
   option will provide the unused VRAM as additonal heap. (Comment out to
   disable.) */
#define CGDOOM_DIRECT_R61524

//---
// Memory distribution
//---

/* When direct-display access is disabled, put the screens in secondary VRAM
   and use VRAM for rendering. When it's enabled, put the screens in VRAM and
   use the secondary VRAM as heap. We swap because using the VRAM as heap would
   cause problems with error screens (stepping through errors would crash). */
#ifdef CGDOOM_DIRECT_R61524
# define CGDOOM_SCREENS_BASE ((void *)GetVRAMAddress())
#else
# define CGDOOM_SCREENS_BASE SaveVRAMBuffer
#endif

/* The primary/secondary VRAM covers 384*216*2 = 162 kiB:
   -> Give 320*200 = 64 kB to screens[0] (main display)
   -> Give 320*200 = 64 kB to screens[1] (intermissions)
   -> Give 320*32  = 10 kiB to screens[4] (status bar)
   -> 27 kiB are left */
#define CGDOOM_SCREENS_0 (CGDOOM_SCREENS_BASE)
#define CGDOOM_SCREENS_1 (CGDOOM_SCREENS_BASE + 320*200)
#define CGDOOM_SCREENS_4 (CGDOOM_SCREENS_BASE + 320*200*2)

//---

#include "keyboard.hpp"
#include "keyboard_syscalls.h"
#include "APP_syscalls.h"
#include "CONVERT_syscalls.h"
#include "SYSTEM_syscalls.h"
#include "RTC_syscalls.h"
#include "MCS_syscalls.h"
#include "fxcg/display.h"
#include "fxcg/misc.h"
#include "fxcg/file.h"
#include "fxcg/serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define WIDTH 384
#define HEIGHT 216

#define KEY_PRGM_OPTN 68
#define KEY_PRGM_MENU 48
#define KEY_PRGM_POWER 57
#define KEY_PRGM_EXE 31

#define EmulHack_Sleep(x)
#define ASSERT(x)
#define InitFlashSimu(filename)

/* Description of a WAD file selectable by user. */
typedef struct
{
    char name[16];
    uint16_t path[23];
    int size;
} WADFileInfo;

#endif //#ifndef PLATFORM_H
