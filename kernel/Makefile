
OS_NAME = HackOS

GNU_EFI = ../boot/gnu-efi
OVMF_DIR = ../OVMFbin
LDS = link.ld
CC = gcc # $$HOME/opt/cross/bin/i686-elf-gcc
ASM = nasm
LD = ld # $$HOME/opt/cross/bin/i686-elf-ld

CFLAGS = -m32 -ffreestanding -fshort-wchar -fno-pic -c -g
ASM_FLAGS = -f elf32
LDFLAGS = -T $(LDS)
BOOT_EFI := $(GNU_EFI)/x86_64/bootloader/main.efi

SRC_DIR := src
BUILD_DIR := bin
OBJ_DIR := bin-int

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRC_DIR),*.c)  
ASM_SRC = $(call rwildcard,$(SRC_DIR),*.asm)  
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SRC))
DIRS = $(wildcard $(SRC_DIR)/*)

buildimg:
	dd if=/dev/zero of=$(BUILD_DIR)/$(OS_NAME).img bs=512 count=93750
	mformat -i $(BUILD_DIR)/$(OS_NAME).img -f 1440 ::
	mmd -i $(BUILD_DIR)/$(OS_NAME).img ::/EFI
	mmd -i $(BUILD_DIR)/$(OS_NAME).img ::/EFI/BOOT
	mcopy -i $(BUILD_DIR)/$(OS_NAME).img $(BOOT_EFI) ::/EFI/BOOT
	mcopy -i $(BUILD_DIR)/$(OS_NAME).img startup.nsh ::
	mcopy -i $(BUILD_DIR)/$(OS_NAME).img $(BUILD_DIR)/kernel.elf ::

kernel: $(OBJS) link

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASM) $(ASM_FLAGS) $^ -o $@
	 
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.elf $(OBJS)

# cleaning
clean:
	rm -rf iso
	rm -rf *.iso *.bin *.dis *.o os-image.bin *.elf *.tmp
	rm -rf ${OBJ_DIR}
	rm -rf ${BIN_DIR}
	mkdir ${OBJ_DIR}
	mkdir ${BUILD_DIR}