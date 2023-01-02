#include "utils.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include "err.h"

void set_close_on_exec(int file_descriptor, bool value) {
    int flags = fcntl(file_descriptor, F_GETFD);
    ASSERT_SYS_OK(flags);
    if (value)
        flags |= FD_CLOEXEC;
    else
        flags &= ~FD_CLOEXEC;
    ASSERT_SYS_OK(fcntl(file_descriptor, F_SETFD, flags));
}

char **split_string(const char *s) {
    size_t len = strlen(s);
    int spaces = 0;
    for (int i = 0; i < len; ++i)
        if (s[i] == ' ')
            spaces++;
    char **parts = calloc(spaces + 2, sizeof(char *));
    parts[spaces + 1] = NULL;
    int p = 0;
    int b = 0;
    for (int i = 0; i < len; ++i) {
        if (s[i] == ' ') {
            parts[p++] = strndup(s + b, i - b);
            b = i + 1;
        }
    }
    parts[p++] = strndup(s + b, len - b);
    assert(p == spaces + 1);
    return parts;
}

void free_split_string(char **parts) {
    for (int i = 0; parts[i] != NULL; ++i)
        free(parts[i]);
    free(parts);
}

bool read_line(char *buffer, size_t size_of_buffer, FILE *file) {
    if (size_of_buffer < 2)
        fatal("Buffer too small: %d\n", size_of_buffer);

    // static int time = 0;
    // printf("READING LINE %d\n", time++);

    char *line = NULL;
    size_t n_bytes;
    ssize_t n_chars = getline(&line, &n_bytes, file);

    if (n_chars == -1) {
        if (ferror(file))
            syserr("Getline failed.");
        assert(feof(file));
        buffer[0] = '\0';
        return false;
    }

    if (n_chars == 0) {
        free(line);
        assert(feof(file));
        buffer[0] = '\0';
        return false;
    }

    size_t len = strlen(line);
    if (len < n_chars)
        fatal("Null character in input.");
    assert(n_chars == len);

    if (len + 1 > size_of_buffer)
        fatal("Line too long: %d > %d.", len, size_of_buffer - 1);

    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    memcpy(buffer, line, len + 1);

    free(line);

    return true;
}

void print_buffer(char **buffer) {
    int i = 0;
    while (buffer[i] != NULL) {
        int j = 0;
        while (buffer[i][j] != '\0') {
            fprintf(stderr, "%c", buffer[i][j]);
            j++;
        }
        i++;
    }
}

bool is_empty(char **parts) {
    return isspace(*parts[0]) && parts[1] == NULL;
}

void push(int stack[], int *top, int value) {
    (*top)++;
    stack[*top] = value;
}

int pop(int stack[], int *top) {
    int value = stack[*top];
    (*top)--;
    return value;
}

bool is_empty_stack(int *top) {
    return *top == -1;
}
