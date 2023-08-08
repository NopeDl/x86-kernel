#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"

#define CONSOLE_DISP_ADDR 0XB8000
#define CONSOLE_DISP_END (0XB8000 + 32 * 1024)
#define CONSOLE_ROW_MAX 25
#define CONSOLE_COL_MAX 80

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
    enum {
        CONSOLE_WRITE_NORMAL,
        CONSOLE_WRITE_ESC
    } write_status;

    disp_char_t* disp_base;
    int cursor_row, cursor_col;
    int disp_row, disp_cols;
    color_t foreground, background;

    int old_cursor_col, old_cursor_row;
} console_t;

int console_init();
int console_write(int console, char* data, int size);
void console_close(int console);

#endif