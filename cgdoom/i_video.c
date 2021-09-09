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
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for Nspire
//
//-----------------------------------------------------------------------------

#include "os.h"
#include "cgdoom.h"

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "d_main.h"

#include "doomdef.h"

/* Current palette data (after gamma transform) */
static uint16_t gPaletteData[256];
/* Reference to current palette (before gamma transform) */
static byte *gPalette = NULL;

void I_SetPalette(byte* pal)
{
  /* If pal=NULL, just regenerate the current palette to account for changes
     in the gamma level */
  if (pal == NULL)
    pal = gPalette;
  else
    gPalette = pal;

  extern int usegamma;
  const byte *gamma = gammatable[usegamma % 5];

  for (int i = 0; i < 256; i++)
  {
    int r = pal[3*i];
    int g = pal[3*i+1];
    int b = pal[3*i+2];

    r = gamma[r] >> 3;
    g = gamma[g] >> 2;
    b = gamma[b] >> 3;
    gPaletteData[i] = (r << 11) | (g << 5) | b;
  }
}

#if !defined(CG_EMULATOR) && defined(CGDOOM_DIRECT_R61524)

#define SYNCO() __asm__ volatile("synco":::"memory");
#define PRDR *(volatile uint8_t *)0xA405013C
#define LCDC *(volatile uint16_t *)0xB4000000

#define LCD_GRAM_X 0x200
#define LCD_GRAM_Y 0x201
#define LCD_GRAM 0x202
#define LCD_WINDOW_LEFT 0x210
#define LCD_WINDOW_RIGHT 0x211
#define LCD_WINDOW_TOP 0x212
#define LCD_WINDOW_BOTTOM 0x213

static void SelectLCDReg(unsigned short reg)
{
   PRDR &= ~0x10;
   SYNCO();
   LCDC = reg;
   SYNCO();
   PRDR |= 0x10;
   SYNCO();
}

static void WriteLCDReg(unsigned short reg, unsigned short value)
{
   SelectLCDReg(reg);
   LCDC = value;
}

static void SetWindow(int left, int top, int width, int height)
{
  WriteLCDReg(LCD_WINDOW_LEFT, left);
  WriteLCDReg(LCD_WINDOW_TOP, top);
  WriteLCDReg(LCD_WINDOW_RIGHT, left + width - 1);
  WriteLCDReg(LCD_WINDOW_BOTTOM, top + height - 1);
}

/* Whether the black background has been rendered. We could fill the screen
   at initialization but it would flicker. Instead, we go the extra mile and
   render the background at the same time as the first frame. This also allows
   redrawing it after erros.*/
static int firstflipdone = 0;

void I_Flip (void)
{
  prof_enter(CGD_Perf.DisplayInterface);

  if (!firstflipdone)
  {
    SetWindow(0, 0, 396, 224);
    WriteLCDReg(LCD_GRAM_X, 0);
    WriteLCDReg(LCD_GRAM_Y, 0);
    SelectLCDReg(LCD_GRAM);

    /* Clear 12 lines, render VRAM, then clear 12 */
    for (int n = 0; n < 396 * 12; n++)
      LCDC = 0x0000;

    byte *scr = screens[0];
    for (int y = 0; y < SCREENHEIGHT; y++)
    {
      for(int x = 0; x < 38; x++)
        LCDC = 0x0000;
      for(int x = 0; x < SCREENWIDTH; x++)
        LCDC = gPaletteData[*scr++];
      for(int x = 0; x < 38; x++)
        LCDC = 0x0000;
    }

    for (int n = 0; n < 396 * 12; n++)
      LCDC = 0x0000;

    /* Set the future window */
    SetWindow((396 - SCREENWIDTH) / 2, (224 - SCREENHEIGHT) / 2,
      SCREENWIDTH, SCREENHEIGHT);
    firstflipdone = 1;
  }
  else
  {
    WriteLCDReg(LCD_GRAM_X, 0);
    WriteLCDReg(LCD_GRAM_Y, 0);
    SelectLCDReg(LCD_GRAM);

    unsigned j=0, i=320*200;
    while(i--)
    {
      LCDC = gPaletteData[screens[0][j]];
      j++;
    }
  }

  prof_leave(CGD_Perf.DisplayInterface);
}

void I_InitGraphics(void)
{	
  usegamma = 0;
}

void I_ShutdownGraphics(void)
{
  SetWindow(6, 0, 384, 216);
}

void I_ReinitAfterError(void)
{
    firstflipdone = 0;
    /* Redraw status bar over help message */
    extern boolean st_firsttime;
    st_firsttime = true;
}

#else /* fx-CG 50 without direct-DD access */

void I_Flip (void)
{
  prof_enter(CGD_Perf.DisplayInterface);

  /* Set a black frame if the current one is white */
  if ((Bdisp_FrameAndColor(0, 0) & 1) == 0)
  {
    Bdisp_FrameAndColor(3, 17);
    Bdisp_FrameAndColor(1, 0);
  }

  int x,y;
  for(y=0;y<SCREENHEIGHT;y++)
  {
    for(x=0;x<SCREENWIDTH;x++)
    {
      VRAM[(x+(WIDTH-SCREENWIDTH)/2)+(y+(HEIGHT-SCREENHEIGHT)/2)*WIDTH] =
        gPaletteData[screens[0][x+y*SCREENWIDTH]];
    }
  }
  Bdisp_PutDisp_DD();

  prof_leave(CGD_Perf.DisplayInterface);
}

void I_InitGraphics(void)
{
  usegamma = 0;
}

void I_ShutdownGraphics(void)
{
}

void I_SetWindowAfterError(void)
{
}

#endif


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
  memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}
