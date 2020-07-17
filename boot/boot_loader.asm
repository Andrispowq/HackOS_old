org 0x7C00
bits 16
	
	mov [BOOT_DRIVE], dl ; Remember that the BIOS sets us the boot drive in 'dl' on boot
	mov bp, 0x9000 ; set the stack
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print ; This will be written after the BIOS messages
	call println

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
    mov dh, 45 ; The kernel may be bigger later!
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
	
times 510 - ($ - $$) db 0
dw 0xAA55