#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "error_types.h"
#include "arithm_oper.h"
#include "colour_codes.h"

stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    assert(value != NULL);

    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
    {
        if (code_error == OVER_FLOW_CAPACITY || code_error == OVER_FLOW_SIZE)
        {
            printf(RED "Error: Stack is overflowed. Pop is failed\n" RESET);
            return code_error;
        }
        else
        {
            stack_dump(stack, code_error, __FILE__, __func__, __LINE__);
            return code_error;
        }
    }

    if (stack -> size < 0)
    {
        fprintf(stderr, RED "Error: Stack is underflowed\n" RESET);
        return UNDER_FLOW_SIZE;
    }

    *value = stack -> array[stack -> size];  // берем элемент
    stack -> array[stack -> size] = POISON;  // очищаем ячейку
    stack -> size = stack -> size - 1;       // уменьшаем размер

    return NO_ERROR;
}


stack_err_t stack_push(stack_t *stack, type_of_element value)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
    {
        if (code_error == OVER_FLOW_CAPACITY || code_error == OVER_FLOW_SIZE)
        {
            printf(RED "Error: Stack is overflowed\n" RESET);
            return code_error;
        }
        else
        {
            stack_dump(stack, code_error, __FILE__, __func__, __LINE__);
            return code_error;
        }
    }

    if (stack->size >= get_data_capacity(stack -> capacity))
    {
        size_t new_capacity = (stack -> capacity) * MULTIPLIER;

        type_of_element *new_array = (type_of_element*)realloc(stack -> array, new_capacity * sizeof(type_of_element));

        if (new_array == NULL)
            return ARRAY_POINTER_ERROR;

        stack -> array = new_array;
        stack -> capacity = new_capacity;

        setup_canaries(stack);

        for (size_t index = (stack -> size) + 1; index < (stack -> capacity) - 1; index++)
            stack -> array[index] = POISON;
    }

    stack -> size = (stack -> size) + 1;
    stack -> array[stack -> size] = value;

    return NO_ERROR;
}


stack_err_t stack_constructor(stack_t *stack, size_t requested_capacity)
{
    if (stack == NULL)
        return STACK_POINTER_ERROR;

    if (requested_capacity == 0)
        return UNDER_FLOW_CAPACITY;

    size_t total_capacity = requested_capacity + NUMBER_OF_CANARIES;

    stack -> array = (type_of_element*)calloc(total_capacity, sizeof(type_of_element));
    if (stack->array == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> capacity = total_capacity;
    stack -> size = 0;

    setup_canaries(stack);

    for (size_t index = 1; index <= requested_capacity; index++)
        stack->array[index] = POISON;

    return NO_ERROR;
}


void stack_destructor(stack_t *stack)
{
    assert(stack != NULL);

    if (stack -> array != NULL)
        free(stack -> array);

    stack -> array = NULL;
    stack -> size = RESET_SIZE;
    stack -> capacity = RESET_CAPACITY;
}


stack_err_t stack_verify(stack_t *stack)
{
    stack_err_t code_error = NO_ERROR;
    if (stack == NULL)
    {
        code_error = STACK_POINTER_ERROR;

        return code_error;
    }

    if ((stack -> array) == NULL)
    {
        code_error = ARRAY_POINTER_ERROR;

        return code_error;
    }

    code_error = check_canaries(stack);
    if (code_error != NO_ERROR)
        return code_error;

    if ((stack -> size) > STACK_MAX_SIZE)
    {
        code_error = OVER_FLOW_SIZE;

        return code_error;
    }

    if ((stack -> capacity) > STACK_MAX_CAPACITY)
    {
        code_error = OVER_FLOW_CAPACITY;

        return code_error;
    }

    if ((stack -> size) > (stack -> capacity) - NUMBER_OF_CANARIES)
    {
        code_error = OVER_FLOW_CAPACITY;

        return code_error;
    }

    return NO_ERROR;
}


void stack_dump(stack_t *stack, stack_err_t code_error, const char *file_name, const char *function_name, int line)
{
    assert(file_name     != NULL);
    assert(function_name != NULL);

    printf("The type of error is ");
    error_translator(code_error);

    printf(CYAN "It was called from function %s at %s line %d\n" RESET, function_name, file_name, line);

    printf(MAGENTA "\nInformation about stack\n" RESET);
    printf(MAGENTA "size = %zu\n" RESET, stack -> size);
    printf(MAGENTA "capacity = %zu (total including canaries)\n" RESET, stack -> capacity);
    printf(MAGENTA "data capacity = %zu (available for data)\n" RESET, get_data_capacity(stack -> capacity));
    printf(MAGENTA "array[%p]\n" RESET, stack -> array);



    if ((code_error != ARRAY_POINTER_ERROR) && (stack -> array != NULL))
    {
        printf("Array content:\n");
        for (size_t index = 0; index < stack -> capacity; index++)
        {
            const char *marker = NULL;

            if (index == 0)
            {
                marker = (stack -> array[index] == CANARY_VALUE) ?
                         "[FRONT CANARY]" : "[FRONT CANARY DAMAGED]";
            }

            else if (index == (stack -> capacity) - 1)
            {
                marker = (stack -> array[index] == CANARY_VALUE) ?
                         "[BACK CANARY]" : "[BACK CANARY DAMAGED]";
            }

            else if (stack -> array[index] == POISON)
                marker = "[POISON]";

            else if (index <= stack -> size)
                marker = "[ACTIVE DATA]";

            else
                marker = "[FREE]";

            printf(YELLOW "[%zu] = " RESET, index);

            if (index == 0 || index == (stack -> capacity) - 1)
                printf(YELLOW "%d" RESET, stack->array[index]);
            else
                printf(YELLOW "%d" RESET, stack->array[index]);
            printf( YELLOW " %s\n" RESET, marker);
        }
    }
}


void error_translator(stack_err_t code_error)
{
    switch(code_error)
    {
        case ARRAY_POINTER_ERROR:
            printf(RED "ARRAY_POINTER_ERROR\n" RESET);
            break;
        case STACK_POINTER_ERROR:
            printf(RED "STACK_POINTER_ERROR\n" RESET);
            break;
        case INITIALIZE_ERROR:
            printf(RED "INITIALIZE_ERROR" RESET);
            break;
        case UNDER_FLOW_CAPACITY:
            printf(RED "UNDER_FLOW_CAPACITY\n" RESET);
            break;
        case OVER_FLOW_CAPACITY:
            printf(RED "OVER_FLOW_CAPACITY\n" RESET);
            break;
        case UNDER_FLOW_SIZE:
            printf(RED "UNDER_FLOW_SIZE\n" RESET);
            break;
        case OVER_FLOW_SIZE:
            printf(RED "OVER_FLOW_SIZE\n" RESET);
            break;
        case CANARY_DAMAGED:
            printf(RED "CANARY_DAMAGED\n" RESET);
            break;
        case ERROR_DIV_BY_0:
            printf(RED "ERROR_DIV_BY_0\n" RESET);
            break;
        case NO_ERROR:
            printf(GREEN "NO_ERROR\n" RESET);
            break;
        default:
            printf(RED "UNKNOWN_ERROR (%d)\n" RESET, code_error);
            break;
    }
}


size_t get_data_capacity(size_t total_capacity)
{
    if (total_capacity >= NUMBER_OF_CANARIES)
        return total_capacity - NUMBER_OF_CANARIES;
    else
        return 0;
}


stack_err_t check_canaries(const stack_t *stack)
{
    if (stack -> capacity < NUMBER_OF_CANARIES)
        return CANARY_DAMAGED;

    if (stack -> array[0] != (type_of_element)CANARY_VALUE)
        return CANARY_DAMAGED;

    if (stack -> array[stack -> capacity - 1] != (type_of_element)CANARY_VALUE)
        return CANARY_DAMAGED;

    return NO_ERROR;
}


stack_err_t setup_canaries(stack_t *stack)
{
    if (stack == NULL || stack -> array == NULL)
        return STACK_POINTER_ERROR;

    stack -> array[0] = (type_of_element)CANARY_VALUE;
    stack -> array[stack -> capacity - 1] = (type_of_element)CANARY_VALUE;

    return NO_ERROR;
}


stack_err_t print_calculated_value(stack_t *stack, FILE *output_file)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    size_t index = 1;
    printf(CYAN "Calculated value [%zu] = %d\n" RESET, index, stack -> array[index]);
    fprintf(output_file, "Calculated value [%zu] = %d\n", index, stack -> array[index]);

    // for (size_t index = 1; index <= stack -> size; index++)
    //     printf(CYAN "Calculated value [%zu] = %d\n" RESET, index, stack -> array[index]);

    return NO_ERROR;
}
