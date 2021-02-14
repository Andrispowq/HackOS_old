OUTPUT_FORMAT(elf32-i386) /*elf64-x86-64*/
OUTPUT_ARCH(i386)
ENTRY(kernel_start)
. = 0x00100000;

SECTIONS
{
    kernel_start = .;
    
    .text : ALIGN(0x1000)
	{
		*(.text)
	}

	.data : ALIGN(0x1000)
	{
		*(.data)
	}

	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}

	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}

    end = .; _end = .; __end = .;
}
