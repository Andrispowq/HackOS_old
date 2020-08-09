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

void kernel_main(struct multiboot* mboot_ptr)
{
    clear_screen();

    gdt_install();
    isr_install();
    irq_install();

    printf("Multiboot header: %x\n", mboot_ptr);

    printf("Initiating paging! Free memory location: %x\n", kmalloc(4));
    initialise_paging();
    void* ptr = (void*) kmalloc(8);
    printf("Initialised paging! Free memory location: %x\n", ptr);
    kfree(ptr);

    printf("Type something, it will go through the kernel\n"
        "Type 'help' for help!\n> ");
}

void user_input(char* input) 
{
    if(tick >= 200 * SECOND)
    {
        printf("The kernel automatically closes after 200 seconds. Bye!\n");
        asm volatile("hlt");
    }

    if (strcmp(input, "shutdown") == 0) 
    {
        printf("Stopping the CPU. Bye!\n");
        asm volatile("hlt"); //This requires the kernel, the other commands don't
    } else
    {
        console_command(input);   
    }
}