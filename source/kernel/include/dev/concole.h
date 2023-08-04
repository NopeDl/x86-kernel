#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"

#define CONSOLE_DISP_ADDR 0XB8000
#define CONSOLE_DISP_END (0XB8000 + 32 * 1024)
#define CONSOLE_ROW_MAX 25
#define CONSOLE_COL_MAX 80

typedef enum {
    COLOR_BLACK = 0,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_GRAY,
    COLOR_DARKGRAY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE
} color_t;

typedef union _disp_char_t {
    uint32_t v;
    struct
    {
        char c;
        char foreground : 4;
        char background : 3;
    };

} disp_char_t;

typedef struct _concole_t {
    disp_char_t* disp_base;
    int cursor_row, cursor_col;
    int disp_row, disp_cols;
    color_t foreground, background;
} console_t;

int console_init();
int console_write(int console, char* data, int size);
void console_close(int console);

#endif