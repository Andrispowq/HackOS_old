#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

void memory_copy(uint8_t* source, uint8_t* dest, int nbytes);
void memory_set(uint8_t* dest, uint8_t val, uint32_t len);

/* At this stage there is no 'free' implemented. */
uint32_t kmalloc_int(size_t size, int align, uint32_t* phys_addr);

uint32_t kmalloc_a(size_t size); // page aligned.
uint32_t kmalloc_p(size_t size, uint32_t* phys); // returns a physical address.
uint32_t kmalloc_ap(size_t size, uint32_t* phys); // page aligned.
uint32_t kmalloc(size_t size); // normal kmalloc

#endif