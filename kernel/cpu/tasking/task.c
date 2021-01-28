#include "task.h"
#include "../../kernel.h"

PROCESS* cp;
PROCESS* ready_queue;
uint32_t lpid = 0;
uint8_t __enabled = 0;

extern uint32_t initial_esp;
extern page_directory_t* current_directory;
extern page_directory_t* kernel_directory;

extern void alloc_frame(page_t* page, int is_kernel, int is_writeable);
extern void set_task(uint8_t i);
extern void enable_task();

void task_confirm()
{
	printf("Tasking is running!\n");
    _kill();
}

void idle_thread()
{
    enable_task();
	__enabled = 1;
    while(1);
}

void kill(uint32_t pid)
{
	if(pid == 1) printf("Idle can't be killed!\n");
	if(pid == cp->id) _kill();

	PROCESS* orig = cp;
	PROCESS* p = orig;

	while(p)
	{
		if(p->id == pid) 
        {
			printf("Process %s (%d) was set to ZOMBIE.\n", p->name, pid);
			p->state = PROCESS_STATE_ZOMBIE;
			break;
		}

		p = p->next;
	}
}

void send_sig(int sig)
{
	cp->notify(sig);
}

int is_tasking()
{
	return __enabled;
}

PROCESS* get_process()
{
	return cp;
}

char* get_name()
{
	return cp->name;
}

int getpid()
{
	return cp->id;
}

void _kill()
{
	asm volatile("cli");

	if(cp->id == 1) 
    { 
        set_task(0); 
        printf("Idle can't be killed!\n");
        while(1) asm volatile("hlt");
    }

	//printf("Killing process %s (%d)\n", cp->name, cp->id);

	set_task(0);
	kfree(cp->page_directory);

    PROCESS* tmp = ready_queue;
    while(tmp)
    {
        if(tmp->next == cp)
        {
            tmp->next = cp->next;
        }

        tmp = tmp->next;
    }
	kfree(cp);
	set_task(1);

	asm volatile("sti");

	schedule();
}

void tasking_print_all()
{
	PROCESS *orig = cp;
	PROCESS *p = orig;

	while(1)
	{
		printf("Process: %s (%d) %s\n", p->name, p->id,
			p->state == PROCESS_STATE_ZOMBIE ? "ZOMBIE":
					p->state==PROCESS_STATE_ALIVE ? "ALIVE" : "DEAD");
		p = p->next;
		if(p == orig) break;
	}
}

void __notified(int sig)
{
	/*switch(sig)
	{
		case SIG_ILL:
			pidprint("Received SIGILL, terminating!\n");
			_kill();
			break;
		case SIG_TERM:
			pidprint("Received SIGTERM, terminating!\n");
			_kill();
		case SIG_SEGV:
			pidprint("Received SIGSEGV, terminating!\n");
			_kill();
		default:
			pidprint("Received unknown SIG!\n");
			return;
	}*/
}

int is_pid_running(uint32_t pid)
{
	set_task(0);

	PROCESS* p = cp;
	PROCESS* orig = cp;

	int ret = 0;
	while(1)
	{
		if(p->id == pid)  
        { 
            ret = 1; 
            break;
        }

		p = p->next;
		if(p == orig) break;
	}

	set_task(1);
	return ret;
}

PROCESS* createProcess(char* name, void* addr)
{
	PROCESS* p = (PROCESS*) kmalloc(sizeof(PROCESS));
	memset(p, 0, sizeof(PROCESS));

	p->name = name;
	p->id = ++lpid;
	p->eip = (uint32_t)addr;
	p->state = PROCESS_STATE_ALIVE;
	p->notify = __notified;
    p->page_directory = clone_directory(current_directory);
    p->next = 0;

	p->esp = kmalloc(4096);
	uint32_t* stack = (uint32_t*)(p->esp + 4096);
	p->ebp = stack;
	*--stack = 0x00000202; // eflags
	*--stack = 0x8; // cs
	*--stack = (uint32_t)addr; // eip
	*--stack = 0; // eax
	*--stack = 0; // ebx
	*--stack = 0; // ecx;
	*--stack = 0; //edx
	*--stack = 0; //esi
	*--stack = 0; //edi
	*--stack = p->esp + 4096; //ebp
	*--stack = 0x10; // ds
	*--stack = 0x10; // fs
	*--stack = 0x10; // es
	*--stack = 0x10; // gs
	p->esp = (uint32_t)stack;

	return p;
}

/* This adds a process while no others are running! */
void __addProcess(PROCESS* p)
{
    PROCESS* tmp = ready_queue;
    while (tmp->next)
       tmp = tmp->next;

    tmp->next = p;
    return;
}

/* add process but take care of others also! */
int addProcess(PROCESS* p)
{
	set_task(0);
	__addProcess(p);
	set_task(1);
	return p->id;
}

void __exec()
{
    uint32_t esp, ebp, eip;
    eip = cp->eip;
    esp = cp->esp;
    ebp = cp->ebp;

    current_directory = cp->page_directory;

    //printf("ESP: %x, EBP: %x, EIP: %x, curr_dir: %x\n", esp, ebp, eip, current_directory->physicalAddr);
    jump_to_ecx(eip, current_directory->physicalAddr, ebp, esp);
}

void schedule_noirq()
{
	if(!__enabled) return;
	asm volatile("int $0x2E");
	return;
}

void schedule()
{
    uint32_t esp, ebp, eip;

    cp = cp->next;
    if(!cp) cp = ready_queue; //If we're at the end, start over again

    eip = cp->eip;
    esp = cp->esp;
    ebp = cp->ebp;

    current_directory = cp->page_directory;

    //printf("ESP: %x, EBP: %x, EIP: %x, curr_dir: %x\n", esp, ebp, eip, current_directory->physicalAddr);
    jump_to_ecx(eip, current_directory->physicalAddr, ebp, esp);
}

void initialise_tasking()
{
    // Relocate the stack so we know where it is
    move_stack((void*)0xE0000000, 0x2000);

	cp = createProcess("kernel_idle", idle_thread);
	cp->next = 0;
    ready_queue = cp;

	__addProcess(createProcess("task_confirm", task_confirm));
	__addProcess(createProcess("kernel", kernel_task));
	__exec();

    asm volatile("sti");

	printf("Failed to start tasking!");
}

void move_stack(void* new_stack_start, uint32_t size)
{
    uint32_t i;

    for(i = (uint32_t)new_stack_start; i >= ((uint32_t)new_stack_start - size);
        i -= 0x1000)
    {
        alloc_frame(get_page(i, 1, current_directory), 0, 1);
    }

    // Flush the TLB by reading and writing the page directory address again.
    uint32_t pd_addr;
    asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
    asm volatile("mov %0, %%cr3" : : "r" (pd_addr)); 

    uint32_t old_stack_pointer; 
    asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));

    uint32_t old_base_pointer;  
    asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

    uint32_t offset = (uint32_t)new_stack_start - initial_esp; 
    uint32_t new_stack_pointer = old_stack_pointer + offset;
    uint32_t new_base_pointer = old_base_pointer + offset; 

    memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp - old_stack_pointer);

    // Backtrace through the original stack, copying new values into
    // the new stack.
    for(i = (uint32_t)new_stack_start; i > (uint32_t)new_stack_start - size; i -= 4)
    {
        uint32_t tmp = *(uint32_t*)i;

        if ((old_stack_pointer < tmp) && (tmp < initial_esp))
        {
            tmp = tmp + offset;
            uint32_t* tmp2 = (uint32_t*)i;
            *tmp2 = tmp;
        }
    }

    // Change stacks.
    asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
    asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}