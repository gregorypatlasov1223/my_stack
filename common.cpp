#include "common.h"

#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>

const char *const REGISTER_NAMES[NUMBER_OF_REGISTERS] = {
    "RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"
};

size_t get_file_size(FILE *file)
{
    assert(file != NULL);

    struct stat stat_buffer = {};

    int file_descriptor = fileno(file);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: Cannot get file descriptor\n");
        return 0;
    }

    if (fstat(file_descriptor, &stat_buffer) != 0)
    {
        fprintf(stderr, "Error: Cannot get file stats\n");
        return 0;
    }

    return (size_t)stat_buffer.st_size;
}

const char *get_register_name(register_code reg)
{
    if (reg >= REG_RAX && reg <= REG_RHX)
        return REGISTER_NAMES[reg];

    return "UNKNOWN_REGISTER";
}

register_code get_register_by_name(const char *name)
{
    if (name == NULL)
        return REG_INVALID;

    for (int index = 0; index < NUMBER_OF_REGISTERS; index++)
    {
        if (strcmp(name, REGISTER_NAMES[index]) == 0)
            return (register_code)index;
    }

    return REG_INVALID;
}
