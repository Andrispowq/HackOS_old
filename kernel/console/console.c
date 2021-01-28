#include "console.h"

int check_command(char* cmd, const char* text);
int check_short_command(char* cmd, const char* text, int length);

void command_mode(char* input);
void calculator_mode(char* input);

int get_stack_pointer()
{
    asm volatile("mov %esp, %eax");
}

int get_base_pointer()
{
    asm volatile("mov %ebp, %eax");
}

extern uint32_t tick;

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
    if(check_command(input, "help"))
    {
        printf("This is the HackOS help panel!\nCurrently implemented commands:\n");
        printf("\t- clear: Clears the console\n");
        printf("\t- page: Allocates 4096 bytes of page-aligned memory (kmalloc test)\n");
        printf("\t- calc: Enters calculator mode, which is currently not functioning\n");
        printf("\t- print <arg>: Prints <arg> to the console\n");
        printf("\t- print <var>: Prints <val> to the console, if it's a kernel variable\n");
        printf("\t- shutdown: Halts the CPU, causing the computer to shut down\n");
        printf("\t- help: displays this menu");
    }
    else if(check_command(input, "page"))
    {
        uint32_t phys_addr;
        uint32_t page = kmalloc_int(1000, 1, &phys_addr);

        printf("Page: %x, physical addr: %x", page, phys_addr);

        state = COMMAND_MODE;
    }
    else if(check_command(input, "clear"))
    {
        clrscr();
        printf("Type 'shutdown' to halt the CPU");

        state = COMMAND_MODE;
    }
    else if(check_command(input, "calc"))
    {
        printf("Entering calculator mode!\nType 'quit' to return to command mode!");

        state = CALCULATOR_MODE;
    }
    else
    {
        if(check_short_command(input, "print", 5))
        {
            char* msg = &input[6];

            //Check if the user tries to get the value of an existing variable:
            if(check_short_command(msg, "time", 4))
            {
                printf("Time elapsed since startup: %dms", tick);
            } 
            else if(check_short_command(msg, "stack", 5))
            {
                printf("Top of the stack: %x", get_stack_pointer());
            }
            else
            {
                printf(msg);
            }
            
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
        printf("Exiting calculator mode.");
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
    } 
    else
    {
        int cnt;
        char** com = split(input, ' ', &cnt);

        printf("You said: \n");
        
        for(int i = 0; i < cnt; i++)
        {
            printf("%s ", com[i]);
        }

        //printf("Sorry, the calculator is not yet implemented!");    
    }

    state = CALCULATOR_MODE;
}

int check_command(char* cmd, const char* text)
{
    return strcmp(cmd, (char*)text) == 0;
}

int check_short_command(char* cmd, const char* text, int length)
{
    int i;
    for (i = 0; cmd[i] == text[i] && i < length; i++);

    if(i == length)
        return 1;
    else 
        return 0;
}