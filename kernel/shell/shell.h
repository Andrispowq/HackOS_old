#ifndef SHELL_H
#define SHELL_H

#include "../libc/string.h"
#include "../libc/memory.h"
#include "../libc/stdio.h"

#define INVALID_MODE -1
#define COMMAND_MODE 0
#define CALCULATOR_MODE 1

static int state = COMMAND_MODE;

void shell_command(char* input);

#endif
