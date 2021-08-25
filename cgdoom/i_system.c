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
//
//-----------------------------------------------------------------------------



#include "platform.h"
#include "cgdoom.h"
#include <stdlib.h>

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"

#include "g_game.h"
#include "d_main.h"
#include "doomstat.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#define RAM_I_Zone_SIZE SYSTEM_STACK_SIZE

int 			contrast_change= 0;
int 			map_mode= 0;
unsigned short 	timer_def_one;
byte			timer_def_two;
byte*			mb_mainzone;


ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
    return RAM_I_Zone_SIZE;
}

#ifdef CG_EMULATOR

/* On the native build, simply reserve a large enough region of heap. */
byte* I_ZoneBase (int*	size, int i)
{
    if (i == 0)
    {
        *size = (8 << 20);
        return malloc(*size);
    }
    return NULL;
}

byte *I_ScreenBase(int screen)
{
    if (screen == 0)
        return malloc(320 * 200);
    if (screen == 1)
        return malloc(320 * 200);
    if (screen == 4)
        return malloc(320 * 32);

    return NULL;
}

#else /* fx-CG build */

/* On the calculator, scrap every possible bit of RAM */
byte* I_ZoneBase (int*  size, int i)
{
    if (i == 0)
    {
        /* System stack (~500 kiB) */
        *size = RAM_I_Zone_SIZE;
        mb_mainzone = (byte *) SystemStack;
        ASSERT(RAM_I_Zone_SIZE <= SYSTEM_STACK_SIZE);
        return mb_mainzone;
    }
    if (i == 1)
    {
        /* Unused part of user stack (~200 kiB); provided by linker script */
        extern byte sextra, eextra;
        *size = &eextra - &sextra;
        return &sextra;
    }
    #ifdef CGDOOM_DIRECT_R61524
    if (i == 2)
    {
        /* Secondary VRAM (screen buffers being in the main VRAM because we're
           doing direct access so we don't need Bdisp_PutDisp_DD() */
        *size = 384 * 216 * 2;
        return SaveVRAMBuffer;
    }
    #endif
    return NULL;
}

byte *I_ScreenBase(int screen)
{
    /* The primary/secondary VRAM covers 384*216*2 = 162 kiB:
       -> Give 320*200 = 64 kB to screens[0] (main display)
       -> Give 320*200 = 64 kB to screens[1] (intermissions)
       -> Give 320*32  = 10 kiB to screens[4] (status bar)
       -> 27 kiB are left */
    if (screen == 0)
        return CGDOOM_SCREENS_BASE;
    if (screen == 1)
        return CGDOOM_SCREENS_BASE + 320*200;
    if (screen == 4)
        return CGDOOM_SCREENS_BASE + 320*200*2;

    return NULL;
}

#endif

//
// I_Init
// Initialize machine state
//
void I_Init (void)
{
}


//
// I_Restore
// Restore machine state (if not done, the calculator will be killed once it enters sleep mode after the game quit)
//
void I_Restore (void)
{
}


//
// I_GetTime
// returns time in 1/70th second tics
int  I_GetTime (void)
{
    //Returns the RTC-basecount in units of 1/128 s. 
    //1/70 ~ 2/128 +-troleybus 
    return (RTC_GetTicks()/2);
}

//
// I_Quit
//
void I_Quit (void)
{
    //M_SaveDefaults ();
    fuck= true;
}


//
// toupper_int
//
int toupper_int(int i) 
{
    if(('a' <= i) && (i <= 'z'))
        return 'A' + (i - 'a');
    else
        return i;
}
//
// I_Error
//
static int key = 31;
void I_ErrorI (const char *error, int i1,int i2,int i3,int i4)
{
  #ifndef CG_EMULATOR

    char buf[21];
    Bdisp_AllClr_VRAM();
    locate_OS( 1, 1 );
    PrintLine( "ERROR:", 21 );
    locate_OS( 1, 2 );
    PrintLine( error, 21 );
    locate_OS( 1, 3 );CGDAppendHex32("i1:",i1,8,buf);PrintLine( buf, 21 );
    locate_OS( 1, 4 );CGDAppendHex32("i2:",i2,8,buf);PrintLine( buf, 21 );
    locate_OS( 1, 5 );CGDAppendHex32("i3:",i3,8,buf);PrintLine( buf, 21 );
    locate_OS( 1, 6 );CGDAppendHex32("i4:",i4,8,buf);PrintLine( buf, 21 );
    GetKey( &key );
    I_ReinitAfterError();
#else
      I_Error (error,i1,i2,i3,i4);
#endif
}

