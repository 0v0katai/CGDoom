#include "cgdoom.h"
#include "platform.h"
#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Surface *VRAM_RGB888 = NULL;

/* Rendering system emulation. */

uint16_t _VRAM[WIDTH * HEIGHT];

static void QuitSDL(void)
{
	if(window)
		SDL_DestroyWindow(window);
	if(VRAM_RGB888)
		SDL_FreeSurface(VRAM_RGB888);
	SDL_Quit();
}

static void InitSDL(void)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		I_Error("InitSDL: %s", SDL_GetError());
		return;
	}

	window = SDL_CreateWindow("CGDoom",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, 0);

	VRAM_RGB888 = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 24, 0, 0, 0, 0);
	if(!VRAM_RGB888) {
		I_Error("InitSDL: Cannot create intermediate surface: %s",
			SDL_GetError());
		QuitSDL();
		return;
	}

	atexit(QuitSDL);
}

uint16_t *GetVRAMAddress(void)
{
	return _VRAM;
}

void Bdisp_PutDisp_DD(void)
{
	if(!window)
		InitSDL();
	if(!window || !VRAM_RGB888)
		return;

	/* Copy from VRAM to RGB888 and let SDL handle the conversion to the
	   window's native format */
	for(int y = 0; y < HEIGHT; y++)
	for(int x = 0; x < WIDTH; x++)
	{
		int rgb565 = VRAM[WIDTH * y + x];
	    int r = (rgb565 >> 8) & 0xf8;
	    int g = (rgb565 >> 3) & 0xfc;
	    int b = (rgb565 << 3) & 0xf8;

	    Uint8 *rgb888 = VRAM_RGB888->pixels + y * VRAM_RGB888->pitch + 3 * x;
	    rgb888[0] = r;
	    rgb888[1] = g;
	    rgb888[2] = b;
	}

	SDL_BlitSurface(VRAM_RGB888, NULL, SDL_GetWindowSurface(window), NULL);
	SDL_UpdateWindowSurface(window);
}

int Bdisp_FrameAndColor(int p1, int p2)
{
	(void)p1;
	(void)p2;
	return 0;
}

void Bdisp_AllClr_VRAM(void)
{
	memset(_VRAM, 0, WIDTH * HEIGHT * 2);
}

/* Keyboard system emulation. */

void GetKey(int *key)
{
	SDL_Event e;
	int valid=0, sym;

	while(!valid)
	{
		SDL_WaitEvent(&e);
		if(e.type != SDL_KEYDOWN) continue;

		valid = 1;
		switch((sym = e.key.keysym.sym))
		{
		case SDL_SCANCODE_UP:     *key = KEY_CTRL_UP;
		case SDL_SCANCODE_RIGHT:  *key = KEY_CTRL_RIGHT;
		case SDL_SCANCODE_DOWN:   *key = KEY_CTRL_DOWN;
		case SDL_SCANCODE_LEFT:   *key = KEY_CTRL_LEFT;
		case SDL_SCANCODE_RETURN: *key = KEY_CTRL_EXE;

		default:
			if(sym >= '0' && sym <= '9') *key = sym;
			else valid = 0;
		}
	}
}

/* Main function */

int main(int argc,char *argv[])
{
}


//Returns the RTC-basecount in units of 1/128 s. 
int RTC_GetTicks(  void  )
{
	return  GetTickCount()>> 3;//1000/128 is very near to 1000/125
}

//start_value has to be set with RTC_GetTicks.
//duration_in_ms is the duration, which has to elapse since start_value.
//returns 0 if duration_in_ms has not elapsed yet
//returns 1 if duration_in_ms has elapsed

int RTC_Elapsed_ms( int start_value, int duration_in_ms )
{
	return (GetTickCount() - duration_in_ms) > (start_value << 3);
}

void assert(int iLine,const char *pszFilename,const char *pszAassert)
{
	static int iDoAssert = 1;
	printf("assert %s,%i: %s\n",pszFilename,iLine,pszAassert);
	if(iDoAssert)
	{
		iDoAssert = 0;
	}
}

//files
//int hFile = Bfile_OpenFile_OS(pFileName,0);
int Bfile_OpenFile_OS( const unsigned short*filename, int mode )
{
	FILE *f;
	char szFileName[1024];
	int i = 0;
	filename += 7;//\\fls0\ 
	while(filename[0])
	{
		szFileName[i] = (char)filename[0];
		filename++;i++;
	}
	szFileName[i] = 0;
	f = fopen(szFileName,"rb");
	return f?(int)f:-1;
}

int Bfile_SeekFile_OS( int HANDLE, int pos )
{
	fseek((FILE*)HANDLE,pos,SEEK_SET);
	return 0;
}

int Bfile_ReadFile_OS( int HANDLE, void *buf, int size, int readpos )
{
  if(readpos != -1)
  {
    Bfile_SeekFile_OS(HANDLE, readpos );
  }
  //fread ( dest, size elements, count elements, FILE handle );
  return fread(buf,1,size,(FILE*)HANDLE);
}

int Bfile_CloseFile_OS( int HANDLE )
{
	fclose((FILE*)HANDLE);
	return 0;
}
