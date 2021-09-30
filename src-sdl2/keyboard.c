#include <SDL2/SDL.h>
#include <stdlib.h>

#include "cgdoom.h"
#include "cgdoom-kbd.h"
#include "doomdef.h"

/* Mapping of calculator matrix codes on the PC keyboard */
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

void CGD_ScanKeyboard(CGD_KeyboardState state)
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
