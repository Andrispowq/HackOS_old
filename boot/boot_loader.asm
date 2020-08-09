org 0x7C00
bits 16

mboot:
    dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct

    dd  mboot                   ; Location of this descriptor
	
    push ebx

	mov [BOOT_DRIVE], dl ; Remember that the BIOS sets us the boot drive in 'dl' on boot
	mov bp, 0x9000 ; set the stack
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print ; This will be written after the BIOS messages
	call println

    ;mov al, 0xDF ; turn A20 on
    ;out 0x64, al

	call load_kernel
    call switch_to_pm
	
infinite_jump:
	jmp $

; The makefile requires to write boot/ even though they are in the same directory
%include "boot/print_real_mode.asm"
%include "boot/disk_reader_real_mode.asm"
%include "boot/gdt.asm"
%include "boot/print_protected_mode.asm"
%include "boot/enter_pm.asm"

bits 16

load_kernel:
    mov bx, KERNEL_OFFSET ; Read from disk and store in 0x1000
    mov dh, 50 ; The kernel may be bigger later!
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

bits 32

BEGIN_PM:
	mov ebx, MSG_KERNEL
	call print_pm
	
	call KERNEL_OFFSET
	
	call infinite_jump

MSG_REAL_MODE: db "In real mode!", 0x00
MSG_KERNEL: db "Launching kernel!", 0x00
BOOT_DRIVE: db 0

KERNEL_OFFSET equ 0x1000

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)
	
times 510 - ($ - $$) db 0
dw 0xAA55