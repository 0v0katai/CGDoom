/* <platform.h> file for Linux/SDL2 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stddef.h>
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

enum
{
	BFILE_CREATEMODE_FILE = 1,
	BFILE_CREATEMODE_FOLDER = 5
};

enum
{
	BFILE_READ = 0,
	BFILE_READ_SHARE = 1,
	BFILE_WRITE = 2,
	BFILE_READWRITE = 3,
	BFILE_READWRITE_SHARE = 4
};

typedef struct tag_FILE_INFO
{
	unsigned short id;
	unsigned short type;
	unsigned long fsize;
	unsigned long datasize;
	unsigned int property;
	unsigned long address;
} Bfile_FileInfo;

int Bfile_CloseFile_OS(int HANDLE);
int Bfile_CreateEntry_OS(const unsigned short*filename, int mode, size_t *size);
int Bfile_DeleteEntry(const unsigned short *filename);
int Bfile_FindClose(int FindHandle);
int Bfile_FindFirst(const unsigned short *pathname, int *FindHandle, unsigned short *foundfile, void *fileinfo);
int Bfile_FindNext(int FindHandle, unsigned short *foundfile, void *fileinfo);
int Bfile_OpenFile_OS(const unsigned short *filename, int mode, int null);
int Bfile_ReadFile_OS(int HANDLE, void *buf, int size, int readpos);
int Bfile_WriteFile_OS(int HANDLE, const void *buf, int size);
void Bfile_NameToStr_ncpy(char* dest, const unsigned short* source, size_t n);
void Bfile_StrToName_ncpy(unsigned short *dest, const char *source, size_t n);
int Bfile_GetFileSize_OS(int handle);

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
