#include "comm/cpu_instr.h"
#include "dev/concole.h"
#include "tools/klib.h"

#define CONSOLE_NR 1
static console_t console_buf[CONSOLE_NR];

/**
 * 读取光标位置
 */
static int
read_cursor_pos()
{
    int pos;
    outb(0x3d4, 0xf);
    pos = inb(0x3b5);

    outb(0x3d4, 0xe);
    pos |= inb(0x3d5) << 8;
    return pos;
}

/**
 * 更新光标位置
 */
static int
update_cursor_pos(console_t* console)
{
    uint16_t pos
        = console->cursor_row * console->disp_cols + console->cursor_col;
    outb(0x3d4, 0xf);
    outb(0x3d5, (uint8_t)(pos & 0xff));
    outb(0x3d4, 0xe);
    outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
    return pos;
}

/**
 * 擦除line行
 */
static void
erase_rows(console_t* console, int start, int end)
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
static void
scroll_up(console_t* console, int line)
{
    // 上移一行
    disp_char_t* dest = console->disp_base;
    disp_char_t* src = console->disp_base + console->disp_cols * line;
    uint32_t size
        = (console->disp_row - line) * console->disp_cols * sizeof(disp_char_t);
    kernel_memcpy((void*)dest, (void*)src, size);
    // 清空最后line行
    erase_rows(console, console->disp_row - line, console->disp_row - 1);
    console->cursor_row -= line;
}

static void
move_cursor(console_t* console, int step)
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

static void
show_char(console_t* console, char c)
{
    int offset = console->cursor_col + console->cursor_row * console->disp_cols;
    disp_char_t* p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_cursor(console, 1);
}

static void
move_to_col0(console_t* console)
{
    console->cursor_col = 0;
}

static void
move_next_line(console_t* console)
{
    console->cursor_row++;
    if (console->cursor_row >= console->disp_row) {
        scroll_up(console, 1);
    }
}

/**
 * 清屏
 */
static void
clear_display(console_t* console)
{
    int size = console->disp_cols * console->disp_row;
    disp_char_t* start = console->disp_base;
    for (int i = 0; i < size; i++, start++) {
        start->c = ' ';
        start->foreground = console->foreground;
        start->background = console->background;
    }
}

/**
 * 左移n个位置
 */
static int
move_backword(console_t* console, int n)
{
    int status = -1;

    for (int i = 0; i < n; i++) {
        if (console->cursor_col > 0) {
            console->cursor_col--;
            status = 0;
        } else if (console->cursor_row > 0) {
            console->cursor_row--;
            console->cursor_col = console->disp_cols - 1;
            status = 0;
        }
    }

    return status;
}

/**
 * 往左擦除一个字符
 */
static void
erase_backword(console_t* console)
{
    if (move_backword(console, 1) == 0) {
        show_char(console, ' ');
        move_backword(console, 1);
    }
}

int console_init()
{
    for (int i = 0; i < CONSOLE_NR; i++) {
        console_buf[i].foreground = COLOR_WHITE;
        console_buf[i].background = COLOR_BLACK;
        console_buf[i].disp_cols = CONSOLE_COL_MAX;
        console_buf[i].disp_row = CONSOLE_ROW_MAX;
        console_buf[i].disp_base = (disp_char_t*)CONSOLE_DISP_ADDR
            + i * (CONSOLE_COL_MAX * CONSOLE_ROW_MAX);

        // 读取光标位置
        int cursor_pos = read_cursor_pos();
        // 转换光标位置为行和列
        console_buf[i].cursor_row = cursor_pos / console_buf[i].disp_cols;
        console_buf[i].cursor_col = cursor_pos % console_buf[i].disp_cols;

        console_buf[i].old_cursor_col = console_buf[i].cursor_col;
        console_buf[i].old_cursor_row = console_buf[i].cursor_row;

        console_buf[i].write_status = CONSOLE_WRITE_NORMAL;
        // clear_display(&console_buf[i]);
    }
    return 0;
}

