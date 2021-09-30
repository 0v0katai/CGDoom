#ifndef CGDOOM_KBD_H
#define CGDOOM_KBD_H

/* CGDoom's keyboard abstraction

   Instead of using libfxcg functions which only provide single-key analysis
   (GetKey, GetKeyWait, and PRGM_GetKey mainly), CGDoom uses a simple KEYSC
   driver adapter from gint's. The following matrix codes identify all keys on
   the keyboard. The PC build maps a subset of them to PC keys. */

/* Copy of the keyboard state, one byte per row */
typedef uint8_t CGD_KeyboardState[12] __attribute__((aligned(2)));

/* Keyboard scanning function (very short), uses either the KEYSC or SDL */
void CGD_ScanKeyboard(CGD_KeyboardState state);

/* Default keymaps for use in UI code */
extern const uint16_t CGD_Keymap_CGDoom_0_3[];
extern const uint16_t CGD_Keymap_ThumbsOnly[];
extern const uint16_t CGD_Keymap_FullHands[];

/* Load a default keymap */
void CGD_LoadKeymap(const uint16_t *template);


/* Keyboard matrix codes for the keyboard driver */

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

#endif /* CGDOOM_KBD_H */