void I_Error (const char *error, ...)
{
#ifdef CG_EMULATOR
    /*// Message first.
    va_start (argptr,error);
    printf ("Error: ");
    printf (error,argptr);
    printf ("\n");
    sprintf(ferror,error,argptr);
    va_end (argptr);
    */
    printf ("Error: %s",error);
    //Inform the user something bad had occured
    //Exit function here
#else
  I_ShutdownGraphics();
    Bdisp_AllClr_VRAM();
    locate_OS( 1, 1 );
    PrintLine( "ERROR:", 21 );
    locate_OS( 1, 2 );
    PrintLine( error, 21 );
    GetKey( &key );
    I_ReinitAfterError();
#endif //#ifdef CG_EMULATOR
    //I_Quit();
}

//
// I_StartTic
// Processes events
// Is a bit strange because a PC handles events synchronously, while the Nspire asynchronously
//
void R_SetViewSize( int		blocks);

static void R_SetViewSizeChange(int iDiff)
{
  static int iViewSize = 10;
  iViewSize += iDiff;
  if(iViewSize >= 1 && iViewSize <= 10)
  {
    R_SetViewSize (iViewSize);
  }
  else
  {
    iViewSize -= iDiff;
  }
}

void I_StartTic (void)
{
    extern int giRefreshMask;

    //---
    // FPS counter
    //---

    extern boolean fpscounteractive;
    extern int fpscounter_data;

    /* Number of frames since FPS count started */
    static int fps_frames = 0;
    /* RTC time when the FPS count started */
    static int fps_ticks = -1;

    if(fps_ticks < 0)
        fps_ticks = RTC_GetTicks();

    if(++fps_frames >= 16)
    {
        int now = RTC_GetTicks();
        if (now == fps_ticks)
            fpscounter_data = -1;
        else
            fpscounter_data = 128 * fps_frames / ((now - fps_ticks) * (giRefreshMask + 1));

        fps_ticks = now;
        fps_frames = 0;
    }

    //---
    // Special keys
    //---

    UpdateKeyboardState();

    /* Capture events for special keys */
    if (KeyWasJustPressed(SKEY_CHEAT))
        CGCheat();
    if (KeyWasJustPressed(SKEY_DECVP))
        R_SetViewSizeChange(-1);
    if (KeyWasJustPressed(SKEY_INCVP))
        R_SetViewSizeChange(1);
    if (KeyWasJustPressed(SKEY_NOCLIP))
        CGSwitchClip();
    if (KeyWasJustPressed(SKEY_GAMMA))
        CGCycleGamma();
    if (KeyWasJustPressed(SKEY_FREEMEM))
        CGFreeMem();
    if (KeyWasJustPressed(SKEY_FPSCOUNTER))
        fpscounteractive = !fpscounteractive;
    if (KeyWasJustPressed(SKEY_FRAMESKIP))
        CGRefreshSwitch();

    //---
    // Profiler
    //---

    /* Number of ticks left before reporting results */
    static int profiler_ticks = -1;

    if (KeyWasJustPressed(SKEY_PROFILER)) {
        prof_t *counters = (prof_t *)&CGD_Perf;
        for(int i = 0; i < sizeof CGD_Perf / sizeof (prof_t); i++) {
            counters[i] = prof_make();
        }
        profiler_ticks = 40;
    }
    else {
        profiler_ticks--;
        if(profiler_ticks == 0) {
            CGProfilerResults();
            profiler_ticks = -1;
        }
    }

    //---
    // Normal keys
    //---

    static int keys[] = {
        KEY_LEFTARROW, KEY_RIGHTARROW, KEY_UPARROW, KEY_DOWNARROW,
        KEY_RCTRL, KEY_TAB, KEY_PAUSE, KEY_SLEFTARROW, KEY_SRIGHTARROW,
        KEY_ESCAPE, KEY_ENTER,
        ' ', '1', '2', '3', '4', '5', '6', '7',
        0
    };

    /* Emit events for all changes */
    for(int i = 0; keys[i]; i++)
    {
        event_t e;
        if(KeyWasJustPressed(keys[i]))
            e.type = ev_keydown;
        else if(KeyWasJustReleased(keys[i]))
            e.type = ev_keyup;
        else continue;

        e.data1 = keys[i];
        D_PostEvent(&e);
    }
}
