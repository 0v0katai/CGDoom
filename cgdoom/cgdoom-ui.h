#ifndef CGDOOM_UI_H
#define CGDOOM_UI_H

#include "platform.h"
#include <stdarg.h>

/* Basic rendering functions. */

enum {
    UI_LEFT = 0,
    UI_CENTER = 1,
    UI_RIGHT = 2,
};

/* Show a progress bar of file mapping at the bottom of the screen. */
void UI_FileMappingProgressBar(int size_mapped, int size_total);

/* Print text with a cleaner font. */
void UI_Print(int x, int y, int fg, int halign, char const *text);
/* Like UI_Print, but applies printf() formatting. */
void UI_Printf(int x, int y, int fg, int halign, char const *fmt, ...);
void UI_Vprintf(int x, int y, int fg, int halign, char const *fmt, va_list args);

/* A simple immediate-mode layout system. */

typedef struct
{
    int y, focus, focus_count;
    int16_t focus_y[16];
} Layout;

void Layout_Init(Layout *l);

void Layout_StartFrame(Layout *l);
void Layout_EndFrame(Layout *l);

/* Add a centered text. */
void Layout_CenteredText(Layout *l, const char *text);
/* Add a labeled field with printf-formatted contents. */
void Layout_Text(Layout *l, const char *label, const char *fmt, ...);
/* Add a checkbox. */
void Layout_Checkbox(Layout *l, const char *label, int checked);
/* Add spacing. */
void Layout_Spacing(Layout *l, int spacing);

/* Handle a key press; returns non-zero if event is caught. */
int Layout_Event(Layout *l, int key);

/* Larger-scale functions. */

/* Show the program's main screen; returns index of selected WAD file. */
int UI_Main(WADFileInfo *wads, int wad_count, int *dev_info, int *use_mmap);

#endif /* CGDOOM_UI_H */
