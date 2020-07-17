#ifndef STRINGS_H
#define STRINGS_H

#include "stdint.h"

void itoa(uint8_t* buf, uint32_t base, uint32_t d);
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
int strncmp(char s1[], char s2[], int bytes);

#endif