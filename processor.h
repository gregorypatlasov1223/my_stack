#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <sys/stat.h>

#include "stack.h"
#include "common.h"
#include "proc_error_types.h"

struct processor
{
    stack_t stack;
    int registers[NUMBER_OF_REGISTERS];
    int instruction_counter;
    int *code_buffer;
    size_t code_buffer_size;
};

processor_error_type execute_processor(processor *processor_pointer);
processor_error_type processor_constructor(processor* processor_pointer, size_t starting_capacity);
processor_error_type read_binary_file_to_buffer(processor* processor_pointer, const char *binary_filename);

void proc_error_translator(processor_error_type error);
void processor_destructor(processor *processor_pointer);
void processor_dump(processor* proc, processor_error_type code_error, const char* message);

size_t get_size_of_binary_file(FILE *file);

#endif // PROCESSOR_H_
