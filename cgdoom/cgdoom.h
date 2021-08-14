#ifndef CGDOOM_H
#define CGDOOM_H

/* CGDoom-specific definitions that cover both calculator and native builds. */

#include <stdint.h>

/* VRAM pointer and size */
extern uint16_t *VRAM;
#define WIDTH  384
#define HEIGHT 216

/* Description of a WAD file selectable by user. */
typedef struct
{
    char name[16];
    uint16_t path[23];
    int size;
} WADFileInfo;

// TODO: Move developer info here

/* Global variables interfacing with Doom itself. */

/* Map and episode to start at when loading the game. */
extern int startmap, startepisode;

#endif /* CGDOOM_H */
