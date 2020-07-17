#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/memory.h"
#include "../libc/string.h"

/* Declaration of private functions */
int get_cursor_offset();
void set_cursor_offset(int offset);
int putchar(char str);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

void clear_screen() 
{
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = (char*)VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) 
    {
        screen[i * 2 + 0] = ' ';
        screen[i * 2 + 1] = WHITE_ON_BLACK;
    }

    set_cursor_offset(get_offset(0, 0));
}

/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 */
void printf(const char* format, ...) 
{
    uint8_t** arg = (uint8_t**) &format;
    uint8_t c;
    uint8_t buf[20];

    arg++;

    while((c = *format++) != 0) 
    {
        if (c != '%')
        {
            putchar(c);
        }
        else 
        {
            uint8_t *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0') 
            {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9') 
            {
                pad = c - '0';
                c = *format++;
            }

            switch (c) 
            {
            case 'X':
            case 'x':
                putchar('0');
                putchar('x');
            case 'd':
            case 'u':
                itoa (buf, c, *((int *) arg++));
                p = buf;
                goto string;
            case 's':
                p = *arg++;
                if (!p)
                    p = (uint8_t*)"(null)";
            string:
                for (p2 = p; *p2; p2++);
                for (; p2 < p + pad; p2++)
                    putchar((char)(pad0 ? '0' : ' '));
                while (*p)
                    putchar((char)(*p++));
                break;
            default:
                putchar((char)(*((int*) arg++)));
                break;
            }
        }
    }
}

void printf_backspace() 
{
    putchar((char)0x08);
}

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/

int putchar(char str) 
{
    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;

    int cursor = get_cursor_offset();
    int row = get_offset_row(cursor);
    int col = get_offset_col(cursor);

    if(str == '\n')
    {
        cursor = get_offset(0, row + 1);
    }
    else if(str == '\r')
    {
        cursor = get_offset(0, row);
    }
    else if(str == '\t')
    {
        cursor = get_offset(col + 4, row);
    }
    else if (str == 0x08) /* Backspace */
    {
        cursor -= 2;
        vidmem[cursor] = ' ';
        vidmem[cursor + 1] = WHITE_ON_BLACK;
    }
    else
    {
        vidmem[cursor] = str;
        vidmem[cursor + 1] = WHITE_ON_BLACK;
        cursor = cursor + 2;
    }

    /* Check if the offset is over screen size and scroll */
    if (cursor >= MAX_ROWS * MAX_COLS * 2) 
    {
        int i;

        for (i = 1; i < MAX_ROWS; i++)
        {
            memory_copy((char*)(VIDEO_ADDRESS + get_offset(0, i)),
                        (char*)(VIDEO_ADDRESS + get_offset(0, i - 1)),
                        MAX_COLS * 2);
        }

        /* Blank last line */
        char *last_line = (char*)(VIDEO_ADDRESS + get_offset(0, MAX_ROWS - 1));
        for (i = 0; i < MAX_COLS * 2; i++) 
            last_line[i] = 0;

        cursor -= 2 * MAX_COLS;
    }

    set_cursor_offset(cursor);
    return cursor;
}

int get_cursor_offset() 
{
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */

    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) 
{
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;

    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2; }