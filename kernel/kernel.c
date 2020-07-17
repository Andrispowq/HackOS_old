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

    initialise_paging();

    int arr[200];

    int stack_ptr = get_stack_pointer();
    int base_ptr = get_base_pointer();
    printf("Current stack frame (from top to bottom): %x -> %x\n", stack_ptr, base_ptr);

    printf("Location of the kernel_main function: %x\n", kernel_main);

    printf("Type something, it will go through the kernel\n"
        "Type END to halt the CPU\n> ");
}

void user_input(char *input) 
{
    if(tick > 10000)
    {
        printf("The kernel automatically closes after 200 seconds. Bye!\n");
        asm volatile("hlt");
    }

    if (strcmp(input, "end") == 0) 
    {
        printf("A quick page fault before we shut down! :D\n");
        uint32_t* ptr = (uint32_t*)0xA0000000;
        uint32_t page_fault = *ptr;

        printf("Stopping the CPU. Bye!\n");
        asm volatile("hlt"); //This requires the kernel, the other commands don't
    } else
    {
        console_command(input);   
    }
}