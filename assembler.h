#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "calculator.h"
#include "common.h"
#include "asm_error_types.h"

const int MAX_LABEL_LENGTH      = 32;
const int MAX_COMMAND_LENGTH    = 32;
const int MAX_ARGUMENT_NUMBER   = 32;
const int MAX_NUMBER_OF_LABELS  = 100;
const char label_id_symbol      = ':';

struct label
{
    char name[MAX_LABEL_LENGTH];
    int address;
};

struct label_table
{
    label labels[MAX_NUMBER_OF_LABELS];
    int number_of_labels;
};

struct assembler
{
    char *instruction_filename; // имя файла на вход и указатель на него
    char *instruction_buffer; // буфер с текстом из входного файла, вводится один раз
    char *binary_filename; // имя файла на выход и указатель на него
    FILE *binary_file;
    int  *binary_buffer;
    label_table list_of_labels;
};

void asm_error_translator(assembler_type_error error);
void assembler_destructor(assembler* assembler_pointer);
code_type get_operation_code(const char* name_of_command);

assembler_type_error process_jump_command(assembler *assembler_pointer, int *binary_buffer, size_t *binary_index,
                                          char **buffer_ptr, code_type operation_code, FILE *binary_file);
assembler_type_error process_call_command(assembler *assembler_pointer, int *binary_buffer, size_t *binary_index,
                                          char **buffer_ptr, FILE *binary_file);
assembler_type_error process_memory_command(assembler *assembler_pointer, int *binary_buffer, size_t *binary_index,
                                            char **buffer_ptr, code_type operation_code);
assembler_type_error process_register_command(assembler *assembler_pointer, int *binary_buffer, size_t *binary_index,
                                              char **buffer_ptr, code_type operation_code, FILE *binary_file);
assembler_type_error process_push_command(assembler *assembler_pointer, int *binary_buffer, size_t *binary_index,
                                          char **buffer_ptr, FILE *binary_file);
                                          
assembler_type_error from_buffer_to_binary_file(assembler *assembler_pointer);
assembler_type_error read_from_instruction_file_to_buffer(assembler *assembler_pointer);
assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename);

void init_label_table(label_table* ptr_table);
int find_label(label_table* table, const char* name);
assembler_type_error add_label(label_table* table, const char* name, int address);

assembler_type_error first_pass(assembler *assembler_pointer);
assembler_type_error second_pass(assembler *assembler_pointer);
int command_requires_argument(code_type operation);

#endif // ASSEMBLER_H_
