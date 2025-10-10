#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "calculator.h"
#include "asm_error_types.h"

struct assembler
{
    char *instruction_filename; // имя файла на вход и указатель на него
    char *instruction_buffer; // буфер с текстом из входного файла, вводится один раз
    char *binary_filename; // имя файла на выход и указатель на него
    FILE *binary_file;
    int  *binary_buffer;
};

code_type get_operation_code(const char* name_of_command);
assembler_type_error from_buffer_to_binary_file(assembler *assembler_pointer);
assembler_type_error read_from_instruction_file_to_buffer(assembler *assembler_pointer);
assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename);
void assembler_destructor(assembler* assembler_pointer);
void asm_error_translator(assembler_type_error error);
size_t get_file_size(FILE *file);

#endif // ASSEMBLER_H_
