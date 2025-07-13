#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define MAX_CMD_LEN 128
#define MAX_ARGS 5

void shell_init(void);
void shell_handle_input(char c);
void shell_execute_command(const char* cmd);
void shell_print_prompt(void);

#endif