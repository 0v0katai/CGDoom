#include "cgdoom.h"
#include "cgdoom-kbd.h"
#include "platform.h"

#include "doomdef.h"
#include "i_system.h"

char _PRAM0[160*1024];

/* Rendering system emulation. */

uint16_t _VRAM[WIDTH * HEIGHT];
uint16_t _VRAM2[WIDTH * HEIGHT];

extern int ecall_no;

uint16_t *GetVRAMAddress(void)
{
	return _VRAM;
}

uint16_t *GetSecondaryVRAMAddress(void)
{
	return _VRAM2;
}

void Bdisp_PutDisp_DD(void)
{
	ecall_no =
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
		rgb888[0] = b;
		rgb888[1] = g;
		rgb888[2] = r;
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
	memset(_VRAM, 0xff, WIDTH * HEIGHT * 2);
}

/* Keyboard system emulation. */

static int MatrixCodeToPCKey(int key)
{
    switch(key)
    {
    case KEYCODE_F1:        return SDL_SCANCODE_F1;
    case KEYCODE_F2:        return SDL_SCANCODE_F2;
    case KEYCODE_F3:        return SDL_SCANCODE_F3;
    case KEYCODE_F4:        return SDL_SCANCODE_F4;
    case KEYCODE_F5:        return SDL_SCANCODE_F5;
    case KEYCODE_F6:        return SDL_SCANCODE_F6;

    case KEYCODE_UP:        return SDL_SCANCODE_UP;
    case KEYCODE_RIGHT:     return SDL_SCANCODE_RIGHT;
    case KEYCODE_DOWN:      return SDL_SCANCODE_DOWN;
    case KEYCODE_LEFT:      return SDL_SCANCODE_LEFT;

    case KEYCODE_SHIFT:     return SDL_SCANCODE_Q;
    case KEYCODE_OPTN:      return SDL_SCANCODE_W;
    case KEYCODE_VARS:      return SDL_SCANCODE_E;
    case KEYCODE_MENU:      return SDL_SCANCODE_R;

    case KEYCODE_ALPHA:     return SDL_SCANCODE_A;
    case KEYCODE_SQUARE:    return SDL_SCANCODE_S;
    case KEYCODE_POWER:     return SDL_SCANCODE_D;
    case KEYCODE_EXIT:      return SDL_SCANCODE_F;

    case KEYCODE_XOT:       return SDL_SCANCODE_T;
    case KEYCODE_LOG:       return SDL_SCANCODE_Y;
    case KEYCODE_LN:        return SDL_SCANCODE_U;
    case KEYCODE_SIN:       return SDL_SCANCODE_I;
    case KEYCODE_COS:       return SDL_SCANCODE_O;
    case KEYCODE_TAN:       return SDL_SCANCODE_P;

    case KEYCODE_FRAC:      return SDL_SCANCODE_G;
    case KEYCODE_FD:        return SDL_SCANCODE_H;
    case KEYCODE_LEFTP:     return SDL_SCANCODE_J;
    case KEYCODE_RIGHTP:    return SDL_SCANCODE_K;
    case KEYCODE_COMMA:     return SDL_SCANCODE_L;
    case KEYCODE_ARROW:     return SDL_SCANCODE_SEMICOLON;

    case KEYCODE_1:         return SDL_SCANCODE_1;
    case KEYCODE_2:         return SDL_SCANCODE_2;
    case KEYCODE_3:         return SDL_SCANCODE_3;
    case KEYCODE_4:         return SDL_SCANCODE_4;
    case KEYCODE_5:         return SDL_SCANCODE_5;
    case KEYCODE_6:         return SDL_SCANCODE_6;
    case KEYCODE_7:         return SDL_SCANCODE_7;
    case KEYCODE_8:         return SDL_SCANCODE_8;
    case KEYCODE_9:         return SDL_SCANCODE_9;
    case KEYCODE_0:         return SDL_SCANCODE_0;

    case KEYCODE_DEL:       return SDL_SCANCODE_BACKSPACE;
    case KEYCODE_ACON:      return SDL_SCANCODE_ESCAPE;

    case KEYCODE_MUL:       return SDL_SCANCODE_LEFTBRACKET;
    case KEYCODE_DIV:       return SDL_SCANCODE_RIGHTBRACKET;

    case KEYCODE_PLUS:      return SDL_SCANCODE_MINUS;
    case KEYCODE_MINUS:     return SDL_SCANCODE_EQUALS;

    case KEYCODE_DOT:       return -1;
    case KEYCODE_EXP:       return -1;
    case KEYCODE_NEG:       return -1;

    case KEYCODE_EXE:       return SDL_SCANCODE_RETURN;

    default:                return -1;
    }
}

