#ifndef SCREEN_H
#define SCREEN_H

#include "../libc/stdint.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F
#define RED_ON_WHITE 0xF4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

/* Public kernel API */
void clear_screen();
void printf(const char* format, ...);
void printf_backspace();

#endif