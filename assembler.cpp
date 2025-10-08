#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include "assembler.h"
#include "calculator.h"
#include "asm_error_types"

code_type get_operation_code(const char* name_of_command)
{
    assert(name_of_command != NULL);

    if (strcmp(name_of_command, "HLT") == 0)
        return code_HLT;

    if (strcmp(name_of_command, "PUSH") == 0)
        return code_PUSH;

    if (strcmp(name_of_command, "POP") == 0)
        return code_POP;

    if (strcmp(name_of_command, "ADD") == 0)
        return code_ADD;

    if (strcmp(name_of_command, "SUB") == 0)
        return code_SUB;

    if (strcmp(name_of_command, "MUL") == 0)
        return code_MUL;

    if (strcmp(name_of_command, "DIV") == 0)
        return code_DIV;

    if (strcmp(name_of_command, "SQRT") == 0)
        return code_SQRT;

    if (strcmp(name_of_command, "OUT") == 0)
        return code_OUT;

    fprintf(stderr, "Error: Unknown command '%s'\n", name_of_command);
    return code_SHIT; // правильно возвращаю?
}


assembler_type_error read_from_instruction_file_to_buffer(assembler *assembler_pointer)
{
    assert(assembler_pointer                         != NULL);
    assert(assembler_pointer -> instruction_filename != NULL);
    assert(assembler_pointer -> instruction_buffer   != NULL);

    FILE* instruction_file = fopen(assembler_pointer -> instruction_filename, "r");
    if (instruction_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open instruction_file.txt\n");
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }

    size_t size_of_file = get_file_size(instruction_file);

    if (size_of_file < 0)
    {
        fclose(instruction_file);
        return ASM_READING_FILE_ERROR;
    }

    assembler_pointer -> instruction_buffer = (char*)calloc(size_of_file + 1, sizeof(char)); // +1 для '\0'
    if (assembler_pointer -> instruction_buffer == NULL)
    {
        fclose(instruction_file);

        return ASM_ALLOCATION_FAILED_ERROR;
    }

    size_t number_of_bytes_read = fread(assembler_pointer -> instruction_buffer, sizeof(char), size_of_file, instruction_file);
    if (number_of_bytes_read != size_of_file)
    {
        fclose(instruction_file);
        free(assembler_pointer -> instruction_buffer);
        assembler_pointer -> instruction_buffer = NULL;

        return ASM_READING_FILE_ERROR;
    }

    assembler_pointer -> instruction_buffer[size_of_file] = '\0';
    fclose(instruction_file);
}


assembler_type_error read_operation_code_from_instruction_file_and_put_to_binary_file(const char *instruction_filename, const char *binary_filename)
{
    // TODO: from buffer to binary (fwrite)

    assert(assembler_pointer -> instruction_filename != NULL);
    assert(assembler_pointer -> binary_filename      != NULL); 

    FILE* instruction_file = fopen(assembler_pointer -> instruction_filename, "r");
    if (instruction_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open instruction_file.txt\n");
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }

    assembler_pointer -> binary_file = fopen(assembler_pointer -> binary_filename, "wb"); // бинарник поэтому wb?
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open binary_file.txt\n");
        return ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR;
    }

    char name_of_command[50] = {0};

    code_type code_of_command = code_SHIT;

    type_of_element argument = 0;
    type_of_element number_of_arguments = 0;

    int should_continue = 1;

    while (should_continue)
    {
        if (get_operation_code(&code_of_command) == code_SHIT)
        {
            if (feof(instruction_file))
            {
                printf("End of file reached. Normal termination.\n");
                break;
            }

            fprintf(stderr, "The command wasn't read. Try again\n");
            continue;
        }

        if (code_of_command == code_HLT)
            break;

        switch(code_of_command)
        {
            case code_PUSH:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, assembler_pointer -> binary_file);
                else
                    fprintf(stderr, "PUSH failed\n");
                break;

            case code_POP:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, assembler_pointer -> binary_file);
                else
                    fprintf(stderr, "POP failed\n");
                break;

            case code_ADD:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, assembler_pointer -> binary_file);
                break;

            case code_MUL:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, assembler_pointer -> binary_file);
                break;

            case code_SUB:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, assembler_pointer -> binary_file);
                break;

            case code_DIV:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, binary_file);
                break;

            case code_SQRT:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, binary_file);
                break;

            case code_OUT:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                    fwrite(&argument, sizeof(int), 1, binary_file);
                break;

            default:
                fprintf(stderr, "Syntax error. The command wasn't read. Try again\n"); // что возвращать если код ошибки несуществующий
        }

        number_of_arguments += 1;
    }

    fclose(instruction_file);
    fclose(binary_file);

    return number_of_arguments;
}


assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t initial_capacity)
{
    assert(assembler_pointer != NULL);
    assert(input_filename    != NULL);
    assert(output_filename   != NULL);

    assembler_pointer -> instruction_filename = input_filename;
    assembler_pointer -> binary_filename = output_filename;

    assembler_pointer -> instruction_buffer = (char*)calloc(initial_capacity, sizeof(char));
    assembler_pointer -> binary_buffer      = (int*)calloc(initial_capacity * sizeof(int), sizeof(int));

}


void assembler_destructor(assembler* assembler_pointer)
{
    if (assembler_pointer -> instruction_buffer != NULL)
        free(assembler_pointer -> instruction_buffer);

    if (assembler_pointer -> binary_buffer != NULL)
        free(assembler_pointer -> binary_buffer);

    if (assembler_pointer -> binary_filename != NULL)
        free(assembler_pointer -> binary_filename);

    if (assembler_pointer -> binary_file != NULL)
        free(assembler_pointer -> binary_file);

    if (assembler_pointer -> instruction_filename != NULL)
        free(assembler_pointer -> instruction_filename);

    assembler_pointer -> instruction_filename = NULL;
    assembler_pointer -> binary_filename      = NULL;
    assembler_pointer -> binary_file          = NULL
    assembler_pointer -> instruction_file     = NULL;
    assembler_pointer -> binary_file          = NULL;
}


size_t get_file_size(FILE *file)
{
    assert(file != NULL);

    struct stat stat_buffer = {}; // верно?

    if (stat(file, &stat_buffer) != 0)
    {
        fprintf(stderr, "Error: Cannot open file\n");
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }

    return stat_buffer.st_size;
}

