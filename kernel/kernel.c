#include "kernel.h"
#include "cpu/isr.h"
#include "cpu/gdt.h"
#include "cpu/paging/paging.h"
#include "drivers/screen.h"
#include "drivers/ata/ata.h"
#include "libc/string.h"
#include "libc/memory.h"

#include "console/console.h"

//defined in timer.c
extern uint32_t tick;

#define SECOND 1000

int get_stack_pointer()
{
    asm volatile("mov %esp, %eax");
}

int get_base_pointer()
{
    asm volatile("mov %ebp, %eax");
}

void kernel_main()
{
    clear_screen();

    gdt_install();
    isr_install();
    irq_install();

    uint32_t a = kmalloc(8);
    initialise_paging();
    uint32_t b = kmalloc(8);
    uint32_t c = kmalloc(8);
    printf("A: %x, B: %x, C: %x\n", a, b, c);

    kfree((void*)c);
    kfree((void*)b);
    uint32_t d = kmalloc(8);
    printf("D: %x\n", d);
    kfree((void*)d);

    printf("Type something, it will go through the kernel\n"
        "Type 'end' to halt the CPU\n> ");
}

void user_input(char* input) 
{
    if(tick >= 200 * SECOND)
    {
        printf("The kernel automatically closes after 200 seconds. Bye!\n");
        asm volatile("hlt");
    }

    if (strcmp(input, "end") == 0) 
    {
        printf("Stopping the CPU. Bye!\n");
        asm volatile("hlt"); //This requires the kernel, the other commands don't
    } else
    {
        console_command(input);   
    }
}