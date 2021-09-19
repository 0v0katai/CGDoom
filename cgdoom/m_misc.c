// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// $Log:$
//
// DESCRIPTION:
//	Main loop menu stuff.
//
//-----------------------------------------------------------------------------

//static const char 

#include "os.h"

#include "doomdef.h"

#include "z_zone.h"

#include "m_swap.h"
//#include "m_argv.h"

#include "w_wad.h"

#include "i_system.h"
#include "i_video.h"
#include "v_video.h"

#include "hu_stuff.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"

#include "m_misc.h"

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern const patch_t* hu_font[HU_FONTSIZE];

/*
int M_DrawText ( int x, int y, boolean direct, char* string )
{
    int 	c;
    int		w;

    while (*string)
    {
		c = toupper_int(*string) - HU_FONTSTART;
		string++;
		if (c < 0 || c> HU_FONTSIZE)
		{
			x += 4;
			continue;
		}
			
		w = SHORT (hu_font[c]->width);
		if (x+w > SCREENWIDTH)
			break;
		if (direct)
			V_DrawPatchDirect(x, y, 0, hu_font[c]);
		else
			V_DrawPatch(x, y, 0, hu_font[c]);
		x+=w;
    }
    return x;
}

*/


//
// M_ReadFile
//
int M_ReadFile(const char *name, byte **buffer)
{
    uint16_t fc_path[100] = u"\\\\fls0\\";
    int j = 7;
    int fd;

    for (int i = 0; name[i]; i++)
        fc_path[j++] = name[i];
    fc_path[j++] = 0x0000;

    fd = Bfile_OpenFile_OS(fc_path, BFILE_READ, 0);
    if (fd < 0) {
        I_Error("Bfile_OpenFile_OS(%s): %d", name, fd);
        return -1;
    }

    int size = Bfile_GetFileSize_OS(fd);
    if (size <= 0) {
        I_Error("Bfile_GetFileSize_OS(%s): %d", name, size);
        Bfile_CloseFile_OS(fd);
        return -1;
    }

    void *buf = Z_Malloc(size, PU_STATIC, NULL);
    if (!buf) {
        Bfile_CloseFile_OS(fd);
        return -1;
    }

    Bfile_ReadFile_OS(fd, buf, size, -1);
    Bfile_CloseFile_OS(fd);

    *buffer = buf;
    return size;
}

//
// DEFAULTS
//
extern int	key_right;
extern int	key_left;
extern int	key_up;
extern int	key_down;

extern int	key_strafeleft;
extern int	key_straferight;

extern int	key_fire;
extern int	key_use;
extern int	key_strafe;
extern int	key_speed;

extern int	viewwidth;
extern int	viewheight;

extern int	showMessages;

extern int	detailLevel;

extern int	screenblocks;

extern int	showMessages;

int		numdefaults;
char*	defaultfile;
