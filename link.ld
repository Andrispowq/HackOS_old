OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(start)
. = 0x00100000;

SECTIONS
{
    kernel_start = .;
    
    .boot :
    {
        *(.multiboot_header)
    }

    .text ALIGN(4096) : AT(ADDR(.text))
    {
        code = .; _code = .; __code = .;
        *(.text)
    }

    .rodata ALIGN(4096) : AT(ADDR(.rodata))
    {
        rodata = .; _rodata = .; __rodata = .;
        *(.rodata)
    }

    .data ALIGN(4096) : AT(ADDR(.data))
    {
        data = .; _data = .; __data = .;
        *(.data)
    }

    bss_start = .;

    .bss ALIGN(4096) : AT(ADDR(.bss))
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
    }

    bss_end = .;

    end = .; _end = .; __end = .;
}
