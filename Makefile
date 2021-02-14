
OS_NAME = HackOS

GNU_EFI = boot/gnu-efi
OVMF_DIR = OVMFbin

BOOT_EFI := $(GNUEFI)/x86_64/bootloader/main.efi
BUILD_DIR := kernel/bin

all:
	make bootldr
	make kernel
	make run

bootldr:
	make -C boot/gnu-efi bootloader

kernel:
	make -C kernel buildimg

install: kernel.bin initrd.img
	sudo cp kernel.bin /boot/kernel.bin
	sudo cp initrd.img /boot/modules/initrd.img

run:
	qemu-system-x86_64 -drive file=$(BUILD_DIR)/$(OS_NAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMF_DIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMF_DIR)/OVMF_VARS-pure-efi.fd" -net none

# kernel/second_stage_loader/*.o
clean:
	make -C kernel clean
	rm -rf boot/gnu-efi/x86_64/bootloader