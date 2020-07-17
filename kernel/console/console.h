#ifndef CONSOLE_H
#define CONSOLE_H

#include "../libc/string.h"
#include "../libc/memory.h"
#include "../drivers/screen.h"

#define INVALID_MODE -1
#define COMMAND_MODE 0
#define CALCULATOR_MODE 1

static int state = COMMAND_MODE;

void console_command(char* input);

#endif
