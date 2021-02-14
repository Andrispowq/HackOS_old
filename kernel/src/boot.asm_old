section .multiboot_header

MBOOT_HEADER_FLAGS  equ 0x00000003 ;0x00000007
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS) & 0xFFFFFFFF

global multiboot_header         ; Make 'multiboot_header' accessible from C.
extern code                     ; Start of the '.text' section.
extern bss                      ; Start of the .bss section.
extern end                      ; End of the last loadable section.

global start                    ; Kernel entry point.
extern kernel_main              ; This is the entry point of our C code

multiboot_header:
    dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct

    ; dd  0,0,0,0,0               ; Aout kludge (unused)
    ; 
    ; dd  1                       ; Linear graphics
    ; dd  0                       ; Preferred width
    ; dd  0                       ; Preferred height
    ; dd  32                      ; Preferred pixel depth
    
    dd  multiboot_header        ; Location of this descriptor
    dd  code                    ; Start of kernel '.text' (code) section.
    dd  bss                     ; End of kernel '.data' section.
    dd  end                     ; End of kernel.
    dd  start                   ; Kernel entry point (initial EIP).

bits 32
section .text

start:
    mov     esp, default_stack_pointer

    ; Load multiboot information:
    push    esp
    ; push    eax
    push    ebx

    ; Execute the kernel:
    cli
    call    kernel_main
    jmp     $ 

KERNEL_STACK_SIZE   equ 8192 

section .bss
align 4
resb KERNEL_STACK_SIZE
default_stack_pointer: