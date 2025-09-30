#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_MAX_CAPACITY 333
#define STACK_MAX_SIZE  100
#define RESET_CAPACITY 0
#define RESET_SIZE 0
#define INITIAL_SIZE 0
#define MULTIPLIER 2

/*#define CHECK_STACK code_error = stack_verify(stack, __FILE__, __func__, __LINE__); \
                    if (code_error) \
                    { \
                        return code_error; \
                    } \         */

enum stack_err_t
{
    INITIALIZE_ERROR    = 0,
    NO_ERROR            = 1,
    UNDER_FLOW          = 3,
    OVER_FLOW           = 4,
    ARRAY_POINTER_ERROR = 13,
    SIZE_ERROR          = 52,
    CAPACITY_ERROR      = 666
};

typedef int type_of_element;

struct stack_t
{
    type_of_element *array;
    type_of_element capacity;
    type_of_element size;
};


stack_err_t stack_pop(stack_t *stack, type_of_element *value);
stack_err_t stack_push(stack_t *stack, type_of_element value);
stack_err_t stack_creator(stack_t *stack, size_t capacity);
stack_err_t stack_verify(stack_t *stack, const char *file_name, const char *function_name, int line);

void stack_destructor(stack_t *stack);
void stack_dump(stack_t *stack, stack_err_t type_of_error, const char *file_name, const char *function_name, int line);


int main(void)
{
    stack_t my_stack = {0};

    if (stack_creator(&my_stack, 5) != NO_ERROR)
    {
        fprintf(stderr, "Failed to initialize stack\n");
        return INITIALIZE_ERROR;
    }

    my_stack.size = 1000;  // OVER_FLOW error
    //my_stack.size = -1;    // UNDER_FLOW error


    if (stack_push(&my_stack, 10) != NO_ERROR)
    {
        printf("Push failed\n");
    }
    if (stack_push(&my_stack, 20) != NO_ERROR)
    {
        printf("Push failed\n");
    }
    if (stack_push(&my_stack, 30) != NO_ERROR)
    {
        printf("Push failed\n");
    }

    type_of_element value = 0;

    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);

    if (stack_pop(&my_stack, &value) != NO_ERROR)
        printf("Stack pop failed with error (this should show stack_dump)\n");

    stack_destructor(&my_stack);

    return 0;
}


/*stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    if (stack == NULL || value == NULL)
    {
        return ARRAY_POINTER_ERROR;
    }

    if ((stack -> size) < 0)
    {
        fprintf(stderr, "Error: Stack is underflowed\n");
        return stack_verify(stack, __FILE__, __func__, __LINE__);
    }

    *value = stack -> array[(stack -> size) - 1];
    stack -> size = (stack -> size) - 1;

    return NO_ERROR;
}*/


stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    if (stack == NULL || value == NULL) {
        return ARRAY_POINTER_ERROR;
    }

    stack_err_t code_error = stack_verify(stack, __FILE__, __func__, __LINE__);
    if (code_error != NO_ERROR) {
        return code_error;
    }

    if ((stack -> size) <= 0)
    {
        fprintf(stderr, "Error: Stack is underflowed\n");
        return UNDER_FLOW;
    }

    *value = stack -> array[(stack -> size) - 1];
    stack -> size = (stack -> size) - 1;

    return NO_ERROR;
}


stack_err_t stack_push(stack_t *stack, type_of_element value)
{
    stack_err_t code_error = stack_verify(stack, __FILE__, __func__, __LINE__);

    if (code_error != NO_ERROR)
        return code_error;

    if (stack -> size == stack -> capacity) //>=
    {
        (stack -> capacity) *= MULTIPLIER;
        stack -> array = (type_of_element*)realloc(stack -> array, (stack -> capacity) * sizeof(type_of_element));

        if ((stack -> array) == NULL)
            return ARRAY_POINTER_ERROR;
    }

    stack -> size = (stack -> size) + 1;
    stack -> array[(stack -> size) - 1] = value;

    return NO_ERROR;
}


stack_err_t stack_creator(stack_t *stack, size_t initial_capacity)
{
    if (stack == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> array = (type_of_element*)calloc(initial_capacity, sizeof(type_of_element));

    if (stack->array == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> size = INITIAL_SIZE;
    stack -> capacity = initial_capacity;

    return NO_ERROR;
}


void stack_destructor(stack_t *stack)
{
    assert(stack != NULL);

    free(stack -> array);

    stack -> array = NULL;
    stack -> size = RESET_SIZE;
    stack -> capacity = RESET_CAPACITY;
}


stack_err_t stack_verify(stack_t *stack, const char *file_name, const char *function_name, int line)
{
    stack_err_t type_of_error = NO_ERROR;
    if (stack == NULL)
    {
        printf("Stack has a null pointer\n");
        type_of_error = ARRAY_POINTER_ERROR;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> array) == NULL)
    {
        printf("Array has a null pointer\n");
        type_of_error = ARRAY_POINTER_ERROR;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> size) > STACK_MAX_SIZE)
    {
        printf("Size is bigger than MAX value\n");
        type_of_error = OVER_FLOW;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> capacity) > STACK_MAX_CAPACITY)
    {
        printf("Capacity is bigger than MAX value\n");
        type_of_error = OVER_FLOW;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> size) > (stack -> capacity))
    {
        printf("Size is bigger than capacity\n");
        type_of_error = OVER_FLOW;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> size) < RESET_SIZE)
    {
        printf("Size is less than MIN value\n");
        type_of_error = UNDER_FLOW;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    if ((stack -> capacity) < RESET_CAPACITY)
    {
        printf("Capacity is less than MIN value\n");
        type_of_error = UNDER_FLOW;

        stack_dump(stack, type_of_error, file_name, function_name, line);
        return type_of_error;
    }

    return NO_ERROR;
}


void stack_dump(stack_t *stack, stack_err_t type_of_error, const char *file_name, const char *function_name, int line)
{
    size_t capacity_of_stack = stack -> capacity;
    size_t size_of_stack = stack -> size;

    printf("There are %d errors int Stack_dump. It was called from function %s at %s line %d\n", type_of_error, function_name, file_name, line);

    printf("size = %lu\n", size_of_stack);
    printf("capacity = %lu\n", capacity_of_stack);
    printf("array[%p]\n", stack -> array);

    for (int index = 0; index < size_of_stack; index++)
        printf("*[%d] = %d\n", index, stack -> array[index]);

    //for (int index = 0; index >= (size_of_stack) && (index < capacity_of_stack); index++)
    //    printf("[%d] = %d (POISON)\n", index, stack -> array[index]);
    for (int index = stack->size; index < stack->capacity; index++)
        printf("  [%d] = %d (FREE)\n", index, stack->array[index]);
}




