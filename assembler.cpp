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

int command_requires_argument(code_type operation)
{
    switch (operation)
    {
        case code_PUSH:
        case code_POPR:
        case code_PUSHR:
        case code_JMP:
        case code_JB:
        case code_JBE:
        case code_JA:
        case code_JAE:
        case code_JE:
        case code_JNE:
            return 1;

        default:
            return 0;
    }
}


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

    if (strcmp(name_of_command, "IN") == 0)
        return code_IN;

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

    assembler_pointer -> instruction_buffer = (char*)calloc(size_of_file + 1, sizeof(char));

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


assembler_type_error first_pass(assembler *assembler_pointer)
{
    assert(assembler_pointer != NULL);
    assert(assembler_pointer -> instruction_buffer != NULL);

    char token[MAX_COMMAND_LENGTH] = {};
    int current_address = 0;
    char* buffer_ptr = assembler_pointer -> instruction_buffer;

    while (sscanf(buffer_ptr, "%31s", token) == 1)
    {
        buffer_ptr += strlen(token);
        while (isspace(*buffer_ptr))
            buffer_ptr++;

        if (token[0] == label_id_symbol)
        {
            assembler_type_error error = add_label(&assembler_pointer -> list_of_labels, token + 1, current_address);
            if (error != ASM_NO_ERROR)
            {
                fprintf(stderr, "Error: Label table full or duplicate label '%s'\n", token);
                return error;
            }
            continue; // не увеличиваем адрес для меток
        }

        code_type operation_code = get_operation_code(token);

        if (operation_code == code_SHIT)
        {
            register_code reg = get_register_by_name(token);
            if (reg == REG_INVALID)
                return ASM_UNKNOWN_COMMAND_ERROR;
            continue;
        }

        if (command_requires_argument(operation_code))
            current_address += 2;
        else
            current_address += 1;

        if (command_requires_argument(operation_code))
        {
            char next_token[MAX_COMMAND_LENGTH] = {};
            if (sscanf(buffer_ptr, "%31s", next_token) == 1)
            {
                buffer_ptr += strlen(next_token);
                while (isspace(*buffer_ptr))
                    buffer_ptr++;
            }
        }
    }

    printf("First pass completed. Code size: %d\n", current_address);
    return ASM_NO_ERROR;
}


assembler_type_error second_pass(assembler *assembler_pointer)
{
    assert(assembler_pointer                     != NULL);
    assert(assembler_pointer->instruction_buffer != NULL);

    FILE *binary_file = fopen(assembler_pointer->binary_filename, "wb");
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open binary file for writing\n");
        return ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR;
    }

    char token[MAX_COMMAND_LENGTH] = {};
    int argument = 0;
    int commands_processed = 0;
    char* buffer_ptr = assembler_pointer -> instruction_buffer;

    size_t buffer_size = strlen(assembler_pointer -> instruction_buffer) * 2; // грубая оценка
    int* binary_buffer = (int*)calloc(buffer_size, sizeof(int));
    if (binary_buffer == NULL)
    {
        fclose(binary_file);
        return ASM_ALLOCATION_FAILED_ERROR;
    }

    size_t binary_index = 0;

    while (sscanf(buffer_ptr, "%31s", token) == 1)
    {
        buffer_ptr += strlen(token);
        while (isspace(*buffer_ptr))
            buffer_ptr++;

        // Пропускаем метки
        if (token[0] == label_id_symbol)
            continue;

        code_type operation_code = get_operation_code(token);
        if (operation_code == code_SHIT)
        {
            free(binary_buffer);
            fclose(binary_file);
            return ASM_UNKNOWN_COMMAND_ERROR;
        }

        switch(operation_code)
        {
            case code_PUSH:
                binary_buffer[binary_index++] = operation_code;
                if (sscanf(buffer_ptr, "%d", &argument) == 1)
                {
                    binary_buffer[binary_index++] = argument;

                    char arg_buffer[32] = {};
                    sprintf(arg_buffer, "%d", argument);
                    buffer_ptr += strlen(arg_buffer);
                    while (isspace(*buffer_ptr))
                        buffer_ptr++;
                }
                else
                {
                    free(binary_buffer);
                    fclose(binary_file);
                    return ASM_INVALID_ARGUMENT_ERROR;
                }
                break;

            case code_JMP:
            case code_JB:
            case code_JBE:
            case code_JA:
            case code_JAE:
            case code_JE:
            case code_JNE:
                binary_buffer[binary_index++] = operation_code;
                {
                    char label_name[MAX_LABEL_LENGTH] = {};
                    if (sscanf(buffer_ptr, "%31s", label_name) == 1)
                    {
                        int label_address = find_label(&assembler_pointer -> list_of_labels, label_name);
                        if (label_address == -1)
                        {
                            free(binary_buffer);
                            fclose(binary_file);
                            return ASM_ERROR_UNDEFINED_LABEL;
                        }

                        binary_buffer[binary_index++] = label_address;
                        buffer_ptr += strlen(label_name);
                        while (isspace(*buffer_ptr))
                            buffer_ptr++;
                    }
                    else
                    {
                        free(binary_buffer);
                        fclose(binary_file);
                        return ASM_INVALID_ARGUMENT_ERROR;
                    }
                }
                break;

            case code_POPR:
            case code_PUSHR:
                binary_buffer[binary_index++] = operation_code;
                {
                    char register_name[MAX_COMMAND_LENGTH] = {};
                    if (sscanf(buffer_ptr, "%31s", register_name) == 1)
                    {
                        register_code reg = get_register_by_name(register_name);
                        if (reg == REG_INVALID)
                        {
                            free(binary_buffer);
                            fclose(binary_file);
                            return ASM_ERROR_INVALID_REGISTER;
                        }

                        binary_buffer[binary_index++] = (int)reg;
                        buffer_ptr += strlen(register_name);
                        while (isspace(*buffer_ptr))
                            buffer_ptr++;
                    }
                    else
                    {
                        free(binary_buffer);
                        fclose(binary_file);
                        return ASM_ERROR_EXPECTED_REGISTER;
                    }
                }
                break;

            case code_ADD:
            case code_SUB:
            case code_MUL:
            case code_DIV:
            case code_SQRT:
            case code_OUT:
            case code_IN:
            case code_POP:
            case code_HLT:
                binary_buffer[binary_index++] = operation_code;
                break;

            case code_SHIT:
            default:
                free(binary_buffer);
                fclose(binary_file);
                return ASM_UNKNOWN_COMMAND_ERROR;
        }

        commands_processed++;
        if (operation_code == code_HLT)
            break;
    }

    size_t written = fwrite(binary_buffer, sizeof(int), binary_index, binary_file);
    free(binary_buffer);
    fclose(binary_file);

    if (written != binary_index)
    {
        return ASM_READING_FILE_ERROR;
    }

    printf("Second pass completed. Processed %d commands\n", commands_processed);
    return ASM_NO_ERROR;
}


