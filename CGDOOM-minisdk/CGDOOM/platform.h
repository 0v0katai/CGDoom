/* <platform.h> file for CASIO Graph 90+E / fx-CG 50 hardware */
#ifndef PLATFORM_H
#define PLATFORM_H

//---
// WAD file access method (enable exactly one)
//---

/* Use BFile dynamically (slow but entirely accurate, a good reference) */
// #define CGDOOM_WAD_BFILE
/* Search fragments in physical ROM and copy by hand (faster) */
#define CGDOOM_WAD_MAPPING
/* Search fragments in phyiscal ROM and copy with DMA (even faster) */
// #define CGDOOM_WAD_MAPPING_DMA

/* Settings for file mappings: traverse the whole 32-MiB Flash */
#define FLASH_START (0xA0000000)
#define FLASH_END   (0xA2000000)
/* Storage unit is a cluster of 512 bytes */
#define FLASH_PAGE_SIZE 512
#define FLASH_PAGE_SIZE_LOG2 9
#define FLASH_PAGE_COUNT ((FLASH_END-FLASH_START) / FLASH_PAGE_SIZE)

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

#endif //#ifndef PLATFORM_H
