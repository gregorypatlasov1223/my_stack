#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include "stack.h"
#include "common.h"
#include "processor.h"
#include "calculator.h"
#include "arithm_oper.h"
#include "colour_codes.h"
#include "proc_error_types.h"

void proc_error_translator(processor_error_type error)
{
    switch(error)
    {
        case PROC_ERROR_NO:
            printf(GREEN "PROC_ERROR_NO\n" RESET);
            break;

        case PROC_ERROR_ALLOCATION_FAILED:
            printf(RED "PROC_ERROR_ALLOCATION_FAILED\n" RESET);
            break;

        case PROC_ERROR_CANNOT_OPEN_BINARY_FILE:
            printf(RED "PROC_ERROR_CANNOT_OPEN_BINARY_FILE\n" RESET);
            break;

        case PROC_ERROR_READING_FILE:
            printf(RED "PROC_ERROR_READING_FILE\n" RESET);
            break;

        case PROC_ERROR_UNKNOWN_OPERATION_CODE:
            printf(RED "PROC_ERROR_UNKNOWN_OPERATION_CODE\n" RESET);
            break;

        case PROC_ERROR_STACK_OPERATION_FAILED:
            printf(RED "PROC_ERROR_STACK_OPERATION_FAILED\n" RESET);
            break;

        case PROC_ERROR_INVALID_STATE:
            printf(RED "PROC_ERROR_INVALID_STATE\n" RESET);
            break;

        case PROC_ERROR_INVALID_JUMP:
            printf(RED "PROC_ERROR_INVALID_JUMP\n" RESET);
            break;

        default:
            printf(RED "UNKNOWN_ERROR (%d)\n" RESET, error);
            break;
    }
}


processor_error_type execute_processor(processor *processor_pointer)
{
    assert(processor_pointer != NULL);
    assert(processor_pointer -> code_buffer != NULL);

    processor_error_type proc_error = PROC_ERROR_NO;
    stack_err_t stack_error = NO_ERROR;

    while (processor_pointer -> instruction_counter < (int)processor_pointer -> code_buffer_size)
    {
        int current_instruction_counter = processor_pointer -> instruction_counter;
        code_type operation_code = (code_type)processor_pointer -> code_buffer[current_instruction_counter];
        int argument = 0;

        register_code reg = REG_INVALID;

        if (operation_code == code_PUSH || operation_code == code_PUSHR || operation_code == code_POPR ||
           (operation_code >= code_JMP && operation_code <= code_JNE))
        {
            if (current_instruction_counter + 1 >= (int)processor_pointer -> code_buffer_size)
                return PROC_ERROR_INVALID_STATE;

            argument = processor_pointer -> code_buffer[current_instruction_counter + 1];
        }

        switch(operation_code)
        {
            case code_PUSH:
            case code_POP:
            case code_PUSHR:
            case code_POPR:
                proc_error = execute_command_with_stack(processor_pointer, operation_code, argument);
                break;

            case code_ADD:
            case code_SUB:
            case code_MUL:
            case code_DIV:
            case code_SQRT:
                proc_error = execute_math_operation_command(processor_pointer, operation_code);
                break;

            case code_JMP:
            case code_JB:
            case code_JBE:
            case code_JA:
            case code_JAE:
            case code_JE:
            case code_JNE:
                proc_error = execute_jump_command(processor_pointer, operation_code, argument);
                break;

            case code_IN:
            {
                type_of_element value = 0;
                printf("Enter a number: ");
                if (scanf("%d", &value) == 1)
                    stack_error = stack_push(&processor_pointer -> stack, value);
                else
                    return PROC_ERROR_INVALID_STATE;
                processor_pointer -> instruction_counter += 1;
                break;
            }

            case code_OUT:
            {
                type_of_element value = 0;
                stack_error = stack_pop(&processor_pointer -> stack, &value);
                if (stack_error == NO_ERROR)
                    printf("OUT -> %d\n", value);
                processor_pointer -> instruction_counter += 1;
                break;
            }

            case code_HLT:
                return PROC_ERROR_NO;

            case code_SHIT:
            default:
                return PROC_ERROR_UNKNOWN_OPERATION_CODE;
        }

        if (proc_error != PROC_ERROR_NO || stack_error != NO_ERROR)
        {
            processor_dump(processor_pointer, proc_error, "Processor execution failed");
            return proc_error;
        }
    }

    return PROC_ERROR_NO;
}


