#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

struct assembler
{
    char *instruction_filename; // имя файла на вход и указатель на него
    char *instruction_buffer; // буфер с текстом из входного файла, вводится один раз
    char *binary_filename; // имя файла на выход и указатель на него
    FILE *binary_file;
    int  *binary_buffer;
};

code_type get_operation_code(const char* name_of_command);
assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t initial_capacity);
assembler_type_error read_operation_code_from_instruction_file_and_put_to_binary_file(const char* instruction_filename, const char* binary_filename);
assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t initial_capacity);
void assembler_destructor(assembler* assembler_pointer);

#endif // ASSEMBLER_H_
