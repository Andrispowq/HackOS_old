#include "../drivers/screen.h"
#include "../drivers/ata/ata.h"
#include "../libc/memory.h"

void load_kernel()
{
    clear_screen();
    
    printf("Welcome to my operating system.\r\n");
    printf("Memory address of load_kernel in: %x\n", load_kernel);

    uint32_t phy_addr;
    uint32_t page = kmalloc_int(1000, 1, &phy_addr);

    /*read_sectors_ATA_PIO(page, 20, 64); // read 38 sectors (size of our kernel) from 12th sector (That's where kernel will be stored.)

    uint32_t* kernelFile = (uint32_t*)page;

    if(kernelFile[0] == 0x464C457F) // read first 4 bits of file to validate that it is an ELF file
    {
        printf("Kernel is ELF File:\n");
    }

    int bits = (kernelFile[1] >> 0) & 0xFF; // 5th bit is for 32-bit or 64-bit
    if(bits == 1)
    {
        printf("\t32-bit\n");
    }

    int endian = (kernelFile[1] >> 8) & 0xFF; // 6th bit is for little endian or big endian
    if(endian == 1)
    {
        printf("\tlittle endian\n");
    }
    
    printf("\tentry position: %x \n", page + kernelFile[6]); // get the entry point of a kernel
    printf("\tExecuting kernel from: %x....\n\n", page + kernelFile[6]); // add entry point of a kernel to base address of requested memory

    void (*kernel_main_function)(void) = (void(*)())(page + kernelFile[6]); */
    //kernel_main_function();
}