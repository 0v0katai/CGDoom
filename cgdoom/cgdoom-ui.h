#ifndef CGDOOM_UI_H
#define CGDOOM_UI_H

#include "platform.h"

/* Show the program's main screen; returns index of selected WAD file. */
int UI_Main(char const wad_files[][32], int wad_file_count, int *dev_info);

/* Show a progress bar of file mapping. */
void UI_FileMappingProgressBar(int size_mapped, int size_total);

#endif /* CGDOOM_UI_H */
