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

/* Keymap */
static uint16_t Keymap[] = {
    KEY_LEFTARROW,          0,
    KEY_RIGHTARROW,         0,
    KEY_UPARROW,            0,
    KEY_DOWNARROW,          0,
    ' ',                    0, /* Open doors etc */
    KEY_RCTRL,              0, /* Shoot */
    '1',                    0,
    '2',                    0,
    '3',                    0,
    '4',                    0,
    '5',                    0,
    '6',                    0,
    '7',                    0,
    KEY_TAB,                0,
    KEY_PAUSE,              0,
    KEY_SLEFTARROW,         0,
    KEY_SRIGHTARROW,        0,
    KEY_RSHIFT,             0, /* Speed toggle */
    /* Special CGDoom keys */
    SKEY_DECVP,             0,
    SKEY_INCVP,             0,
    SKEY_CHEAT,             0,
    SKEY_NOCLIP,            0,
    SKEY_GAMMA,             0,
    SKEY_FREEMEM,           0,
    SKEY_FPSCOUNTER,        0,
    SKEY_FRAMESKIP,         0,
    SKEY_PROFILER,          0,
    0,
};

/* Default keymap: CGDoom 0.3 */
static const uint16_t KeyMap_CGDoom_0_3[] = {
    KEYCODE_LEFT,
    KEYCODE_RIGHT,
    KEYCODE_UP,
    KEYCODE_DOWN,
    KEYCODE_EXE,
    KEYCODE_ALPHA,
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_F3,
    KEYCODE_F4,
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_SHIFT,
    KEYCODE_EXIT,
    KEYCODE_OPTN,
    KEYCODE_0,
    KEYCODE_DOT,
    KEYCODE_MINUS,
    KEYCODE_PLUS,
    KEYCODE_POWER,
    KEYCODE_ARROW,
    KEYCODE_FRAC,
    KEYCODE_FD,
    KEYCODE_LEFTP,
    KEYCODE_VARS,
    KEYCODE_RIGHTP,
    0,
};
/* Default keymap: thumbs only */
static const uint16_t KeyMap_ThumbsOnly[] = {
    KEYCODE_LEFT,
    KEYCODE_RIGHT,
    KEYCODE_UP,
    KEYCODE_DOWN,
    KEYCODE_SQUARE,
    KEYCODE_ALPHA,
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_F3,
    KEYCODE_F4,
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_SHIFT,
    KEYCODE_EXIT,
    KEYCODE_OPTN,
    KEYCODE_LOG,
    KEYCODE_LN,
    KEYCODE_XOT,
    KEYCODE_MINUS,
    KEYCODE_PLUS,
    KEYCODE_COMMA,
    KEYCODE_ARROW,
    KEYCODE_FRAC,
    KEYCODE_FD,
    KEYCODE_LEFTP,
    KEYCODE_VARS,
    KEYCODE_RIGHTP,
    0,
};
/* Default keymap: full-hands */
static const uint16_t KeyMap_FullHands[] = {
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_OPTN,
    KEYCODE_SQUARE,
    KEYCODE_MUL,
    KEYCODE_DEL,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_TAN,
    KEYCODE_ACON,
    KEYCODE_ALPHA,
    KEYCODE_POWER,
    KEYCODE_COS,
    KEYCODE_MINUS,
    KEYCODE_PLUS,
    KEYCODE_COMMA,
    KEYCODE_ARROW,
    KEYCODE_FRAC,
    KEYCODE_FD,
    KEYCODE_LEFTP,
    KEYCODE_SIN,
    KEYCODE_RIGHTP,
    0,
};

/* Current keyboard state. */
static KeyboardState st_prev={0}, st_now={0};

static void ScanKeyboard(KeyboardState state)
{
    volatile uint16_t *KEYSC = (void *)0xa44b0000;
    uint16_t *array = (void *)state;
    for(int i = 0; i < 6; i++) array[i] = KEYSC[i];
}

static void LoadKeymap(uint16_t *map, uint16_t *template)
{
    for(int i = 0; map[2*i] != 0; i++) {
        map[2*i+1] = template[i];
    }
}

static int DecodeKeymap(uint16_t *map, int key)
{
    for(int i = 0; map[2*i] != 0; i++) {
        if(map[2*i] == key)
            return map[2*i+1];
    }
    return -1;
}

static int DoomKeyToKeycode(int key)
{
    int code = DecodeKeymap(Keymap, key);
    if(code != -1)
        return code;

    switch(key)
    {
    /* Interface constants */
    case KEY_ESCAPE:        return KEYCODE_MENU;
    case KEY_ENTER:         return KEYCODE_EXE;

    /* Alphabetic keys */
    case 'A':               return KEYCODE_XOT;
    case 'B':               return KEYCODE_LOG;
    case 'C':               return KEYCODE_LN;
    case 'D':               return KEYCODE_SIN;
    case 'E':               return KEYCODE_COS;
    case 'F':               return KEYCODE_TAN;
    case 'G':               return KEYCODE_FRAC;
    case 'H':               return KEYCODE_FD;
    case 'I':               return KEYCODE_LEFTP;
    case 'J':               return KEYCODE_RIGHTP;
    case 'K':               return KEYCODE_COMMA;
    case 'L':               return KEYCODE_ARROW;
    case 'M':               return KEYCODE_7;
    case 'N':               return KEYCODE_8;
    case 'O':               return KEYCODE_9;
    case 'P':               return KEYCODE_4;
    case 'Q':               return KEYCODE_5;
    case 'R':               return KEYCODE_6;
    case 'S':               return KEYCODE_MUL;
    case 'T':               return KEYCODE_DIV;
    case 'U':               return KEYCODE_1;
    case 'V':               return KEYCODE_2;
    case 'W':               return KEYCODE_3;
    case 'X':               return KEYCODE_PLUS;
    case 'Y':               return KEYCODE_MINUS;
    case 'Z':               return KEYCODE_0;
    case -' ':              return KEYCODE_DOT;
    case KEY_BACKSPACE:     return KEYCODE_DEL;

    /* Numeric keys (handled differently, require a modifier) */
    case -'0':              return KEYCODE_0;
    case -'1':              return KEYCODE_1;
    case -'2':              return KEYCODE_2;
    case -'3':              return KEYCODE_3;
    case -'4':              return KEYCODE_4;
    case -'5':              return KEYCODE_5;
    case -'6':              return KEYCODE_6;
    case -'7':              return KEYCODE_7;
    case -'8':              return KEYCODE_8;
    case -'9':              return KEYCODE_9;

    default:                return -1;
    }
}

static int KeycodeDown(KeyboardState state, int code)
{
    if(code < 0) return 0;
    int row = (code >> 4) ^ 1;
    int col = 0x80 >> (code & 0x7);
    return (state[row] & col) != 0;
}

static int KeyDown(KeyboardState state, int key)
{
    int code = DoomKeyToKeycode(key);

    /* For alphanumeric keys, watch the status of ALPHA */
    if(key >= -'9' && key <= -'0')
        return KeycodeDown(state, code) && KeycodeDown(state, KEYCODE_ALPHA);
    else if((key >= 'A' && key <= 'Z') || key == -' ')
        return KeycodeDown(state, code) && !KeycodeDown(state, KEYCODE_ALPHA);
    else
        return KeycodeDown(state, code);
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