processor_error_type read_binary_file_to_buffer(processor* processor_pointer, const char* binary_filename)
{
    assert(processor_pointer != NULL);
    assert(binary_filename   != NULL);

    FILE* binary_file = fopen(binary_filename, "rb");
    if (binary_file == NULL)
        return PROC_ERROR_CANNOT_OPEN_BINARY_FILE;

    size_t file_size = get_file_size(binary_file);
    if (file_size <= 0 || file_size % sizeof(int) != 0)
    {
        fclose(binary_file);
        return PROC_ERROR_READING_FILE;
    }

    size_t total_ints = file_size / sizeof(int);

    processor_pointer -> code_buffer = (int*)calloc(total_ints, sizeof(int));
    if (processor_pointer -> code_buffer == NULL)
    {
        fclose(binary_file);
        return PROC_ERROR_ALLOCATION_FAILED;
    }

    size_t elements_read = fread(processor_pointer -> code_buffer, sizeof(int), total_ints, binary_file);
    fclose(binary_file);

    if (elements_read != total_ints)
    {
        free(processor_pointer -> code_buffer);
        processor_pointer -> code_buffer = NULL;
        return PROC_ERROR_READING_FILE;
    }

    processor_pointer -> code_buffer_size = total_ints;

    return PROC_ERROR_NO;
}


processor_error_type processor_constructor(processor* processor_pointer, size_t starting_capacity)
{
    assert(processor_pointer != NULL);
    assert(starting_capacity > 0);

    stack_err_t stack_constructor_result = stack_constructor(&(processor_pointer -> stack), starting_capacity);

    if (stack_constructor_result != NO_ERROR)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
        processor_pointer -> registers[i] = 0;

    processor_pointer -> instruction_counter = 0;
    processor_pointer -> code_buffer_size    = 0;
    processor_pointer -> code_buffer         = NULL;

    return PROC_ERROR_NO;
}


void processor_destructor(processor *processor_pointer)
{
    assert(processor_pointer != NULL);

    if (processor_pointer -> code_buffer != NULL)
        free(processor_pointer -> code_buffer);

    stack_destructor(&(processor_pointer -> stack));

    for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
        processor_pointer -> registers[i] = 0;

    processor_pointer -> instruction_counter = 0;
    processor_pointer -> code_buffer_size    = 0;
    processor_pointer -> code_buffer         = NULL;
}


void processor_dump(processor* proc, processor_error_type code_error, const char* message)
{
    assert(message != NULL);

    if (proc == NULL)
    {
        printf("Processor [%p] %s", proc, message);
        return;
    }

    printf("Processor [%p] Dump: %s\n", proc, message);

    stack_dump(&(proc -> stack), NO_ERROR, __FILE__, __func__, __LINE__); 

    printf("Processor error: ");
    proc_error_translator(code_error);

    printf("Instruction counter: %d\n",  proc -> instruction_counter);
    printf("Code buffer size:    %lu\n", proc -> code_buffer_size);
    printf("Code buffer address: %p\n",  proc -> code_buffer);


    if (proc -> code_buffer != NULL)
    {
        printf("Code buffer content (first 10 elements):\n"); //  вывод содержимого буфера кода
        for (size_t i = 0; i < proc -> code_buffer_size && i < 10; i++)
            printf("  [%zu] = %d\n", i, proc -> code_buffer[i]);
    }
}


