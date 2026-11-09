/* <platform.h> file for CASIO Graph 90+E / fx-CG 50 hardware */
#ifndef PLATFORM_H
#define PLATFORM_H

//---
// WAD file access in Flash
//---

/* Settings for file mappings: traverse the whole 32-MiB Flash */
#define FLASH_START ((const void *)0xA0000000)
#define FLASH_END   ((const void *)0xA2000000)
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

/* Block size for WAD files */
#define WAD_BLOCK_SIZE 4096
#define WAD_MAX_SIZE (16 * 1024 * 1024)
#define WAD_MAX_BLOCKS (WAD_MAX_SIZE / WAD_BLOCK_SIZE)
//---
// Display driver access
//---

/* Whether to use direct-DD access rather than intermediate VRAM. Enabling this
   option will provide the unused VRAM as additonal heap. (Comment out to
   disable.) */
#define CGDOOM_DIRECT_R61524

//---
// Memory layout
//---

/* PRAM0 is an area of SPU2 memory that supports only 32-bit access. It is used
   for the file mapping and some allocations; see <cgdoom-alloc.h>. */
#define PRAM0_START ((void *)0xfe200000)
#define PRAM0_END   ((void *)0xfe228000)

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
# define CGDOOM_SCREENS_BASE SecondaryVRAM
#endif

/* Amount of extra memory past the 2 MB line that we deem safe to access. */
extern int CGD_2MBLineMemory;

/* Maximum amount of memory beyond the 2 MB line */
#define CGDOOM_2MBLINEMEMORY_MAX (6 << 20)

//---

#include "fxcg/display.h"
#include "fxcg/misc.h"
#include "fxcg/file.h"
#include "fxcg/serial.h"
#include "fxcg/keyboard.h"
#include "fxcg/rtc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ASSERT(x)
#define GetOSVersion() ((char *)0x80020020)
#define printf(...)

#endif //#ifndef PLATFORM_H
