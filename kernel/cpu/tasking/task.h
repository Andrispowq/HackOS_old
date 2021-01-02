#ifndef TASK_H
#define TASK_H

#include "../../libc/memory.h"
#include "../../libc/string.h"
#include "../paging/paging.h"

void move_stack(void* new_stack_start, uint32_t size);

typedef struct task
{
    int id;                             // Process ID.
    uint32_t esp, ebp;                  // Stack and base pointers.
    uint32_t eip;                       // Instruction pointer.
    page_directory_t* page_directory;   // Page directory.
    struct task* next;                  // The next task in a linked list.
} task_t;

extern uint32_t read_eip();
extern void jump_to_ecx(uint32_t eip, uint32_t pageDirAddr, uint32_t ebp, uint32_t esp);

void initialise_tasking();
void switch_task();
int fork();
void move_stack(void* new_stack_start, uint32_t size);
int getpid();

void switch_to_user_mode();

#endif