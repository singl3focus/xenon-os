#include <stddef.h>
#include <string.h>

#include "shell.h"
#include "fb.h"
#include "fat16.h"
#include <string.h>

static uint32_t cursor_x = 15;
static uint32_t cursor_y = 20;
static const uint32_t BG_COLOR = 0x1F2126;

static char input_buffer[MAX_CMD_LEN];
static int input_index = 0;
static int shell_active = 1;

// Прототипы команд
static void cmd_help(void);
static void cmd_ls(const char* path);
static void cmd_cat(const char* path);
static void cmd_clear(void);

void shell_init(void) {
    input_index = 0;
    memset(input_buffer, 0, MAX_CMD_LEN);
    shell_print_prompt();
}

void shell_print_prompt(void) {
    fb_write("XenonOS> ");
}

void shell_handle_input(char c) {
    if (!shell_active) return;

    if (c == '\n') {
        input_buffer[input_index] = '\0';
        fb_write("\n");
        shell_execute_command(input_buffer);     
        input_index = 0;
        shell_print_prompt();
    } else if (c == '\b') {
        if (input_index > 0) {
            input_index--;
            input_buffer[input_index] = '\0';
            fb_backspace();
        }
    } else {
        if (input_index < MAX_CMD_LEN - 1) {
            input_buffer[input_index] = c;
            input_buffer[input_index + 1] = '\0';
            input_index++;
            char s[2] = {c, '\0'};
            fb_write(s);
        }
    }
}

void shell_execute_command(const char* cmd) {
    if (strlen(cmd) == 0) return;

    char cmd_copy[MAX_CMD_LEN];
    strcpy(cmd_copy, cmd);

    // Разбиение на аргументы
    char* args[MAX_ARGS] = {0};
    int arg_count = 0;
    char* token = strtok(cmd_copy, " ");
    while (token && arg_count < MAX_ARGS) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0) return;

    // Выполнение команд
    if (strcmp(args[0], "help") == 0) {
        cmd_help();
    } else if (strcmp(args[0], "ls") == 0) {
        const char* path = (arg_count > 1) ? args[1] : "/";
        cmd_ls(path);
    } else if (strcmp(args[0], "cat") == 0) {
        if (arg_count < 2) {
            fb_write("Usage: cat <file>\n");
        } else {
            cmd_cat(args[1]);
        }
    } else if (strcmp(args[0], "clear") == 0) {
        cmd_clear();
    } else {
        fb_write("Unknown command: ");
        fb_write(args[0]);
        fb_write("\n");
    }
}

// Реализация команд
static void cmd_help(void) {
    fb_write("Available commands:\n");
    fb_write("  help - show this help\n");
    fb_write("  ls [path] - list directory\n");
    fb_write("  cat <file> - show file content\n");
    fb_write("  clear - clear screen\n");
}

static void cmd_ls(const char* path) {
    fat16_list_dir(path);
}

static void cmd_cat(const char* path) {
    #define BUF_SIZE 256
    char buffer[BUF_SIZE];
    int bytes_read;
    int fd = fat16_open(path);
    
    if (fd < 0) {
        fb_write("File not found: ");
        fb_write(path);
        return;
    }
    
    while ((bytes_read = fat16_read(fd, buffer, BUF_SIZE-1)) > 0) {
        buffer[bytes_read] = '\0';
        fb_write(buffer);
    }
    
    fat16_close(fd);
    //fb_write("\n");
}

static void cmd_clear(void) {
    fb_clear(BG_COLOR);

    // Сброс курсора для shell
    cursor_x = 15;
    cursor_y = 20;

    extern void fb_reset_state(void);

    fb_reset_state();
}
