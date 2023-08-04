#include "dev/concole.h"

#define CONSOLE_NR 1
static console_t console_buf[CONSOLE_NR];

static void move_cursor(console_t* console, int step)
{
    for (int i = 0; i < step; i++) {
        if (++console->cursor_col >= console->disp_cols) {
            console->cursor_row++;
            console->cursor_col = 0;
        }
    }
}

static void show_char(console_t* console, char c)
{
    int offset = console->cursor_col + console->cursor_row * console->disp_cols;
    disp_char_t* p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_cursor(console, 1);
}

int console_init()
{
    for (int i = 0; i < CONSOLE_NR; i++) {
        console_buf[i].foreground = COLOR_WHITE;
        console_buf[i].background = COLOR_BLACK;
        console_buf[i].cursor_col = console_buf[i].cursor_row = 0;
        console_buf[i].disp_cols = CONSOLE_COL_MAX;
        console_buf[i].disp_row = CONSOLE_ROW_MAX;
        console_buf[i].disp_base = (disp_char_t*)CONSOLE_DISP_ADDR + i * (CONSOLE_COL_MAX * CONSOLE_ROW_MAX);
    }
    return 0;
}

int console_write(int console, char* data, int size)
{
    console_t* c = console_buf + console;
    int len;

    for (len = 0; len < size; len++) {
        char ch = *data++;

        show_char(c, ch);
    }
    return len;
}

void console_close(int console)
{
}