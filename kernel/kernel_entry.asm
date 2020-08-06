bits 32
extern kernel_main ;load_kernel

global _start

_start:
	call kernel_main ;load_kernel
	jmp $