assembler_type_error from_buffer_to_binary_file(assembler *assembler_pointer)
{
    assert(assembler_pointer                       != NULL);
    assert(assembler_pointer -> instruction_buffer != NULL);
    assert(assembler_pointer -> binary_filename    != NULL);

    printf("=== FIRST PASS ===\n");
    assembler_type_error error = first_pass(assembler_pointer);
    if (error != ASM_NO_ERROR)
        return error;

    printf("=== SECOND PASS ===\n");
    error = second_pass(assembler_pointer);
    if (error != ASM_NO_ERROR)
        return error;

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

    init_label_table(&assembler_pointer -> list_of_labels);

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
        case ASM_ERROR_LABEL_TABLE:
            fprintf(stderr, "Error: Label table overflow\n");
            break;
        case ASM_ERROR_REDEFINITION_LABEL:
            fprintf(stderr, "Error: Label redefinition\n");
            break;
        case ASM_ERROR_UNDEFINED_LABEL:
            fprintf(stderr, "Error: Undefined label\n");
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


void init_label_table(label_table* ptr_table)
{
    assert(ptr_table != NULL);

    ptr_table -> number_of_labels = 0;

    for (int index_of_label = 0; index_of_label < MAX_NUMBER_OF_LABELS; index_of_label++)
    {
        for (int index_of_char_in_name = 0; index_of_char_in_name < MAX_LABEL_LENGTH; index_of_char_in_name++)
        {
            ptr_table -> labels[index_of_label].name[index_of_char_in_name] = '\0';
        }
        ptr_table -> labels[index_of_label].address = -1;
    }
}

int find_label(label_table* ptr_table, const char* name_of_label)
{
    assert(ptr_table     != NULL);
    assert(name_of_label != NULL);

    for (int i = 0; i < ptr_table -> number_of_labels; i++)
    {
        if (strcmp(ptr_table -> labels[i].name, name_of_label) == 0)
            return ptr_table -> labels[i].address;
    }
    return -1;
}

assembler_type_error add_label(label_table* ptr_table, const char* name_of_label, int address)
{
    assert(ptr_table     != NULL);
    assert(name_of_label != NULL);

    if (ptr_table -> number_of_labels >= MAX_NUMBER_OF_LABELS)
        return ASM_ERROR_LABEL_TABLE;

    if (find_label(ptr_table, name_of_label) != -1)
        return ASM_ERROR_REDEFINITION_LABEL;

    strncpy(ptr_table -> labels[ptr_table -> number_of_labels].name, name_of_label, MAX_LABEL_LENGTH - 1);
    ptr_table -> labels[ptr_table -> number_of_labels].name[MAX_LABEL_LENGTH - 1] = '\0';
    ptr_table -> labels[ptr_table -> number_of_labels].address = address;
    ptr_table -> number_of_labels++;

    printf("Added label: '%s' at address %d\n", name_of_label, address);
    return ASM_NO_ERROR;
}
