# HackOS
This is a hobby operating system I'm currently developing. I have big hopes that it will eventually turn out to some kind of hacking toolset, but of course that is just the plan, and may turn out completely differently.

I have a lot of plans, these are:
 - [x] Create a simple first-stage bootloader
 - [x] Jump into the kernel, written in C
 - [x] Create a simple VGA driver for outputting text in 25 x 80 text mode
 - [x] Create the Interrupt Descriptor Table, enable interrupts
 - [x] Create a very simple timer and keyboard driver
 - [x] Write a simple kmalloc() function for basic, 4K aligned and unaligned memory allocations for the kernel
 - [ ] Implement a basic paging system, for virtual memory
 - [ ] Write a second stage bootloader, so that the kernel can be bigger then 64 KiB, and it can be placed anywhere in the physical address space
 - [ ] Write a proper memory management system to use by the kernel
 - [ ] Write a VGA driver for low resolution (and maybe high resolution) graphics (256-bit)
 - [ ] Start working on switching to userspace
 - [ ] Writing a memory manager for userspace
 - [ ] Adding support for the FAT32 file system, to be able to have applications
 - [ ] Implementing multitasking
 - [ ] Writing a good, easy-to-use shell
 - [ ] Write a few basic apps for the OS, probably even a C compiler and an assembler, along with a linker
 - [ ] Switch to long mode, and implement the IDT and paging for 64 bit
 - [ ] Start working on proper hardware detection
 - [ ] Write a few other drivers, like a high resolution VGA driver, mouse driver, etc.
 - [ ] Integrate very simple networking