void CGD_ScanKeyboard(CGDoom_KeyboardState state)
{
    SDL_Event e;
    while(SDL_PollEvent(&e)) {}

    int length;
    const Uint8 *sdl_state = SDL_GetKeyboardState(&length);

    for(int row = 0; row < 12; row++) {
        for(int col = 0; col < 8; col++) {
            int matrix_code = (row << 4) | col;
            int index = row ^ 1;
            int pattern = (0x80 >> col);

            int scancode = MatrixCodeToPCKey(matrix_code);
            state[index] &= ~pattern;
            if(scancode >= 0 && scancode < length && sdl_state[scancode])
                state[index] |= pattern;
        }
    }
}

int PRGM_GetKey(void)
{
	// TODO: PRGM_GetKey()
	// We don't really need it here, it's only used to delay error messages
	// on-calc, and these are printed on terminal
	return 0;
}

void GetKey(int *key)
{
	SDL_Event e;
	int valid=0, sym;

	while(!valid)
	{
		SDL_WaitEvent(&e);
		if(e.type == SDL_QUIT ||
		        (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			exit(0);

		if(e.type != SDL_KEYDOWN) continue;

		valid = 1;
		switch((sym = e.key.keysym.sym))
		{
		case SDLK_UP:     *key = KEY_CTRL_UP;     break;
		case SDLK_RIGHT:  *key = KEY_CTRL_RIGHT;  break;
		case SDLK_DOWN:   *key = KEY_CTRL_DOWN;   break;
		case SDLK_LEFT:   *key = KEY_CTRL_LEFT;   break;
		case SDLK_RETURN: *key = KEY_CTRL_EXE;    break;
		case SDLK_F1:     *key = KEY_CTRL_F1;     break;
		case SDLK_F2:     *key = KEY_CTRL_F2;     break;
		case SDLK_F3:     *key = KEY_CTRL_F3;     break;
		case SDLK_F4:     *key = KEY_CTRL_F4;     break;
		case SDLK_F5:     *key = KEY_CTRL_F5;     break;
		case SDLK_F6:     *key = KEY_CTRL_F6;     break;

		default:
			if(sym >= '0' && sym <= '9') *key = sym;
			else valid = 0;
		}
	}
}


/* Returns the value of a 128 Hz counter */
int RTC_GetTicks(void)
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	int msec = (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000);
	return (msec * 128) / 1000;
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

//---
// Filesystem API emulation
//---

#define FILE_TABLE_MAX 16
static FILE *file_table[FILE_TABLE_MAX] = { NULL };
static struct {
	int pos;
	glob_t glob;
} search_table[FILE_TABLE_MAX];

void Bfile_NameToStr_ncpy(char *dest, const uint16_t *source, size_t n)
{
	size_t i = 0;

	if(!memcmp(source, u"\\\\fls0\\", 14))
		source += 7;

	while(i < n && source[i] != 0x0000 && source[i] != 0xffff) {
		dest[i] = source[i];
		i++;
	}
	for(size_t j = i; j < n; j++)
		dest[j] = source[i];
}

void Bfile_StrToName_ncpy(uint16_t *dest, const char *source, size_t n)
{
	if(!strncmp(source, "\\\\fls0\\", 7)) {
		memcpy(dest, u"\\\\fls0\\", 14);
		dest += 7;
	}

	size_t i = 0;
	while(i < n && source[i]) {
		dest[i] = source[i];
		i++;
	}
	while(i < n)
		dest[i++] = 0;
}

int Bfile_DeleteEntry(const uint16_t *filename_u16)
{
	char filename_u8[1024];
	Bfile_NameToStr_ncpy(filename_u8, filename_u16, 1024);
	printf("Deleting %s (virtually)\n", filename_u8);
	return 0;
}

int Bfile_CreateEntry_OS(const uint16_t *filename_u16, int mode, size_t *size)
{
	char filename_u8[1024];
	Bfile_NameToStr_ncpy(filename_u8, filename_u16, 1024);

	if(mode == BFILE_CREATEMODE_FILE) {
		printf("Creating %s with size %zu (virtually)\n", filename_u8, *size);
	}
	else if(mode == BFILE_CREATEMODE_FOLDER) {
		printf("Cannot create folder %s: Not Implemented\n", filename_u8);
		return -1;
	}
	else {
		printf("Bfile_CreateEntry_OS(): Invalid mode %d\n", mode);
		return -1;
	}

	return 0;
}

int Bfile_OpenFile_OS(const uint16_t *filename_u16, int mode, int zero)
{
	(void)zero;
	char filename_u8[1024];
	Bfile_NameToStr_ncpy(filename_u8, filename_u16, 1024);

	char const *bits;
	if(mode == BFILE_READ || mode == BFILE_READ_SHARE)
		bits = "rb";
	else if(mode == BFILE_WRITE)
		bits = "wb";
	else if(mode == BFILE_READWRITE || mode == BFILE_READWRITE_SHARE)
		bits = "w+b";
	else {
		printf("Bfile_OpenFile_OS(): invalid mode %d for %s\n", mode, filename_u8);
		return -1;
	}

	FILE *fp = fopen(filename_u8, bits);
	if(!fp) {
		printf("Bfile_OpenFile_OS(): cannot open %s: %m\n", filename_u8);
		return -1;
	}

	/* Find open slot in file table */
	int slot = 0;
	while(slot < FILE_TABLE_MAX && file_table[slot] != NULL)
		slot++;
	if(slot >= FILE_TABLE_MAX) {
		printf("Bfile_OpenFile_OS(): cannot open %s, table is full\n",filename_u8);
		fclose(fp);
		return -1;
	}

	printf("Bfile_OpenFile_OS(): opened %s\n", filename_u8);
	file_table[slot] = fp;
	return slot;
}

int Bfile_GetFileSize_OS(int fd)
{
	FILE *fp = file_table[fd];
	long pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return size;
}

int Bfile_SeekFile_OS(int fd, int pos)
{
	fseek(file_table[fd], pos, SEEK_SET);
	return 0;
}

int Bfile_ReadFile_OS(int fd, void *buf, int size, int readpos)
{
  if(readpos != -1)
    Bfile_SeekFile_OS(fd, readpos);

  return fread(buf, 1, size, file_table[fd]);
}

int Bfile_WriteFile_OS(int fd, const void *buf, int size)
{
	return fwrite(buf, 1, size, file_table[fd]);
}

int Bfile_CloseFile_OS(int fd)
{
	fclose(file_table[fd]);
	file_table[fd] = NULL;
	return 0;
}

int Bfile_FindFirst(const uint16_t *pattern_u16, int *fd, uint16_t *found,
  void *fileinfo)
{
	char pattern_u8[1024];
	Bfile_NameToStr_ncpy(pattern_u8, pattern_u16, 1024);

	int slot = 0;
	while(slot < FILE_TABLE_MAX && search_table[slot].pos != 0)
		slot++;
	if(slot >= FILE_TABLE_MAX) {
		printf("Bfile_FindFirst(): cannot search %s, table is full\n", pattern_u8);
		*fd = -1;
		return -16;
	}

	int rc = glob(pattern_u8, 0, NULL, &search_table[slot].glob);

	printf("Bfile_FindFirst(): Searching %s: %zu results\n", pattern_u8,
		(rc == GLOB_NOMATCH) ? 0 : search_table[slot].glob.gl_pathc);

	if(rc == GLOB_NOMATCH)
		return -16;

	*fd = slot;
	return Bfile_FindNext(slot, found, fileinfo);
}

int Bfile_FindNext(int fd, uint16_t *found, void *fileinfo0)
{
	int *pos = &search_table[fd].pos;
	glob_t *glob = &search_table[fd].glob;

	if(*pos >= glob->gl_pathc)
		return -16;

	const char *name = glob->gl_pathv[*pos];
	Bfile_StrToName_ncpy(found, name, strlen(name)+1);
	(*pos)++;

	Bfile_FileInfo *fileinfo = fileinfo0;
	// TODO: More resonsable Bfile_FileInfo entries?
	memset(fileinfo, 0, sizeof *fileinfo);

	FILE *fp = fopen(name, "rb");
	if(fp) {
		fseek(fp, 0, SEEK_END);
		fileinfo->fsize = ftell(fp);
		fclose(fp);
	}

	return 0;
}

int Bfile_FindClose(int fd)
{
	if(fd < 0 || fd >= FILE_TABLE_MAX)
		return -1;

	search_table[fd].pos = 0;
	globfree(&search_table[fd].glob);
	return 0;
}
