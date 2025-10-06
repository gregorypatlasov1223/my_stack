#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "stack.h"
#include "calculator.h"
#include "error_types.h"



stack_err_t calc_online(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    FILE *input_file = fopen("input_file.txt", "r");
    FILE *output_file = fopen("output_file.txt", "w");

    if (input_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open input_file.txt\n");

        return INITIALIZE_ERROR;
    }

    if (output_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open output_file.txt\n");
        fclose(input_file);

        return INITIALIZE_ERROR;
    }

    char name_of_command[50] = {0};

    code_type code_of_command = BAD_VALUE;

    type_of_element popped_value = 0;
    type_of_element number = 0;

    int should_continue = 1;

    while (should_continue)
    {
        if (get_command(&code_of_command, &number, name_of_command, input_file) == 0)
        {
            if (feof(input_file))
            {
                printf("End of file reached. Normal termination.\n");
                break;
            }
            
            fprintf(stderr, "The command wasn't read. Try again\n");
            continue;
        }

        if (code_of_command == code_HLT)
        {
            printf("HLT command received. Stopping.\n");
            break;
        }

        switch(code_of_command)
        {
            case code_PUSH:
                if (stack_push(stack, number) != NO_ERROR)
                    fprintf(stderr, "Push failed\n");
                break;

            case code_POP:
                if (stack_pop(stack, &popped_value) == NO_ERROR)
                    fprintf(output_file, "Popped value: %d\n", popped_value);
                else
                    fprintf(stderr, "Pop failed\n");
                break;

            case code_ADD:
                if (stack_add(stack) != NO_ERROR)
                    fprintf(stderr, "Add failed\n");
                break;

            case code_MUL:
                if (stack_mul(stack) != NO_ERROR)
                    fprintf(stderr, "Mul failed\n");
                break;

            case code_SUB:
                if (stack_sub(stack) != NO_ERROR)
                    fprintf(stderr, "Sub failed\n");
                break;

            case code_DIV:
                if (stack_div(stack) != NO_ERROR)
                    fprintf(stderr, "Div failed\n");
                break;

            case code_SQRT:
                if (stack_sqrt(stack) != NO_ERROR)
                    fprintf(stderr, "Sqrt failed\n");
                break;

            case code_OUT:
                if (print_stack(stack, output_file) != NO_ERROR)
                    fprintf(stderr, "Print failed\n");
                break;

            default:
                fprintf(stderr, "Syntax error. The command wasn't read. Try again\n");
        }
    }

    fprintf(output_file, "Calculation completed\n");
    fclose(input_file);
    fclose(output_file);

    return NO_ERROR;
}


int get_command(code_type *code_command, type_of_element *number,
                char *name_of_command, FILE *input_file)
{
    assert(number != NULL);
    assert(input_file != NULL);
    assert(code_command != NULL);
    assert(name_of_command != NULL);

    *code_command = code_SHIT;

    if (feof(input_file))
        return 0;

    if (fscanf(input_file, "%49s", name_of_command) != 1)
    {
        if (feof(input_file))
            return 0;

        fprintf(stderr, "ERROR IN READING command\n");
        return 0;
    }

    if (strcmp("PUSH", name_of_command) == 0)
    {
        *code_command = code_PUSH;
        if (fscanf(input_file, "%d", number) != 1)
        {
            fprintf(stderr, "The number for PUSH wasn't read\n");
            return 0;
        }
    }
    else if (strcmp("POP", name_of_command) == 0)
        *code_command = code_POP;

    else if (strcmp("ADD", name_of_command) == 0)
        *code_command = code_ADD;

    else if (strcmp("MUL", name_of_command) == 0)
        *code_command = code_MUL;

    else if (strcmp("SUB", name_of_command) == 0)
        *code_command = code_SUB;

    else if (strcmp("DIV", name_of_command) == 0)
        *code_command = code_DIV;

    else if (strcmp("SQRT", name_of_command) == 0)
        *code_command = code_SQRT;

    else if (strcmp("OUT", name_of_command) == 0)
        *code_command = code_OUT;

    else if (strcmp("HLT", name_of_command) == 0)
        *code_command = code_HLT;

    else {
        fprintf(stderr, "Unknown command: %s\n", name_of_command);
        return 0;
    }

    return 1;
}



