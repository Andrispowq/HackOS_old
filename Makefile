C_SOURCES = $(wildcard kernel/*.c kernel/drivers/*.c dkernel/rivers/ata/*.c kernel/cpu/*.c kernel/libc/*.c kernel/console/*.c kernel/cpu/paging/*.c)
HEADERS = $(wildcard kernel/*.h kernel/drivers/*.h kernel/drivers/ata/*.h kernel/cpu/*.h kernel/libc/*.h kernel/console/*.h kernel/cpu/paging/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o kernel/cpu/interrupts.o kernel/cpu/gdt_flush.o} 

# -g: Use debugging symbols in gcc
CFLAGS = -ffreestanding -m32 -fno-pic -c -g
LDFLAGS = -m elf_i386 -Tlink.ld

# First rule is run by default
os-image.bin: boot_loader.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: kernel/kernel_entry.o ${OBJ}
	ld ${LDFLAGS} -o $@ $^ --oformat binary
	
kernel.tmp: kernel/kernel_entry.o ${OBJ}
	ld ${LDFLAGS} -o $@ $^

boot_loader.bin:
	nasm -fbin boot/boot_loader.asm -o $@ 
	
kernel/kernel_entry.o:
	nasm -felf kernel/kernel_entry.asm -o $@

# Used for debugging purposes
kernel.elf: kernel/kernel_entry.o ${OBJ}
	ld ${LDFLAGS} -o $@ $^ 

run: os-image.bin kernel.tmp
	qemu-system-x86_64 -fda os-image.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-x86_64 -s -fda os-image.bin -d guest_errors,cpu_reset &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	gcc ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin kernel/drivers/*.o kernel/drivers/ata/*.o kernel/boot/*.o kernel/cpu/*.o kernel/libc/*.o kernel/console/*.o kernel/cpu/paging/*.o kernel/second_stage_loader/*.o