static void
write_normal(console_t* c, char ch)
{
    switch (ch) {
    case ASCII_ESC:
        c->write_status = CONSOLE_WRITE_ESC;
        break;
    case '\n':
        move_to_col0(c);
        move_next_line(c);
        break;
    case 0x7f:
        erase_backword(c);
        break;
    case '\b':
        move_backword(c, 1);
        break;
    case '\r':
        move_to_col0(c);
        break;
    default:
        if ((ch >= ' ') && (ch <= '~')) {
            show_char(c, ch);
        }
        break;
    }
}

static void
save_cursor(console_t* console)
{
    console->old_cursor_col = console->cursor_col;
    console->old_cursor_row = console->cursor_row;
}

static void
restore_cursor(console_t* console)
{
    console->cursor_col = console->old_cursor_col;
    console->cursor_row = console->old_cursor_row;
}

static void
clear_esc_param(console_t* console)
{
    kernel_memset(console->esc_param, 0, sizeof(console->esc_param));
    console->cur_parm_idx = 0;
}

static void
set_font_style(console_t* console)
{
    static const color_t color_table[]
        = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
              COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE };

    for (int i = 0; i <= console->cur_parm_idx; i++) {
        int param = console->esc_param[i];
        if ((param >= 30) && (param <= 37)) {
            console->foreground = color_table[param - 30];
        } else if ((param >= 40) && (param <= 47)) {
            console->foreground = color_table[param - 40];
        } else if (param == 39) {
            console->foreground = COLOR_WHITE;
        } else if (param == 49) {
            console->background = COLOR_BLACK;
        }
    }
}

static void
write_esc(console_t* console, char ch)
{
    switch (ch) {
    case '7':
        save_cursor(console);
        console->write_status = CONSOLE_WRITE_NORMAL;
        break;
    case '8':
        restore_cursor(console);
        console->write_status = CONSOLE_WRITE_NORMAL;
        break;

    case '[':
        clear_esc_param(console);
        console->write_status = CONSOLE_WRITE_SQUARE;
        break;
    default:
        console->write_status = CONSOLE_WRITE_NORMAL;
        break;
    }
}

static void erase_in_display(console_t* console)
{
    if (console->cur_parm_idx < 0) {
        return;
    }

    int param = console->esc_param[0];
    if (param == 2) {
        erase_rows(console, 0, console->disp_cols - 1);
        console->cursor_col = console->cursor_row = 0;
    }
}

static void move_cursor(console_t* console)
{
    console->cursor_row = console->esc_param[0];
    console->cursor_col = console->esc_param[1];
}

static void move_left(console_t* console, int n)
{
    if (n == 0) {
        n == 1;
    }

    int col = console->cursor_col - n;
    console->cursor_col = (col >= 0) ? col : 0;
}

static void move_right(console_t* console, int n)
{
    if (n == 0) {
        n == 1;
    }

    int col = console->cursor_col + n;
    if (col >= console->disp_cols) {
        console->cursor_col = console->disp_cols - 1;
    } else {
        console->cursor_col = col;
    }
}

static void write_esc_square(console_t* console, char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        int* param = &console->esc_param[console->cur_parm_idx];
        *param = *param * 10 + ch - '0';
    } else if ((console->cur_parm_idx < ESC_PARAM_MAX) && (ch == ';')) {
        console->cur_parm_idx++;
    } else {
        switch (ch) {
        case 'm':
            set_font_style(console);
            break;
        case 'D':
            move_left(console, console->esc_param[0]);
            break;
        case 'C':
            move_right(console, console->esc_param[0]);
            break;
        case 'H':
        case 'f':
            move_cursor(console);
            break;
        case 'J':
            erase_in_display(console);
            break;
        default:
            break;
        }
        console->write_status = CONSOLE_WRITE_NORMAL;
    }
}

int console_write(int console, char* data, int size)
{
    console_t* c = console_buf + console;
    int len;

    for (len = 0; len < size; len++) {
        char ch = *data++;
        switch (c->write_status) {
        case CONSOLE_WRITE_NORMAL:
            write_normal(c, ch);
            break;
        case CONSOLE_WRITE_ESC:
            write_esc(c, ch);
            break;
        case CONSOLE_WRITE_SQUARE:
            write_esc_square(c, ch);
            break;
        default:
            break;
        }
    }
    update_cursor_pos(c);
    return len;
}

void console_close(int console)
{
}