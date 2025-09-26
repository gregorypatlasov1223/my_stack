#include <stdio.h>
#include <stdlib.h>

//#define STACK_MAX_SIZE  100
//#define OVER_FLOW 333
#define RESET_CAPACITY 0
#define RESET_SIZE 0
#define INITIAL_SIZE 0
#define MULTIPLIER 2


enum stack_err_t
{
    INITIALIZE_ERROR    = 0,
    NO_ERROR            = 1,
    UNDER_FLOW          = 3,
    ARRAY_POINTER_ERROR = 13,
    SIZE_ERROR          = 52,
    CAPACITY_ERROR      = 666

};

typedef int type_of_element;

typedef struct stack_t
{
    type_of_element *array;
    type_of_element capacity;
    type_of_element size;

}stack_t;

void stack_destroy(stack_t *stack);

stack_err_t stack_pop(stack_t *stack, type_of_element *value);
stack_err_t stack_push(stack_t *stack, type_of_element value);
stack_err_t stack_initial(stack_t *stack, size_t capacity);


int main(void)
{
    stack_t my_stack = {0};

    if (stack_initial(&my_stack, 5) != NO_ERROR)
    {
        fprintf(stderr, "Failed to initialize stack\n");
        return INITIALIZE_ERROR;
    }

    stack_push(&my_stack, 10);
    stack_push(&my_stack, 20);
    stack_push(&my_stack, 30);

    type_of_element value;

    if (stack_pop(&my_stack,&value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack,&value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack,&value) == NO_ERROR)
        printf("Popped value = %d\n", value);

    stack_destroy(&my_stack);

    return 0;
}


stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    if (stack == NULL || value == NULL) {
        return ARRAY_POINTER_ERROR;
    }

    if (stack->size <= 0)
    {
        fprintf(stderr, "Error: Stack is underflowed\n");
        return UNDER_FLOW;
    }

    *value = stack->array[stack->size];
    stack->size = stack->size - 1;

    return NO_ERROR;
}


stack_err_t stack_push(stack_t *stack, type_of_element value)
{
    if (stack -> size >= (stack -> capacity) - 1)
    {
        (stack -> capacity) *= MULTIPLIER;
        stack -> array = (type_of_element*)realloc(stack -> array, (stack -> capacity) * sizeof(type_of_element));

        if ((stack -> array) == NULL)
            return ARRAY_POINTER_ERROR;
    }

    stack -> size = (stack -> size) + 1; //stack -> size++ good?
    stack -> array[stack -> size] = value;

    return NO_ERROR;
}


stack_err_t stack_initial(stack_t *stack, size_t initial_capacity)
{
    if (stack == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> array = (type_of_element*)malloc(initial_capacity * sizeof(type_of_element));

    if (stack->array == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> size = INITIAL_SIZE; // what i need to write?
    stack -> capacity = initial_capacity;

    return NO_ERROR;
}


void stack_destroy(stack_t *stack)
{
    free(stack -> array);

    stack -> array = NULL;
    stack -> size = RESET_SIZE;
    stack -> capacity = RESET_CAPACITY;
}




