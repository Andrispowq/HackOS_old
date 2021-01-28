#ifndef TASK_H
#define TASK_H

#include "../../libc/memory.h"
#include "../../libc/string.h"
#include "../paging/paging.h"

#define PROCESS_STATE_ALIVE 0
#define PROCESS_STATE_ZOMBIE 1
#define PROCESS_STATE_DEAD 2

void move_stack(void* new_stack_start, uint32_t size);
extern void jump_to_ecx(uint32_t eip, uint32_t pageDirAddr, uint32_t ebp, uint32_t esp);

typedef struct process
{
	char* name;
    int id;
    uint32_t esp, ebp;
    uint32_t eip;
	uint32_t state;
    page_directory_t* page_directory;

	void (*notify)(int);

    struct process* next;
} PROCESS;

int addProcess(PROCESS* t);
PROCESS* createProcess(char* name, void* addr);

int is_pid_running(uint32_t pid);

int is_tasking();

char* get_name();
int getpid();
PROCESS* get_process();

void send_sig(int sig);

void tasking_print_all();

void _kill();
void kill();
void schedule();
void schedule_noirq();
void initialise_tasking();

#endif