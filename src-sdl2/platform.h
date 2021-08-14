#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "keyboard.hpp"

/* On the emulator all lumps are allocated with malloc(), so there are not
   pointers-to-flash to account for when freeing. */
#define PTR_TO_FLASH(p) 0

/* Rendering system emulation. */

// Emulated fx-CG function
void Bdisp_PutDisp_DD(void);
void Bdisp_AllClr_VRAM(void);
uint16_t *GetVRAMAddress(void);

// Additional SDL-related calls
void EmuInitGraphics(void);
void EmuShutdownGraphics(void);
void EmuUpdateWindow(void);

/* Input system emulation. */

// TODO

//~

void G3A_main( void );
int Serial_Open( unsigned char *mode );
int Serial_Close( int mode );
int Serial_DirectTransmitOneByte( unsigned char byte_to_transmit );
int Serial_CheckRX();
unsigned char Serial_Read();

#define dbgprint(x) printf(x)
#define dbgprint1(x,y) printf(x,y)


int RTC_GetTicks(  void  );
int RTC_Elapsed_ms( int start_value, int duration_in_ms );

void Sleep(int dwMilliseconds     );

#define EmulHack_Sleep(x) Sleep(x)

void assert(int iLine,const char *pszFilename,const char *pszAassert);
#define ASSERT(x) if(!(x)){assert(__LINE__,__FILE__,#x);}


int Bfile_OpenFile_OS( const unsigned short*filename, int mode );
int Bfile_SeekFile_OS( int HANDLE, int pos );
int Bfile_ReadFile_OS( int HANDLE, void *buf, int size, int readpos );
int Bfile_CloseFile_OS( int HANDLE );


extern const unsigned char *gpcFlashBuffer;

void InitFlashSimu(const char *filename);

