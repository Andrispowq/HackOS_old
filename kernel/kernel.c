#include "kernel.h"
#include "cpu/isr.h"
#include "cpu/gdt.h"
#include "cpu/paging/paging.h"
#include "cpu/tasking/task.h"
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
extern uint32_t free_mem_addr;

#define SECOND 1000

uint32_t initial_esp;

void kernel_main(multiboot_info_t* mboot_ptr, uint32_t initial_stack)
{
    initial_esp = initial_stack;

    init_display();
    clrscr();

    gdt_install();
    isr_install();
    irq_install();

    uint32_t initrd_location = *((uint32_t*)mboot_ptr->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mboot_ptr->mods_addr + 4);

    free_mem_addr = initrd_end;

    initialise_paging();
    initialise_tasking();

    fs_root = initialise_initrd(initrd_location);

    int ret = fork();
    printf("fork() returned %x, and getpid() returned %x\n", ret, getpid());

    asm volatile("cli");
    
    int i = 0;
    struct dirent* node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0)
    {
        printf("Found file %s\n", node->name);
        fs_node_t* fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY)
        {
            printf("\t(directory)\n");
        }
        else
        {
            printf("\t contents: \"");
            char buf[256];
            memset(buf, 0, 256);
            uint32_t size = read_fs(fsnode, 0, 256, buf);
            printf(buf);

            printf("\"\n");
        }
        i++;
    }

    printf("Type 'help' for help!\n> ");

    asm volatile("sti");
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
        outw(0xB004, 0x2000);
        //Newer QEMU versions
        outw(0x0604, 0x2000);
        //VirtualBox
        outw(0x4004, 0x3400);
    }
    else if (strcmp(input, "restart") == 0) 
    {
        printf("Restarting the CPU. Bye!\n");

        //Flush the IRQ1 buffer
        unsigned temp;
        do
        {
           temp = (unsigned)inb(0x64);
           if((temp & 0x01) != 0)
           {
              (void)inb(0x60);
              continue;
           }
        } 
        while((temp & 0x02) != 0);
        
        //Restart
        outb(0x70, 0x8F);
        outb(0x71, 0x00);
        outb(0x70, 0x00);
        outb(0x92, inb(0x92) | 0x1);
    }
    else
    {
        console_command(input);   
    }
}