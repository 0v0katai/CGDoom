#include <SDL2/SDL.h>
#include <stdlib.h>

#include "cgdoom.h"
#include "doomdef.h"

/* Copy of the keyboard state. */
static Uint8 *st_prev=NULL, *st_now=NULL;

static Uint8 *ScanKeyboard(void)
{
    int length;
    const Uint8 *state = SDL_GetKeyboardState(&length);
    return memcpy(malloc(length), state, length);
}

static int DoomKeyToKeycode(int key)
{
    switch(key)
    {
    /* Normal Doom keys */
    case KEY_LEFTARROW:     return SDL_SCANCODE_LEFT;
    case KEY_RIGHTARROW:    return SDL_SCANCODE_RIGHT;
    case KEY_UPARROW:       return SDL_SCANCODE_UP;
    case KEY_DOWNARROW:     return SDL_SCANCODE_DOWN;
    case ' ':               return SDL_SCANCODE_SPACE;
//    case KEY_RCTRL:         return KEYCODE_ALPHA;
    case '1':               return SDL_SCANCODE_1;
    case '2':               return SDL_SCANCODE_2;
    case '3':               return SDL_SCANCODE_3;
    case '4':               return SDL_SCANCODE_4;
    case '5':               return SDL_SCANCODE_5;
    case '6':               return SDL_SCANCODE_6;
    case '7':               return SDL_SCANCODE_7;
    case KEY_TAB:           return SDL_SCANCODE_TAB;
//    case KEY_PAUSE:         return KEYCODE_OPTN;
//    case KEY_SLEFTARROW:    return KEYCODE_0;
//    case KEY_SRIGHTARROW:   return KEYCODE_DOT;
    case KEY_ESCAPE:        return SDL_SCANCODE_ESCAPE;
    case KEY_ENTER:         return SDL_SCANCODE_RETURN;

    /* Special CGDoom keys */
/*    case SKEY_CHEAT:        return KEYCODE_POWER;
    case SKEY_DECVP:        return KEYCODE_MINUS;
    case SKEY_INCVP:        return KEYCODE_PLUS;
    case SKEY_NOCLIP:       return KEYCODE_ARROW;
    case SKEY_GAMMA:        return KEYCODE_FRAC;
    case SKEY_FREEMEM:      return KEYCODE_FD;
    case SKEY_FPSCOUNTER:   return KEYCODE_LEFTP;
    case SKEY_FRAMESKIP:    return KEYCODE_VARS; */

    default:                return -1;
    }
}

static int KeyDown(Uint8 *state, int key)
{
    int code = DoomKeyToKeycode(key);
    return (code < 0 || state == NULL) ? 0 : state[code];
}

void UpdateKeyboardState(void)
{
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT)
            exit(0);
    }

    free(st_prev);
    st_prev = st_now;
	st_now = ScanKeyboard();
}

int KeyWasJustPressed(int key)
{
    return !KeyDown(st_prev, key) && KeyDown(st_now, key);
}

int KeyWasJustReleased(int key)
{
    return KeyDown(st_prev, key) && !KeyDown(st_now, key);
}
