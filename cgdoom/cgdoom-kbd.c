#include "cgdoom.h"
#include "cgdoom-kbd.h"
#include "doomdef.h"

//---
// Keymaps
//---

/* List of Doom keys in their order of assignment in keymaps */
int16_t CGD_KeymapEntries[CGDOOM_KEYMAP_SIZE] = {
    KEY_LEFTARROW,
    KEY_RIGHTARROW,
    KEY_UPARROW,
    KEY_DOWNARROW,
    ' ',                     /* Open doors etc */
    KEY_RCTRL,               /* Shoot */
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    KEY_TAB,
    KEY_PAUSE,
    KEY_SLEFTARROW,
    KEY_SRIGHTARROW,
    KEY_RSHIFT,              /* Speed toggle */

    /* Special CGDoom keys */
    SKEY_DECVP,
    SKEY_INCVP,
    SKEY_CHEAT,
    SKEY_NOCLIP,
    SKEY_GAMMA,
    SKEY_FREEMEM,
    SKEY_FPSCOUNTER,
    SKEY_FRAMESKIP,
    SKEY_PROFILER,
};

/* Default keymap: CGDoom 0.03 */
const CGDoom_Keymap CGD_Keymap_CGDoom_0_03 = {
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
    -1,
    KEYCODE_MINUS,
    KEYCODE_PLUS,
    KEYCODE_POWER,
    KEYCODE_ARROW,
    KEYCODE_FRAC,
    KEYCODE_FD,
    KEYCODE_LEFTP,
    KEYCODE_VARS,
    KEYCODE_RIGHTP,
};
/* Default keymap: thumbs only */
const CGDoom_Keymap CGD_Keymap_ThumbsOnly = {
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
};
/* Default keymap: full-hands */
const CGDoom_Keymap CGD_Keymap_FullHands = {
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
};

void CGD_CopyKeymap(CGDoom_Keymap dest, const CGDoom_Keymap src)
{
    memcpy(dest, src, sizeof(CGDoom_Keymap));
}

static int DecodeKeymap(const CGDoom_Keymap map, int key)
{
    for(int i = 0; i < CGDOOM_KEYMAP_SIZE; i++) {
        if(CGD_KeymapEntries[i] == key)
            return map[i];
    }
    return -1;
}

//---
// Physical keyboard driver
//---

static CGDoom_KeyboardState st_prev = { 0 };
static CGDoom_KeyboardState st_now = { 0 };

#ifndef CG_EMULATOR
void CGD_ScanKeyboard(CGDoom_KeyboardState state)
{
    volatile uint16_t *KEYSC = (void *)0xa44b0000;
    uint16_t *array = (void *)state;
    for(int i = 0; i < 6; i++) array[i] = KEYSC[i];
}
#endif

int KeycodeDown(CGDoom_KeyboardState state, int code)
{
    if(code < 0) return 0;
    int row = (code >> 4) ^ 1;
    int col = 0x80 >> (code & 0x7);
    return (state[row] & col) != 0;
}

int CGD_PRGM_Getkey(void)
{
    CGDoom_KeyboardState st;
    CGD_ScanKeyboard(st);

    for(int code_y = 9; code_y >= 1; code_y--) {
        for(int code_x = 1; code_x <= 6 - (code_y < 5); code_x++) {
            int code = (code_y << 4) | code_x;
            if(code == 0x45) code = KEYCODE_ACON;

            if(KeycodeDown(st, code))
                return code;
        }
    }
    return -1;
}

//---
// Doom keys
//---

