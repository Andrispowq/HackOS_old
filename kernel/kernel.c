#include "kernel.h"
#include "cpu/isr.h"
#include "cpu/gdt.h"
#include "cpu/paging/paging.h"
#include "drivers/screen.h"
#include "cpu/ports.h"
#include "drivers/ata/ata.h"
#include "libc/string.h"
#include "libc/memory.h"
#include "filesystem/initrd.h"
#include "filesystem/multiboot.h"

#include "console/console.h"

//defined in timer.c
extern uint32_t tick;

#define SECOND 1000

void kernel_main(multiboot_info_t* mboot_ptr)
{
    clear_screen();

    gdt_install();
    isr_install();
    irq_install();

    printf("Number of multiboot modules: %d\n", mboot_ptr->mods_count);
    printf("Address of multiboot modules: %x\n", mboot_ptr->mods_addr);
    //initialise_initrd((uint32_t)mboot_ptr);

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

        //Bochs/older QEMU versions
        port_word_out(0xB004, 0x2000);
        //Newer QEMU versions
        port_word_out(0x0604, 0x2000);
        //VirtualBox
        port_word_out(0x4004, 0x3400);
    }
    else if (strcmp(input, "restart") == 0) 
    {
        printf("Restarting the CPU. Bye!\n");

        //Flush the IRQ1 buffer
        unsigned temp;
        do
        {
           temp = (unsigned)port_byte_in(0x64);
           if((temp & 0x01) != 0)
           {
              (void)port_byte_in(0x60);
              continue;
           }
        } 
        while((temp & 0x02) != 0);
        
        //Restart
        port_byte_out(0x70, 0x8F);
        port_byte_out(0x71, 0x00);
        port_byte_out(0x70, 0x00);
        port_byte_out(0x92, port_byte_in(0x92) | 0x1);
    }
    else
    {
        console_command(input);   
    }
}