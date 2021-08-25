///////////////////////////////////////////////////////////////////////////////
// Keyboard input
//
// Instead of using libfxcg functions which only provide single-key analysis
// (GetKey, GetKeyWait, and PRGM_GetKey mainly), the following code uses a
// simple KEYSC driver. This is indirectly taken from gint code.
///////////////////////////////////////////////////////////////////////////////

#include "cgdoom.h"
#include "doomdef.h"

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

/* Current keyboard state. */
static KeyboardState st_prev={0}, st_now={0};

static void ScanKeyboard(KeyboardState state)
{
    volatile uint16_t *KEYSC = (void *)0xa44b0000;
    uint16_t *array = (void *)state;
    for(int i = 0; i < 6; i++) array[i] = KEYSC[i];
}

static int DoomKeyToKeycode(int key)
{
    switch(key)
    {
    /* Normal Doom keys */
    case KEY_LEFTARROW:     return KEYCODE_LEFT;
    case KEY_RIGHTARROW:    return KEYCODE_RIGHT;
    case KEY_UPARROW:       return KEYCODE_UP;
    case KEY_DOWNARROW:     return KEYCODE_DOWN;
    case ' ':               return KEYCODE_SQUARE;
    case KEY_RCTRL:         return KEYCODE_ALPHA;
    case '1':               return KEYCODE_F1;
    case '2':               return KEYCODE_F2;
    case '3':               return KEYCODE_F3;
    case '4':               return KEYCODE_F4;
    case '5':               return KEYCODE_F5;
    case '6':               return KEYCODE_F6;
    case '7':               return KEYCODE_SHIFT;
    case KEY_TAB:           return KEYCODE_EXIT;
    case KEY_PAUSE:         return KEYCODE_OPTN;
    case KEY_SLEFTARROW:    return KEYCODE_0;
    case KEY_SRIGHTARROW:   return KEYCODE_DOT;
    case KEY_ESCAPE:        return KEYCODE_MENU;
    case KEY_ENTER:         return KEYCODE_EXE;

    /* Special CGDoom keys */
    case SKEY_CHEAT:        return KEYCODE_POWER;
    case SKEY_DECVP:        return KEYCODE_MINUS;
    case SKEY_INCVP:        return KEYCODE_PLUS;
    case SKEY_NOCLIP:       return KEYCODE_ARROW;
    case SKEY_GAMMA:        return KEYCODE_FRAC;
    case SKEY_FREEMEM:      return KEYCODE_FD;
    case SKEY_FPSCOUNTER:   return KEYCODE_LEFTP;
    case SKEY_FRAMESKIP:    return KEYCODE_VARS;
    case SKEY_PROFILER:     return KEYCODE_RIGHTP;

    default:                return -1;
    }
}

static int KeyDown(KeyboardState state, int key)
{
    int code = DoomKeyToKeycode(key);
    if(code < 0) return 0;
    int row = (code >> 4) ^ 1;
    int col = 0x80 >> (code & 0x7);
    return (state[row] & col) != 0;
}

void UpdateKeyboardState(void)
{
	memcpy(st_prev, st_now, sizeof st_now);
	ScanKeyboard(st_now);
}

int KeyWasJustPressed(int key)
{
    return !KeyDown(st_prev, key) && KeyDown(st_now, key);
}

int KeyWasJustReleased(int key)
{
    return KeyDown(st_prev, key) && !KeyDown(st_now, key);
}
