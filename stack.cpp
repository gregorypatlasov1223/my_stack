#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "error_types.h"
#include "stack.h"

stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    assert(value != NULL);

    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
    {
        if (code_error == OVER_FLOW_CAPACITY || code_error == OVER_FLOW_SIZE)
        {
            printf("Error: Stack is overflowed. Pop is failed\n");
            return code_error;
        }
        else
        {
            stack_dump(stack, code_error, __FILE__, __func__, __LINE__);
            return code_error;
        }
    }

    if ((stack -> size) <= 0)
    {
        fprintf(stderr, "Error: Stack is underflowed\n"); // может тут сделать другую реализацию?
        return UNDER_FLOW_SIZE;
    }

    *value = stack -> array[stack -> size];
    stack -> size = (stack -> size) - 1;

    return NO_ERROR;
}


stack_err_t stack_push(stack_t *stack, type_of_element value)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
    {
        if (code_error == OVER_FLOW_CAPACITY || code_error == OVER_FLOW_SIZE)
        {
            printf("Error: Stack is overflowed\n");
            return code_error;
        }
        else
        {
            stack_dump(stack, code_error, __FILE__, __func__, __LINE__);
            return code_error;
        }
    }

    if (stack -> size >= stack -> capacity - NUMBER_OF_CANARIES)
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

    stack->array = (type_of_element*)calloc(total_capacity, sizeof(type_of_element));
    if (stack->array == NULL)
        return ARRAY_POINTER_ERROR;

    stack->capacity = total_capacity;
    stack->size = INITIAL_SIZE;

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

    printf("It was called from function %s at %s line %d\n", function_name, file_name, line);

    printf("\nInformation about stack\n");
    printf("size = %zu\n", stack -> size);
    printf("capacity = %zu (total including canaries)\n", stack -> capacity);
    printf("data capacity = %zu (available for data)\n", get_data_capacity(stack -> capacity));
    printf("array[%p]\n", stack -> array);



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

            printf("[%zu] = ", index);

            if (index == 0 || index == (stack -> capacity) - 1)
                printf("%d", stack->array[index]);
            else
                printf("%d", stack->array[index]);
            printf(" %s\n", marker);
        }
    }
}


void error_translator(stack_err_t code_error)
{
    switch(code_error)
    {
        case ARRAY_POINTER_ERROR:
            printf("ARRAY_POINTER_ERROR\n");
            break;
        case STACK_POINTER_ERROR:
            printf("STACK_POINTER_ERROR\n");
            break;
        case INITIALIZE_ERROR:
            printf("INITIALIZE_ERROR");
            break;
        case UNDER_FLOW_CAPACITY:
            printf("UNDER_FLOW_CAPACITY\n");
            break;
        case OVER_FLOW_CAPACITY:
            printf("OVER_FLOW_CAPACITY\n");
            break;
        case UNDER_FLOW_SIZE:
            printf("UNDER_FLOW_SIZE\n");
            break;
        case OVER_FLOW_SIZE:
            printf("OVER_FLOW_SIZE\n");
            break;
        case CANARY_DAMAGED:
            printf("CANARY_DAMAGED\n");
            break;
        case ERROR_DIV_BY_0:
            printf("ERROR_DIV_BY_0\n");
            break;
        case NO_ERROR:
            printf("NO_ERROR\n");
            break;
        default:
            printf("UNKNOWN_ERROR (%d)\n", code_error);
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


stack_err_t print_stack(stack_t *stack, FILE *output_file)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    fprintf(output_file, "stack:\n");

    for (size_t index = 1; index <= stack -> size; index++)
        printf("[%zu] = %d\n", index, stack -> array[index]);

    return NO_ERROR;
}
