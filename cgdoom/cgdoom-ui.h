#ifndef CGDOOM_UI_H
#define CGDOOM_UI_H

#include "cgdoom.h"
#include <stdarg.h>
#include <stdint.h>

/* Basic rendering functions. */

enum {
    UI_LEFT = 0,
    UI_CENTER = 1,
    UI_RIGHT = 2,
};

/* Show a progress bar at the bottom of the screen. */
void UI_ProgressBar(int y, int current, int total);

/* Print text with a cleaner font. */
void UI_Print(int x, int y, int fg, int halign, char const *text, int length);
/* Like UI_Print, but applies printf() formatting. */
void UI_Printf(int x, int y, int fg, int halign, char const *fmt, ...);
void UI_Vprintf(int x, int y, int fg, int halign, char const *fmt, va_list args);

/* A simple immediate-mode layout system. */

typedef struct
{
    /* Reset each frame: current rendering y, number of focusable elements */
    int y, focus_count;
    /* Persistent: currently-focused element, non-zero if input in progress */
    int focus, active;
    /* Input text and cursor position */
    char text[16];
    int cursor;
    /* Properties for focusable elements */
    uint8_t focus_y[16];
    uint8_t focus_type[16];
    void *focus_data[16];
} Layout;

void Layout_Init(Layout *l);

void Layout_StartFrame(Layout *l);
void Layout_EndFrame(Layout *l);

/* Add a centered text. */
void Layout_CenteredText(Layout *l, const char *text);
/* Add a labeled field with printf-formatted contents. */
void Layout_Text(Layout *l, const char *label, const char *fmt, ...);
/* Add a checkbox. */
void Layout_Checkbox(Layout *l, const char *label, int *checked);
/* Add an integer input. */
void Layout_Integer(Layout *l, const char *label, int *value);
/* Add spacing. */
void Layout_Spacing(Layout *l, int spacing);

/* Handle a key press; returns non-zero if event is caught. */
int Layout_Event(Layout *l, int key);

/* Larger-scale functions. */

/* Show the program's main screen; returns index of selected WAD file. */
int UI_Main(WADFileInfo *wads, int wad_count,
    int *dev_info,        /* Enable technical detail screens */
    int *use_mmap,        /* Map file to memory (as opposed to using Bfile) */
    int *startmap,        /* Warp to this map */
    int *startepisode,    /* Warp to this episode */
    int *trustunaligned,  /* Trust unaligned lumps */
    int *autostart,       /* Skip title screen, straight to gameplay */
    int *enabledemos,     /* Enable demos on the title screen */
    int *enable2MBline    /* Enable memory past the 2MB line */
);

/* Show an error with custom formatting. */
void UI_Error(const char *fmt, va_list args);

/* Show a file save with a progress bar. */
void UI_DelayedWrites(CGD_DelayedFileWrite const *dfw, int count,
    int current_file, int current_amount);

#endif /* CGDOOM_UI_H */
