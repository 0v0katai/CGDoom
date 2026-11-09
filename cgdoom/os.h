#include "platform.h"

#define SYSTEM_STACK_SAFE (80*1000)
#define USER_STACK_SAFE (32*1024)

// system stack (512 kB).
#define SYSTEM_STACK_SIZE (512*1000-SYSTEM_STACK_SAFE)
extern unsigned char *SystemStack;

void *CGD_malloc(int size);
void *CGD_calloc(int size);
void *CGD_realloc(void *p, int size);

int strnicmp (const char*s1,const char*s2,int iLen);

void I_Error (const char *error, ...);

//return ptr to flash
int (FindInFlash)(const void **buf, int size, int readpos);
//direct read from flash
int Flash_ReadFile(void *buf, int size, int readpos);
