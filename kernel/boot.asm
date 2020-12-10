section .multiboot_header

MBOOT_HEADER_FLAGS  equ 0x00000007
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS) & 0xFFFFFFFF

global multiboot_header         ; Make 'multiboot_header' accessible from C.
extern code                     ; Start of the '.text' section.
extern bss                      ; Start of the .bss section.
extern end                      ; End of the last loadable section.

global start                    ; Kernel entry point.
extern kernel_main              ; This is the entry point of our C code

align 16, db 0
multiboot_header:
    dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct

    dd  0,0,0,0,0               ; Aout kludge (unused)
    
    dd  1                       ; Linear graphics
    dd  0                       ; Preferred width
    dd  0                       ; Preferred height
    dd  32                      ; Preferred pixel depth

bits 32
section .text

align 4
start:
    mov     esp, default_stack_pointer

    ; Load multiboot information:
    push    esp
    push    eax
    push    ebx

    ; Execute the kernel:
    call    kernel_main
    cli
    jmp     $ 

KERNEL_STACK_SIZE   equ 8192 

section .bss
align 4
resb KERNEL_STACK_SIZE
default_stack_pointer: