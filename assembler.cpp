#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include "assembler.h"
#include "calculator.h"
#include "colour_codes.h"
#include "asm_error_types.h"

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
    return code_SHIT;
}


assembler_type_error read_from_instruction_file_to_buffer(assembler *assembler_pointer)
{
    assert(assembler_pointer                         != NULL);
    assert(assembler_pointer -> instruction_filename != NULL);

    FILE *instruction_file = fopen(assembler_pointer -> instruction_filename, "r");
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
        fclose(instruction_file); // free не нужен так как calloc вернул NULL

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

    return ASM_NO_ERROR;
}


assembler_type_error from_buffer_to_binary_file(assembler *assembler_pointer)
{
    assert(assembler_pointer                       != NULL);
    assert(assembler_pointer -> instruction_buffer != NULL);
    assert(assembler_pointer -> binary_filename    != NULL);

    FILE *binary_file = fopen(assembler_pointer -> binary_filename, "wb");
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open binary file for writing\n");
        return ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR;
    }

    char *buffer_pointer = assembler_pointer -> instruction_buffer; // указатель на текущий элемент
    size_t number_of_commands = 0;

    while (*buffer_pointer != '\0')
    {
        while (*buffer_pointer == ' ' || *buffer_pointer == '\n' || *buffer_pointer == '\t' || *buffer_pointer == '\r')
            buffer_pointer++;

        if (*buffer_pointer == '\0')
            break;

        char name_of_command[50] = {0};
        int index = 0;

        while (*buffer_pointer != ' ' && *buffer_pointer != '\n' && *buffer_pointer != '\t' && *buffer_pointer != '\0' && index < 49)
        {
            name_of_command[index] = *buffer_pointer;
            index = index + 1;
            buffer_pointer = buffer_pointer + 1;
        }

        name_of_command[index] = '\0'; // завершаем строку

        code_type code_of_command = get_operation_code(name_of_command);

        if (code_of_command == code_SHIT)
        {
            fprintf(stderr, "Unknown command: %s\n", name_of_command);
            fclose(binary_file);

            return ASM_UNKNOWN_COMMAND_ERROR;
        }

        fwrite(&code_of_command, sizeof(code_type), 1, binary_file);

        if (code_of_command == code_PUSH || code_of_command == code_POP)
        {
            while (*buffer_pointer == ' ' || *buffer_pointer == '\t')
            {
                buffer_pointer++;
            }

            type_of_element argument = 0;
            if (sscanf(buffer_pointer, "%d", &argument) == 1) // именно sscanf
            {
                fwrite(&argument, sizeof(type_of_element), 1, binary_file);

                while (*buffer_pointer != ' ' && *buffer_pointer != '\n' && *buffer_pointer != '\t' && *buffer_pointer != '\0')
                    buffer_pointer++;
            }
            else
                fprintf(stderr, "Failed to read argument for command: %s\n", name_of_command);
        }

        number_of_commands++;
    }

    fclose(binary_file);
    
    return ASM_NO_ERROR;
}


