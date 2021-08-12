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



#include "os.h"

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

byte* I_ZoneBase (int*	size, int i)
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

void I_Error (char *error, ...)
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

///////////////////////////////////////////////////////////////////////////////
// Keyboard input
//
// Instead of using libfxcg functions which only provide single-key analysis
// (GetKey, GetKeyWait, and PRGM_GetKey mainly), the following code uses a
// simple KEYSC driver. This is indirectly taken from gint code.
///////////////////////////////////////////////////////////////////////////////

/* Keyboard matrix codes */

#define KEYCODE_F1      0x91
#define KEYCODE_F2      0x92
#define KEYCODE_F3      0x93
#define KEYCODE_F4      0x94
#define KEYCODE_F5      0x95
#define KEYCODE_F6      0x96

#define KEYCODE_SHIFT   0x81
#define KEYCODE_OPTN    0x82
#define KEYCODE_VARS    0x83
#define KEYCODE_MENU    0x84
#define KEYCODE_LEFT    0x85
#define KEYCODE_UP      0x86

#define KEYCODE_ALPHA   0x71
#define KEYCODE_SQUARE  0x72
#define KEYCODE_POWER   0x73
#define KEYCODE_EXIT    0x74
#define KEYCODE_DOWN    0x75
#define KEYCODE_RIGHT   0x76

#define KEYCODE_XOT     0x61
#define KEYCODE_LOG     0x62
#define KEYCODE_LN      0x63
#define KEYCODE_SIN     0x64
#define KEYCODE_COS     0x65
#define KEYCODE_TAN     0x66

#define KEYCODE_FRAC    0x51
#define KEYCODE_FD      0x52
#define KEYCODE_LEFTP   0x53
#define KEYCODE_RIGHTP  0x54
#define KEYCODE_COMMA   0x55
#define KEYCODE_ARROW   0x56

#define KEYCODE_7       0x41
#define KEYCODE_8       0x42
#define KEYCODE_9       0x43
#define KEYCODE_DEL     0x44

#define KEYCODE_4       0x31
#define KEYCODE_5       0x32
#define KEYCODE_6       0x33
#define KEYCODE_MUL     0x34
#define KEYCODE_DIV     0x35

#define KEYCODE_1       0x21
#define KEYCODE_2       0x22
#define KEYCODE_3       0x23
#define KEYCODE_PLUS    0x24
#define KEYCODE_MINUS   0x25

#define KEYCODE_0       0x11
#define KEYCODE_DOT     0x12
#define KEYCODE_EXP     0x13
#define KEYCODE_NEG     0x14
#define KEYCODE_EXE     0x15

#define KEYCODE_ACON    0x07

/* Copy of the keyboard state, one byte per row */
typedef uint8_t KeyboardState[12] __attribute__((aligned(2)));

void ScanKeyboard(KeyboardState state)
{
    volatile uint16_t *KEYSC = (void *)0xa44b0000;
    uint16_t *array = (void *)state;
    for(int i = 0; i < 6; i++) array[i] = KEYSC[i];
}

int KeycodeDown(KeyboardState state, int code)
{
    int row = (code >> 4) ^ 1;
    int col = 0x80 >> (code & 0x7);
    return (state[row] & col) != 0;
}

int KeycodePressed(KeyboardState old_state, KeyboardState new_state, int code)
{
    return !KeycodeDown(old_state, code) && KeycodeDown(new_state, code);
}

int KeycodeToDoomKey(int code)
{
    switch(code)
    {
    case KEYCODE_LEFT:      return KEY_LEFTARROW;
    case KEYCODE_RIGHT:     return KEY_RIGHTARROW;
    case KEYCODE_UP:        return KEY_UPARROW;
    case KEYCODE_DOWN:      return KEY_DOWNARROW;
    case KEYCODE_SQUARE:    return ' ';
    case KEYCODE_ALPHA:     return KEY_RCTRL;
    case KEYCODE_F1:        return '1';
    case KEYCODE_F2:        return '2';
    case KEYCODE_F3:        return '3';
    case KEYCODE_F4:        return '4';
    case KEYCODE_F5:        return '5';
    case KEYCODE_F6:        return '6';
    case KEYCODE_SHIFT:     return '7';
    case KEYCODE_EXIT:      return KEY_TAB;
    case KEYCODE_OPTN:      return KEY_PAUSE;
    case KEYCODE_0:         return KEY_SLEFTARROW;
    case KEYCODE_DOT:       return KEY_SRIGHTARROW;
    case KEYCODE_MENU:      return KEY_ESCAPE;
    case KEYCODE_EXE:       return KEY_ENTER;
    default:                return -1;
    }
}

void I_StartTic (void)
{
    static KeyboardState st = { 0 };

    extern boolean fpscounteractive;
    extern int fpscounter_data;
    extern int giRefreshMask;

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

    KeyboardState next_st;
    ScanKeyboard(next_st);

    /* Capture events for special keys */
    if (KeycodePressed(st, next_st, KEYCODE_POWER))
        CGCheat();
    if (KeycodePressed(st, next_st, KEYCODE_MINUS))
        R_SetViewSizeChange(-1);
    if (KeycodePressed(st, next_st, KEYCODE_PLUS))
        R_SetViewSizeChange(1);
    if (KeycodePressed(st, next_st, KEYCODE_ARROW))
        CGSwitchClip();
    if (KeycodePressed(st, next_st, KEYCODE_FRAC))
        CGCycleGamma();
    if (KeycodePressed(st, next_st, KEYCODE_FD))
        CGFreeMem();
    if (KeycodePressed(st, next_st, KEYCODE_LEFTP))
        fpscounteractive = !fpscounteractive;
    if (KeycodePressed(st, next_st, KEYCODE_VARS))
        CGRefreshSwitch();

    /* Emit events for all changes between st and next_st */
    for (int row = 0; row < 12; row++)
    {
        int changed = next_st[row] ^ st[row];
        if (!changed)
            continue;

        for(int code = ((row ^ 1) << 4) | 0x7; code & 0x7; code--)
        {
            if(changed & 1)
            {
                int key = KeycodeToDoomKey(code);
                if (key < 0)
                    continue;

                event_t e;
                e.type = KeycodeDown(next_st, code) ? ev_keydown : ev_keyup;
                e.data1 = key;
                D_PostEvent(&e);
            }
            changed >>= 1;
        }
    }

    memcpy(st, next_st, sizeof st);
}
