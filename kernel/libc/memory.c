#include "memory.h"

void memory_copy(uint8_t* source, uint8_t* dest, int nbytes) 
{
    int i;
    for (i = 0; i < nbytes; i++) 
    {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t* dest, uint8_t val, uint32_t len) 
{
    uint8_t* temp = (uint8_t*) dest;
    for (; len != 0; len--) 
    {
        *temp++ = val;
    }
}

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
extern uint32_t end;
uint32_t free_mem_addr = (uint32_t)&end;

uint32_t kmalloc_int(size_t size, int align, uint32_t* phys_addr)
{
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0x00000FFF)) 
    {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }

    /* Save also the physical address */
    if (phys_addr) 
        *phys_addr = free_mem_addr;

    uint32_t ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    return ret;
}

uint32_t kmalloc_a(uint32_t size)
{
    return kmalloc_int(size, 1, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t* phys)
{
    return kmalloc_int(size, 0, phys);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t* phys)
{
    return kmalloc_int(size, 1, phys);
}

uint32_t kmalloc(uint32_t size)
{
    return kmalloc_int(size, 0, 0);
}