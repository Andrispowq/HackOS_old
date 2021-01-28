#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../libc/function.h"
#include "tasking/task.h"

uint32_t tick = 0;
static uint8_t task = 0;
static uint8_t task_was_on = 0;

void set_task(uint8_t i)
{
	if(!task_was_on) return;
	task = i;
}

void enable_task()
{
	task_was_on = 1;
	task = 1;
}

static void timer_callback(registers_t* regs) 
{
    tick++;
    UNUSED(regs);

    if(task)
    {
        schedule();
    }
}

void init_timer(uint32_t freq) 
{
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);

    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);
}