processor_error_type processor_JB(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a < b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type processor_JBE(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a <= b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type processor_JA(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a > b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type processor_JAE(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a >= b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type processor_JE(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a == b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type processor_JNE(processor* processor_pointer, int argument)
{
    type_of_element a = 0, b = 0;
    stack_err_t stack_error = NO_ERROR;

    stack_error = stack_pop(&processor_pointer -> stack, &b);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_error = stack_pop(&processor_pointer -> stack, &a);
    if (stack_error != NO_ERROR) return PROC_ERROR_STACK_OPERATION_FAILED;

    if (a != b)
    {
        if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
            return PROC_ERROR_INVALID_JUMP;

        processor_pointer -> instruction_counter = argument;
    }
    return PROC_ERROR_NO;
}


processor_error_type execute_math_operation_command(processor* processor_pointer, code_type operation_code)
{
    assert(processor_pointer != NULL);

    stack_err_t stack_error = NO_ERROR;

    switch(operation_code)
    {
        case code_ADD:
            stack_error = stack_add(&processor_pointer -> stack);
            break;

        case code_SUB:
            stack_error = stack_sub(&processor_pointer -> stack);
            break;

        case code_MUL:
            stack_error = stack_mul(&processor_pointer -> stack);
            break;

        case code_DIV:
            stack_error = stack_div(&processor_pointer -> stack);
            break;

        case code_SQRT:
            stack_error = stack_sqrt(&processor_pointer -> stack);
            break;

        default:
            return PROC_ERROR_UNKNOWN_OPERATION_CODE;
    }

    processor_pointer -> instruction_counter += 1;

    if (stack_error == NO_ERROR)
        return PROC_ERROR_NO;

    return PROC_ERROR_STACK_OPERATION_FAILED;
}


processor_error_type execute_command_with_stack(processor* processor_pointer, code_type operation_code, int argument)
{
    assert(processor_pointer != NULL);

    stack_err_t stack_error = NO_ERROR;
    register_code reg = REG_INVALID;

    switch(operation_code)
    {
        case code_PUSH:
            stack_error = stack_push(&processor_pointer -> stack, argument);
            processor_pointer -> instruction_counter += 2;
            break;

        case code_POP:
        {
            type_of_element popped_value = 0;
            stack_error = stack_pop(&processor_pointer -> stack, &popped_value);
            processor_pointer -> instruction_counter += 1;
            break;
        }

        case code_PUSHR:
            if (argument >= REG_RAX && argument <= REG_RHX)
            {
                reg = (register_code)argument;
                int register_value = processor_pointer -> registers[reg];
                stack_error = stack_push(&processor_pointer -> stack, register_value);
            }
            else
                return PROC_ERROR_INVALID_STATE;

            processor_pointer -> instruction_counter += 2;
            break;

        case code_POPR:
            if (argument >= REG_RAX && argument <= REG_RHX)
            {
                type_of_element popped_value = 0;
                stack_error = stack_pop(&processor_pointer -> stack, &popped_value);
                if (stack_error == NO_ERROR)
                {
                    reg = (register_code)argument;
                    processor_pointer -> registers[reg] = popped_value;
                }
            }
            else
                return PROC_ERROR_INVALID_STATE;

            processor_pointer -> instruction_counter += 2;
            break;

        default:
            return PROC_ERROR_UNKNOWN_OPERATION_CODE;
    }

    if (stack_error == NO_ERROR)
        return PROC_ERROR_NO;

    return PROC_ERROR_STACK_OPERATION_FAILED;
}


processor_error_type execute_jump_command(processor* processor_pointer, code_type operation_code, int argument)
{
    assert(processor_pointer != NULL);

    processor_error_type proc_error = PROC_ERROR_NO;
    int current_instruction_counter = processor_pointer -> instruction_counter;

    switch(operation_code)
    {
        case code_JMP:
            if (argument < 0 || argument >= (int)processor_pointer -> code_buffer_size)
                return PROC_ERROR_INVALID_JUMP;

            processor_pointer -> instruction_counter = argument;
            break;

        case code_JB:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JB(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        case code_JBE:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JBE(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        case code_JA:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JA(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        case code_JAE:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JAE(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        case code_JE:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JE(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        case code_JNE:
            if (processor_pointer -> stack.size < 2)
                return PROC_ERROR_STACK_OPERATION_FAILED;

            proc_error = processor_JNE(processor_pointer, argument);

            if (proc_error != PROC_ERROR_NO)
                return proc_error;

            if (processor_pointer -> instruction_counter == current_instruction_counter)
                processor_pointer -> instruction_counter += 2;
            break;

        default:
            return PROC_ERROR_UNKNOWN_OPERATION_CODE;
    }

    return proc_error;
}

