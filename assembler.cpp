#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include "common.h"
#include "assembler.h"
#include "calculator.h"
#include "colour_codes.h"
#include "asm_error_types.h"

const int MAX_SIZE_OF_REGISTERS = 32;

code_type get_operation_code(const char* name_of_command)
{
    assert(name_of_command != NULL);

    if (strcmp(name_of_command, "HLT") == 0)
        return code_HLT;

    if (strcmp(name_of_command, "PUSH") == 0)
        return code_PUSH;

    if (strcmp(name_of_command, "POP") == 0)
        return code_POP;

    if (strcmp(name_of_command, "POPR") == 0)
        return code_POPR;

    if (strcmp(name_of_command, "PUSHR") == 0)
        return code_PUSHR;

    if (strcmp(name_of_command, "JMP") == 0)
        return code_JMP;

    if (strcmp(name_of_command, "JB") == 0)
        return code_JB;

    if (strcmp(name_of_command, "JBE") == 0)
        return code_JBE;

    if (strcmp(name_of_command, "JA") == 0)
        return code_JA;

    if (strcmp(name_of_command, "JAE") == 0)
        return code_JAE;

    if (strcmp(name_of_command, "JE") == 0)
        return code_JE;

    if (strcmp(name_of_command, "JNE") == 0)
        return code_JNE;

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
        fprintf(stderr, "Error: Cannot open file: %s\n", assembler_pointer -> instruction_filename);
        return ASM_CANNOT_OPEN_INPUT_FILE_ERROR;
    }

    size_t size_of_file = get_file_size(instruction_file);

    if (size_of_file == 0)
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

    char *buffer_ptr = assembler_pointer -> instruction_buffer;
    char name_of_command[50] = {};
    size_t number_of_commands = 0;
    int bytes_read = 0;

    while (sscanf(buffer_ptr, "%49s%n", name_of_command, &bytes_read) == 1)
    {
        buffer_ptr += bytes_read;

        printf("Processing command: '%s'\n", name_of_command);

        code_type code_of_command = get_operation_code(name_of_command);

        if (code_of_command == code_SHIT)
        {
            fprintf(stderr, "Unknown command: '%s'\n", name_of_command);
            fclose(binary_file);
            return ASM_UNKNOWN_COMMAND_ERROR;
        }

        fwrite(&code_of_command, sizeof(code_type), 1, binary_file);

        if (code_of_command == code_PUSH || code_of_command == code_POP)
        {
            type_of_element number_arg = 0;
            if (sscanf(buffer_ptr, "%d%n", &number_arg, &bytes_read) == 1)
            {
                fwrite(&number_arg, sizeof(type_of_element), 1, binary_file);
                buffer_ptr += bytes_read;
            }

            else
            {
                fprintf(stderr, "Expected number argument for command: %s\n", name_of_command);
                fclose(binary_file);
                return ASM_INVALID_ARGUMENT_ERROR;
            }
        }

        else if (code_of_command == code_PUSHR || code_of_command == code_POPR)
        {
            char register_name[MAX_SIZE_OF_REGISTERS] = {};
            if (sscanf(buffer_ptr, "%31s%n", register_name, &bytes_read) != 1)
            {
                fprintf(stderr, "Expected register for command: %s\n", name_of_command);
                fclose(binary_file);
                return ASM_ERROR_EXPECTED_REGISTER;
            }

            register_code reg = get_register_by_name(register_name);
            if (reg == REG_INVALID)
            {
                fprintf(stderr, "Invalid register: %s\n", register_name);
                fclose(binary_file);
                return ASM_ERROR_INVALID_REGISTER;
            }

            fwrite(&reg, sizeof(register_code), 1, binary_file);
            buffer_ptr += bytes_read;
        }
        
        else if (code_of_command >= code_JMP && code_of_command <= code_JNE)
        {
            int jump_address = 0;

            if (sscanf(buffer_ptr, "%d%n", &jump_address, &bytes_read) == 1)
            {
                fwrite(&jump_address, sizeof(int), 1, binary_file);
                buffer_ptr += bytes_read;
            }

            else
            {
                fprintf(stderr, "Expected jump address for command: %s\n", name_of_command);
                fclose(binary_file);
                return ASM_INVALID_ARGUMENT_ERROR;
            }
        }

        number_of_commands++;

        while (*buffer_ptr == ' ' || *buffer_ptr == '\t' || *buffer_ptr == '\n' || *buffer_ptr == '\r')
            buffer_ptr++;
    }

    fclose(binary_file);
    printf("Processed %zu commands successfully\n", number_of_commands);

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

    assembler_pointer -> instruction_filename = strdup(input_filename);
    assembler_pointer -> binary_filename      = strdup(output_filename);

    if (assembler_pointer -> instruction_filename == NULL || assembler_pointer -> binary_filename == NULL)
    {
        assembler_destructor(assembler_pointer);

        return ASM_ALLOCATION_FAILED_ERROR;
    }

    assembler_type_error error = read_from_instruction_file_to_buffer(assembler_pointer);
    if (error != ASM_NO_ERROR)
    {
        assembler_destructor(assembler_pointer);
        return error;
    }

    size_t max_possible_commands = strlen(assembler_pointer -> instruction_buffer) / 2 + 1; // Выделяем память для бинарного буфера
    assembler_pointer -> binary_buffer = (int*)calloc(max_possible_commands, sizeof(int));
    if (assembler_pointer -> binary_buffer == NULL)
    {
        assembler_destructor(assembler_pointer);
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
        case ASM_CANNOT_GET_FILE_DESCRIPTOR_ERROR:
            fprintf(stderr, "Error: Cannot get file descriptor\n");
            break;
        case ASM_ERROR_INVALID_REGISTER:
            fprintf(stderr, "Error: Used invalid register\n");
            break;
        case ASM_ERROR_EXPECTED_REGISTER:
            fprintf(stderr, "Error: Get wrong unexpected register\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error code: %d\n", error);
            break;
    }
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
