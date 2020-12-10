C_SOURCES = $(wildcard kernel/*.c kernel/filesystem/*.c kernel/drivers/*.c kernel/drivers/ata/*.c kernel/cpu/*.c kernel/libc/*.c kernel/libc/data_structures/*.c kernel/console/*.c kernel/cpu/paging/*.c)
HEADERS = $(wildcard kernel/*.h kernel/filesystem/*.h kernel/drivers/*.h kernel/drivers/ata/*.h kernel/cpu/*.h kernel/libc/*.h kernel/libc/data_structures/*.h kernel/console/*.h kernel/cpu/paging/*.h)
OBJ = ${C_SOURCES:.c=.o kernel/boot.o kernel/cpu/interrupts.o kernel/cpu/gdt_flush.o} 

# -g: Use debugging symbols in gcc
CFLAGS = -m32 -ffreestanding -fno-pic -c -g
LDFLAGS = -m elf_i386 -Tlink.ld

# First rule is run by default
# rm -rf iso is at the beginning because in case building the iso failed, this directory would
# still remain there, preventing build it again
os-image.iso: kernel.bin
	rm -rf iso
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "HackOS" {'  			 >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin'    	 >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=os-image.iso iso/
	rm -rf iso

kernel.bin: ${OBJ}
	$$HOME/opt/cross/bin/i686-elf-ld ${LDFLAGS} -o $@ $^ --oformat binary
	
kernel.tmp: ${OBJ}
	$$HOME/opt/cross/bin/i686-elf-ld ${LDFLAGS} -o $@ $^

kernel.elf: ${OBJ}
	$$HOME/opt/cross/bin/i686-elf-ld ${LDFLAGS} -o $@ $^ 

run: os-image.iso
	qemu-system-i386 -cdrom os-image.iso

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.iso kernel.elf
	qemu-system-x86_64 -s -cdrom os-image.iso -d guest_errors,cpu_reset &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	$$HOME/opt/cross/bin/i686-elf-gcc ${CFLAGS} $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

# kernel/second_stage_loader/*.o
clean:
	rm -rf iso
	rm -rf *.iso *.bin *.dis *.o os-image.bin *.elf *.tmp
	rm -rf kernel/*.o kernel/filesystem/*.o boot/*.bin kernel/drivers/*.o kernel/drivers/ata/*.o kernel/boot/*.o kernel/cpu/*.o kernel/libc/*.o kernel/libc/data_structures/*.o kernel/console/*.o kernel/cpu/paging/*.o kernel/second_stage_loader/*.o