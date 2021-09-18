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

/* When loading from Flash, non-fragmented lumps are returned directly via
   pointers to Flash in order to save heap. Pointers to Flash can't be freed
   and are detected with the following macro. */
#define PTR_TO_FLASH(x) ( \
    ((x) >= FLASH_START && (x) < FLASH_END) || \
    ((x) >= FLASH_CACHED_START && (x) < FLASH_CACHED_END))

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

/* Whether to trust unaligned lumps. If this is set, all non-fragmented lumps
   will be accessed from ROM. Otherwise, lumps that are not 4-aligned will be
   loaded to the heap even if they are available. */
extern int CGD_TrustUnalignedLumps;

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
# define CGDOOM_SCREENS_BASE ((void *)( \
       ((uint32_t)GetVRAMAddress() & 0x1fffffff) | 0x80000000 \
   ))
#else
# define CGDOOM_SCREENS_BASE SaveVRAMBuffer
#endif

/* Amount of extra memory past the 2 MB line that we deem safe to access. */
extern int CGD_2MBLineMemory;

/* Maximum amount of memory beyond the 2 MB line */
#define CGDOOM_2MBLINEMEMORY_MAX (6 << 20)

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

#define ASSERT(x)

#endif //#ifndef PLATFORM_H
