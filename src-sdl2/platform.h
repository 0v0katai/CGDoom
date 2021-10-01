/* <platform.h> file for Linux/SDL2 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "keyboard.hpp"

//---
// Memory layout
//---

extern char _PRAM0[160*1024];
#define PRAM0_START ((void *)_PRAM0)
#define PRAM0_END ((void *)_PRAM0 + sizeof _PRAM0)

#define CGDOOM_SCREENS_BASE (void *)SecondaryVRAM

//---
// fx-CG-specific functions not defined on emulator
//---

#define CreateFileMapping(...) 0
#define FindInFlash(...) 0
#define FindZeroedMemory(...) 0

/* On the emulator all lumps are allocated with malloc(), so there are not
   pointers-to-flash to account for when freeing. */
#define PTR_TO_FLASH(p) 0

//---
// Rendering system emulation
//---

void Bdisp_PutDisp_DD(void);
void Bdisp_AllClr_VRAM(void);
uint16_t *GetVRAMAddress(void);
uint16_t *GetSecondaryVRAMAddress(void);

int Bdisp_FrameAndColor(int, int);
#define EnableColor(...)
#define EnableStatusArea(...)

//---
// Filesystem API emulation
//---

#include "../../../include/fxcg/file.h"

//---
// Other functions
//---

int PRGM_GetKey(void);
void GetKey(int *key);

int RTC_GetTicks(void);

void assert(int iLine,const char *pszFilename,const char *pszAassert);
#define ASSERT(x) if(!(x)){assert(__LINE__,__FILE__,#x);}

#define GetOSVersion() ("03.60.0000")

#endif /* PLATFORM_H */
