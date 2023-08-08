#include "dev/concole.h"
#include "tools/klib.h"

#define CONSOLE_NR 1
static console_t console_buf[CONSOLE_NR];

/**
 * 擦除line行
 */
static void erase_rows(console_t* console, int start, int end)
{
    disp_char_t* disp_start = console->disp_base + console->disp_cols * start;
    disp_char_t* disp_end = console->disp_base + console->disp_cols * (end + 1);
    while (disp_start < disp_end) {
        disp_start->c = ' ';
        disp_start->foreground = console->foreground;
        disp_start->background = console->background;
        disp_start++;
    }
}

/**
 * 上滚
 */
static void scroll_up(console_t* console, int line)
{
    // 上移一行
    disp_char_t* dest = console->disp_base;
    disp_char_t* src = console->disp_base + console->disp_cols * line;
    uint32_t size = (console->disp_row - line) * console->disp_cols * sizeof(disp_char_t);
    kernel_memcpy((void*)dest, (void*)src, size);
    // 清空最后line行
    erase_rows(console, console->disp_row - line, console->disp_row - 1);
    console->cursor_row -= line;
}

static void move_cursor(console_t* console, int step)
{
    for (int i = 0; i < step; i++) {
        if (++console->cursor_col >= console->disp_cols) {
            console->cursor_row++;
            console->cursor_col = 0;

            if (console->cursor_row >= console->disp_row) {
                scroll_up(console, 1);
            }
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

static void move_to_col0(console_t* console)
{
    console->cursor_col = 0;
}

static void move_next_line(console_t* console)
{
    console->cursor_row++;
    if (console->cursor_row >= console->disp_row) {
        scroll_up(console, 1);
    }
}

/**
 * 清屏
 */
static void clear_display(console_t* console)
{
    int size = console->disp_cols * console->disp_row;
    disp_char_t* start = console->disp_base;
    for (int i = 0; i < size; i++, start++) {
        start->c = ' ';
        start->foreground = console->foreground;
        start->background = console->background;
    }
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

        clear_display(&console_buf[i]);
    }
    return 0;
}

int console_write(int console, char* data, int size)
{
    console_t* c = console_buf + console;
    int len;

    for (len = 0; len < size; len++) {
        char ch = *data++;
        switch (ch) {
        case '\n':
            move_to_col0(c);
            move_next_line(c);
            break;

        default:
            show_char(c, ch);
            break;
        }
    }
    return len;
}

void console_close(int console)
{
}