static int DoomKeyToKeycode(int key)
{
    int code = DecodeKeymap(CGD_Options.Keymap, key);
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

static int KeyDown(CGDoom_KeyboardState state, int key)
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

void CGD_UpdateKeyboardState(void)
{
	memcpy(st_prev, st_now, sizeof st_now);
	CGD_ScanKeyboard(st_now);
}

int CGD_KeyWasJustPressed(int key)
{
    return !KeyDown(st_prev, key) && KeyDown(st_now, key);
}

int CGD_KeyWasJustReleased(int key)
{
    return KeyDown(st_prev, key) && !KeyDown(st_now, key);
}

//---
// Key names and identification
//---

static const char * const display_keynames[] = {
    "F1",      "F2",      "F3",      "F4",      "F5",      "F6",
    "SHIFT",   "OPTN",    "VARS",    "MENU",    "Left",    "Up",
    "ALPHA",   "x^2",     "^",       "EXIT",    "Down",    "Right",
    "X,O,T",   "log",     "ln",      "sin",     "cos",     "tan",
    "Frac",    "S<>D",    "(",       ")",       ",",       "->",
    "7",       "8",       "9",       "DEL",     "AC/ON",   "(None)",
    "4",       "5",       "6",       "x",       "/",       NULL,
    "1",       "2",       "3",       "+",       "-",       NULL,
    "0",       ".",       "x10^x",   "(-)",     "EXE",     NULL,
};
static const char * const technical_keynames[] = {
    "F1",      "F2",      "F3",      "F4",      "F5",      "F6",
    "SHIFT",   "OPTN",    "VARS",    "MENU",    "LEFT",    "UP",
    "ALPHA",   "SQUARE",  "POWER",   "EXIT",    "DOWN",    "RIGHT",
    "XOT",     "LOG",     "LN",      "SIN",     "COS",     "TAN",
    "FRAC",    "FD",      "LEFTP",   "RIGHTP",  "COMMA",   "ARROW",
    "7",       "8",       "9",       "DEL",     "ACON",    "NONE",
    "4",       "5",       "6",       "MUL",     "DIV",     NULL,
    "1",       "2",       "3",       "PLUS",    "MINUS",   NULL,
    "0",       "DOT",     "EXP",     "NEG",     "EXE",     NULL,
};

static const char *KeyName(int key, const char * const *names)
{
    if(key == KEYCODE_ACON)
        key = 0x45;
    if(key <= 0)
        key = 0x46;

    int row = 9 - (key >> 4);
    int col = (key & 0xf) - 1;
    return names[6*row + col];
}

const char *CGD_PhysicalKey_DisplayName(int key)
{
    return KeyName(key, display_keynames);
}

const char *CGD_PhysicalKey_TechnicalName(int key)
{
    return KeyName(key, technical_keynames);
}

int CGD_PhysicalKey_FromTechnicalName(const char *name, int length)
{
    int i, n = sizeof(technical_keynames) / sizeof(technical_keynames[0]);
    if(length < 0)
        length = strlen(name);

    for(i = 0; i < n; i++) {
        const char *t = technical_keynames[i];
        if(t && !strncmp(name, t, length)) break;
    }

    int row = 9 - (i / 6);
    int col = (i % 6) + 1;
    int key = (row << 4) | col;

    if(i >= n || key == 0x46)
        key = -1;
    if(key == 0x45)
        key = KEYCODE_ACON;

    return key;
}

struct DoomkeyInfo {
    int key;
    const char *technical_name;
    const char *display_name;
};

static const struct DoomkeyInfo doomkeys[] = {
    { KEY_LEFTARROW,     "Left",                "Look left"         },
    { KEY_RIGHTARROW,    "Right",               "Look right"        },
    { KEY_UPARROW,       "Up",                  "Move forward"      },
    { KEY_DOWNARROW,     "Down",                "Move backward"     },
    { ' ',               "Inteact",             "Interact"          },
    { KEY_RCTRL,         "Shoot",               "Shoot"             },
    { '1',               "Weapon1",             "1-Hand"            },
    { '2',               "Weapon2",             "2-Pistol"          },
    { '3',               "Weapon3",             "3-Shotgun"         },
    { '4',               "Weapon4",             "4-Chaingun"        },
    { '5',               "Weapon5",             "5-Rockets"         },
    { '6',               "Weapon6",             "6-Plasma"          },
    { '7',               "Weapon7",             "7-BFG9000"         },
    { KEY_TAB,           "Map",                 "Map"               },
    { KEY_PAUSE,         "Pause",               "Pause"             },
    { KEY_SLEFTARROW,    "StrafeLeft",          "Strafe left"       },
    { KEY_SRIGHTARROW,   "StrafeRight",         "Strafe right"      },
    { KEY_RSHIFT,        "Run",                 "Toggle run"        },
    { SKEY_DECVP,        "DecreaseViewport",    "Smaller view"      },
    { SKEY_INCVP,        "IncreaseViewport",    "Larger view"       },
    { SKEY_CHEAT,        "Cheat",               "Cheat"             },
    { SKEY_NOCLIP,       "ToggleNoclip",        "Noclip"            },
    { SKEY_GAMMA,        "GammaCorrection",     "Brightness"        },
    { SKEY_FREEMEM,      "HeapStatistics",      "Heap stats"        },
    { SKEY_FPSCOUNTER,   "FPSCounter",          "FPS counter"       },
    { SKEY_FRAMESKIP,    "CycleFrameskip",      "Frameskip"         },
    { SKEY_PROFILER,     "Profiler",            "Run profiler"      },
    { -1,                NULL,                  NULL                },
};

const char *CGD_DoomKey_DisplayName(int key)
{
    for(int i = 0; doomkeys[i].key >= 0; i++) {
        if(doomkeys[i].key == key)
            return doomkeys[i].display_name;
    }
    return "(None)";
}

const char *CGD_DoomKey_TechnicalName(int key)
{
    for(int i = 0; doomkeys[i].key >= 0; i++) {
        if(doomkeys[i].key == key)
            return doomkeys[i].technical_name;
    }
    return "NONE";
}

int CGD_DoomKey_FromTechnicalName(const char *name, int length)
{
    if(length < 0)
        length = strlen(name);

    for(int i = 0; doomkeys[i].key >= 0; i++) {
        if(!strncmp(doomkeys[i].technical_name, name, length))
            return doomkeys[i].key;
    }
    return -1;
}
