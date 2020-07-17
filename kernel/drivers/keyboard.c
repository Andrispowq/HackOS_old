#include "keyboard.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

static char key_buffer[256];

#define SC_MAX 57

int shift_down = 0;
int control_down = 0;
int super_down = 0;
int alt_down = 0;

#define HUNGARIAN_LAYOUT 0

#if HUNGARIAN_LAYOUT != 0
//Hungarian layout
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "Ö", "Ü", "Ó", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Z", "U", "I", "O", "P", "Ő", "Ú", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", "É", "Á", "0", 
        "LShift", "Ű", "Y", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar", "Caps lock",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "Numlock"};

const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', 'Ö', 'Ü', 'Ó', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Z', 
        'U', 'I', 'O', 'P', 'Ő', 'Ú', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', 'É', 'Á', '0', '?', 'Ű', 'Y', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '*', '?', ' ', '?',
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?'};
#else
//United States layout
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};
#endif

static void keyboard_callback(registers_t* regs) 
{
    //the PIC leaves us the scancode in port 0x60 
    uint8_t scancode = port_byte_in(0x60);

    if (scancode > SC_MAX) 
        return;

    if (scancode == BACKSPACE) 
    {
        if(strlen(key_buffer) == 0)
            return;

        backspace(key_buffer);
        printf_backspace();
    } 
    else if (scancode == ENTER) 
    {
        printf("\n");
        user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } 
    else 
    {
        if(strlen(key_buffer) == 255)
            return;

        char letter = sc_ascii[(int)scancode];

        if(letter == '?')
        {
            char* scanname = sc_name[(int)scancode];
            if(strcmp(scanname, "Lctrl") == 0)
            {
                shift_down = 0;
                control_down = !control_down;
                super_down = 0;
                alt_down = 0;
            } 
            else if(strcmp(scanname, "Lshift") == 0)
            {
                shift_down = !shift_down;
                control_down = 0;
                super_down = 0;
                alt_down = 0;
            }
            else if(strcmp(scanname, "Rshift") == 0)
            {
                shift_down = !shift_down;
                control_down = 0;
                super_down = 0;
                alt_down = 0;
            }
            else if(strcmp(scanname, "Lalt") == 0)
            {
                shift_down = 0;
                control_down = 0;
                super_down = 0;
                alt_down = !alt_down;
            }
        } 
        else
        {
            if(!control_down && (letter >= 'A' && letter <= 'Z'))
                letter += ('a' - 'A'); //If shift is not down, we display a lowercase letter

            /* Remember that kprint only accepts char[] */
            char str[2] = { letter, '\0' };
            append(key_buffer, letter);
            printf(str);
        }      

    }
    
    UNUSED(regs);
}

void init_keyboard() 
{
    register_interrupt_handler(IRQ1, keyboard_callback);
}