assembler_type_error assembler_constructor(assembler* assembler_pointer, const char* input_filename, const char* output_filename)
{
    assert(assembler_pointer != NULL);
    assert(input_filename    != NULL);
    assert(output_filename   != NULL);

    assembler_pointer -> instruction_buffer   = NULL;
    assembler_pointer -> instruction_filename = NULL;
    assembler_pointer -> binary_buffer        = NULL;
    assembler_pointer -> binary_filename      = NULL;
    assembler_pointer -> binary_file          = NULL;

    assembler_pointer -> instruction_filename = strdup(input_filename);
    assembler_pointer -> binary_filename      = strdup(output_filename);

    if (assembler_pointer -> instruction_filename == NULL || assembler_pointer -> binary_filename == NULL)
    {
        assembler_destructor(assembler_pointer);

        return ASM_ALLOCATION_FAILED_ERROR;
    }

    FILE *instruction_file = fopen(input_filename, "r");
     if (instruction_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open instruction_file.txt\n");
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }
    fclose(instruction_file); // закрываем, так как будем читать через read_from_instruction_file_to_buffer

    FILE *binary_file = fopen(output_filename, "wb");
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open binary file for writing\n");
        return ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR;
    }
    fclose(binary_file); // Закрываем, так как будем писать через from_buffer_to_binary_file

    assembler_type_error error = read_from_instruction_file_to_buffer(assembler_pointer);

    if (error != ASM_NO_ERROR)
    {
        fclose(assembler_pointer -> binary_file);
        assembler_pointer -> binary_file = NULL;

        return error;
    }

    size_t max_possible_commands = strlen(assembler_pointer -> instruction_buffer) / 2 + 1; //оценка

    assembler_pointer -> binary_buffer = (int*)calloc(max_possible_commands, sizeof(int));
    if (assembler_pointer -> binary_buffer == NULL)
    {
        free(assembler_pointer -> instruction_buffer);
        assembler_pointer -> instruction_buffer = NULL;
        fclose(instruction_file);

        return ASM_ALLOCATION_FAILED_ERROR;
    }

    return ASM_NO_ERROR;
}


void assembler_destructor(assembler* assembler_pointer)
{
    assert(assembler_pointer != NULL);

    if (assembler_pointer -> instruction_buffer != NULL)
    {
        free(assembler_pointer -> instruction_buffer);
        assembler_pointer -> instruction_buffer = NULL;
    }

    if (assembler_pointer -> binary_buffer != NULL)
    {
        free(assembler_pointer -> binary_buffer);
        assembler_pointer -> binary_buffer = NULL;
    }

    if (assembler_pointer -> binary_filename != NULL)
    {
        free(assembler_pointer -> binary_filename);
        assembler_pointer -> binary_filename = NULL;
    }

    if (assembler_pointer -> instruction_filename != NULL)
    {
        free(assembler_pointer -> instruction_filename);
        assembler_pointer -> instruction_filename = NULL;
    }

//     if (assembler_pointer->binary_file != NULL) // закрываем файлы, если они открыты
//     {
//         fclose(assembler_pointer -> binary_file);
//         assembler_pointer -> binary_file = NULL;
//     }
//
//     if (assembler_pointer -> instruction_file != NULL)
//     {
//         fclose(assembler_pointer -> instruction_file);
//         assembler_pointer -> instruction_file = NULL;
//     }
}


size_t get_file_size(FILE *file)
{
    assert(file != NULL);

    struct stat stat_buffer = {};

    if (stat(file, &stat_buffer) != 0)
    {
        fprintf(stderr,RED "Error: Cannot open file\n" RESET);
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }

    return stat_buffer.st_size;
}


void asm_error_translator(assembler_type_error error)
{
    switch (error)
    {
        case ASM_NO_ERROR:
            printf("Assembly completed successfully!\n");
            break;
        case ASM_CANNOT_OPEN_INPUT_FILE_ERROR:
            fprintf(stderr, "Error: Cannot open input file\n");
            break;
        case ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR:
            fprintf(stderr, "Error: Cannot open output file\n");
            break;
        case ASM_READING_FILE_ERROR:
            fprintf(stderr, "Error: Reading from file failed\n");
            break;
        case ASM_ALLOCATION_FAILED_ERROR:
            fprintf(stderr, "Error: Memory allocation failed\n");
            break;
        case ASM_UNKNOWN_COMMAND_ERROR:
            fprintf(stderr, "Error: Unknown command in source file\n");
            break;
        case ASM_INVALID_ARGUMENT_ERROR:
            fprintf(stderr, "Error: Invalid argument for command\n");
            break;
        case ASM_INCORRECT_NUMBER_OF_ARGUMENTS:
            fprintf(stderr, "Error: Incorrect number of arguments\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error occurred\n");
            break;
    }
}

