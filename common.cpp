#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>

#include "common.h"

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


