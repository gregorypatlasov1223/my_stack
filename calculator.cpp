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
    FILE *output_file = fopen("output_file.txt", "w");;

    char name_of_command[50] = {0};

    const char *type_of_command = NULL;
    code_type code_of_command = BAD_VALUE;

    type_of_element popped_value = 0;
    type_of_element number = 0;



    while (strcmp(name_of_command, "HLT"))
    {
        if (get_command(type_of_command, &code_of_command, &number, name_of_command, input_file) == 0)
        {
            fprintf(stderr, "The command wasn't read. Try again\n");
            continue;
        }

        switch(code_of_command)
        {
            case code_PUSH:
                stack_push(stack, number);
                break;
            case code_POP:
                stack_pop(stack, &popped_value);
                fprintf(output_file, "Popped value: %d\n", popped_value);
                break;
            case code_ADD:
                stack_add(stack);
                break;
            case code_MUL:
                stack_mul(stack);
                break;
            case code_SUB:
                stack_sub(stack);
                break;
            case code_DIV:
                stack_div(stack);
                break;
            case code_SQRT:
                stack_sqrt(stack);
                break;
            case code_OUT:
                print_stack(stack, output_file);
                break;
            case code_HLT:
                break;
            default:
                fprintf(stderr, "Syntax error. The command wasn't read. Try again\n");
        }

        fprintf(output_file, "File has been read\n");
        fclose(input_file);
        fclose(output_file);
    }

    return NO_ERROR;
}


int get_command(const char *type_of_command, code_type *code_command, type_of_element *number,
                char *name_of_command, FILE *input_file)
{
    assert(number != NULL);
    assert(input_file != NULL);
    assert(code_command != NULL);
    assert(type_of_command != NULL);
    assert(name_of_command != NULL);

    *code_command = code_SHIT;

    if (fscanf(input_file, "%49s", name_of_command) != 1)
    {
        fprintf(stderr, "ERROR IN READING\n");

        return 0;
    }

    if (strcmp("PUSH", name_of_command) == 0)
    {
        *code_command = code_PUSH;

        if (fscanf(input_file, "%d", number) != 1)
        {
            printf("The number wasn't read\n");
            return 0;
        }
    }

    else if (strcmp("PUSH", name_of_command) == 0)
        *code_command = code_PUSH;

    else if (strcmp("POP", name_of_command) == 0)
        *code_command = code_POP;

    else if (strcmp("ADD", name_of_command) == 0)
        *code_command = code_ADD;

    else if (strcmp("MUL", name_of_command) == 0)
        *code_command = code_MUL;

    else if (strcmp("SUB", name_of_command) == 0)
        *code_command = code_SUB;

    else if (strcmp("PUSH", name_of_command) == 0)
        *code_command = code_PUSH;

    else if (strcmp("DIV", name_of_command) == 0)
        *code_command = code_DIV;

    else if (strcmp("SQRT", name_of_command) == 0)
        *code_command = code_SQRT;

    else if (strcmp("HLT", name_of_command) == 0)
        *code_command = code_HLT;

    else
        return 0;

    return 1;
}



