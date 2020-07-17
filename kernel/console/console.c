#include "console.h"

int check_command(char* cmd, const char* text);
int check_short_command(char* cmd, const char* text, int length);

void command_mode(char* input);
void calculator_mode(char* input);

void console_command(char* input)
{
    switch(state)
    {
    case COMMAND_MODE:
        command_mode(input);
        break;
    case CALCULATOR_MODE:
        calculator_mode(input);
        break;
    }    

    printf("\n> ");
}

void command_mode(char* input)
{
    if(check_command(input, "page"))
    {
        uint32_t phys_addr;
        uint32_t page = kmalloc_int(1000, 1, &phys_addr);

        printf("Page: %x, physical addr: %x", page, phys_addr);

        state = COMMAND_MODE;
    }
    else if(check_command(input, "clear"))
    {
        clear_screen();
        printf("Type 'end' to halt the CPU");

        state = COMMAND_MODE;
    }
    else if(check_command(input, "calc"))
    {
        printf("Entering calculator mode!\nType QUIT to return to command mode!");

        state = CALCULATOR_MODE;
    }
    else
    {
        char* print_buff = &input[0];
        print_buff[5] = '\0';
        if(check_command(print_buff, "print"))
        {
            char* msg = &print_buff[6];
            printf(msg);
        } else
        {
            printf("You said: %s", input);            
        }
        

        state = COMMAND_MODE;
    }
}

void calculator_mode(char* input)
{
    //First, check for functions like sin, cos, tan
    if(check_command(input, "quit"))
    {
        state = COMMAND_MODE;
        return;
    }
    else if(check_short_command(input, "sin", 3))
    {
        printf("Sorry, the 'sin' function is not yet supported!");
    }
    else if(check_short_command(input, "cos", 3))
    {
        printf("Sorry, the 'cos' function is not yet supported!");
    }
    else if(check_short_command(input, "tan", 3))
    {
        printf("Sorry, the 'tan' function is not yet supported!");
    }
    else if(check_short_command(input, "cotan", 5))
    {
        printf("Sorry, the 'cotan' function is not yet supported!");
    } else
    {
        printf("Sorry, the calculator is not yet implemented!");    
    }

    state = CALCULATOR_MODE;
}

int check_command(char* cmd, const char* text)
{
    return !strcmp(cmd, (char*)text);
}

int check_short_command(char* cmd, const char* text, int length)
{
    return !strncmp(cmd, (char*)text, length);
}