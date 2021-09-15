#include "platform.h"

#define SYSTEM_STACK_SAFE (64*1024)
#define USER_STACK_SAFE (32*1024)

// SaveVRAMBuffer 165888 bytes
#define SAVE_VRAM_SIZE (WIDTH*HEIGHT*2-3)
extern unsigned char *SaveVRAMBuffer;

// system stack (512 kB).
#define SYSTEM_STACK_SIZE (512*1024-SYSTEM_STACK_SAFE)
extern unsigned char *SystemStack;

extern unsigned short *VRAM;
void * CGDMalloc(int iSize);
void * CGDCalloc(int iSize);
void * CGDRealloc (void *p, int iSize);
void D_DoomMain();

void CGDAppendNum09(const char *pszText,int iNum,char *pszBuf);
int CGDstrlen(const char *pszText);
void CGDstrcpy(char *pszBuf,const char *pszText);
int CGDstrcmp (const char*s1,const char*s2);
int CGDstrncmp (const char*s1,const char*s2,int iLen);
int CGDstrnicmp (const char*s1,const char*s2,int iLen);
void CGDstrncpy(char *pszBuf,const char *pszText,int iLen);

void CGDAppendNum0_999(const char *pszText,int iNum,int iMinDigits,char *pszBuf);
void CGDAppendHex32(const char *pszText,int iNum, int iDigits,char *pszBuf);

int abs(int x);

void I_Error (const char *error, ...);

//force compiler error on use following:
#define strcpy 12
#define strnicmp 22
#define strncmp 27
#define strcmp 33

//return ptr to flash
int FindInFlash(const void **buf, int size, int readpos);
//direct read from flash
int Flash_ReadFile(void *buf, int size, int readpos);

#define min(x,y) ({ \
    __auto_type __x = (x); \
    __auto_type __y = (y); \
    __x < __y ? __x : __y; \
})
