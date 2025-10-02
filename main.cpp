#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define RESET_CAPACITY 0
#define RESET_SIZE 0
#define INITIAL_SIZE 0
#define MULTIPLIER 2
#define STACK_MAX_CAPACITY 333
#define STACK_MAX_SIZE  100
#define POISON 765911

/*#define CHECK_STACK code_error = stack_verify(stack, __FILE__, __func__, __LINE__); \
                    if (code_error) \
                    { \
                        return code_error; \
                    } \         */

enum stack_err_t
{
    INITIALIZE_ERROR    = 0,
    NO_ERROR            = 1,
    UNDER_FLOW_CAPACITY = 2,
    OVER_FLOW_CAPACITY  = 3,
    UNDER_FLOW_SIZE     = 4,
    OVER_FLOW_SIZE      = 5,
    ARRAY_POINTER_ERROR = 13,
    STACK_POINTER_ERROR = 52,
};

typedef int type_of_element;

struct stack_t
{
    type_of_element *array;
    size_t capacity;
    size_t size;
};


stack_err_t stack_pop(stack_t *stack, type_of_element *value);
stack_err_t stack_push(stack_t *stack, type_of_element value);
stack_err_t stack_constructor(stack_t *stack, size_t capacity);
stack_err_t stack_verify(stack_t *stack, const char *file_name, const char *function_name, int line);

void stack_destructor(stack_t *stack);
void error_translator(stack_err_t code_error);
void stack_dump(stack_t *stack, stack_err_t code_error, const char *file_name, const char *function_name, int line);


int main(void)
{
    stack_t my_stack = {0};

    if (stack_constructor(&my_stack, 50) != NO_ERROR)
    {
        fprintf(stderr, "Failed to initialize stack\n");
        return INITIALIZE_ERROR;
    }

    // my_stack.size = 350;
    // my_stack.capacity = 340;  // OVER_FLOW_CAPACITY error
    //my_stack.size = -1;    // UNDER_FLOW_CAPACITY error

    //stack_constructor(&my_stack, my_stack.capacity);

    if (stack_push(&my_stack, 10) != NO_ERROR)
        printf("Push failed\n");
    if (stack_push(&my_stack, 20) != NO_ERROR)
        printf("Push failed\n");
    if (stack_push(&my_stack, 30) != NO_ERROR)
        printf("Push failed\n");

     type_of_element value = 0;
//
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);

    stack_dump(&my_stack, NO_ERROR, __FILE__, __func__, __LINE__);

    stack_destructor(&my_stack);

    return 0;
}


stack_err_t stack_pop(stack_t *stack, type_of_element *value)
{
    assert(value != NULL);

    stack_err_t code_error = stack_verify(stack, __FILE__, __func__, __LINE__);

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

    if (code_error != NO_ERROR)
    {
        stack_dump(stack, code_error, __FILE__, __func__, __LINE__);
        return code_error;
    }


    if ((stack -> size) <= 0)
    {
        fprintf(stderr, "Error: Stack is underflowed\n"); // может тут сделать другую реализацию?
        return UNDER_FLOW_SIZE;
    }

    *value = stack -> array[(stack -> size) - 1];
    stack -> size = (stack -> size) - 1;

    return NO_ERROR;
}


stack_err_t stack_push(stack_t *stack, type_of_element value)
{
    stack_err_t code_error = stack_verify(stack, __FILE__, __func__, __LINE__);

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

    if (stack -> size >= stack -> capacity)
    {
        size_t new_capacity = (stack -> capacity) * MULTIPLIER;

        type_of_element *new_array = (type_of_element*)realloc(stack -> array, new_capacity * sizeof(type_of_element));
        stack -> array = new_array;
        printf("555\n");

        for (size_t index = stack -> capacity; index < new_capacity; index++) // проинициализоровал выделенные элементы Poison но оно все равно не выводит как надо
        {
            stack -> array[index] = POISON;
            printf("%d", stack -> array[index]);
        }

    }

    if ((stack -> array) == NULL)
        return ARRAY_POINTER_ERROR; // на нужном месте стоит?

    stack -> size = (stack -> size) + 1;
    stack -> array[(stack -> size) - 1] = value;


    return NO_ERROR;
}


stack_err_t stack_constructor(stack_t *stack, size_t initial_capacity)
{
    if (stack == NULL)
        return STACK_POINTER_ERROR;

    stack -> array = (type_of_element*)calloc(initial_capacity, sizeof(type_of_element));

    if (stack -> array == NULL)
        return ARRAY_POINTER_ERROR;

    stack -> capacity = initial_capacity;

    for (size_t index = 0; index < stack -> capacity; index++)
        stack -> array[index] = POISON;

    stack -> size = INITIAL_SIZE;

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


stack_err_t stack_verify(stack_t *stack, const char *file_name, const char *function_name, int line)
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

    if ((stack -> size) > (stack -> capacity))
    {
        code_error = OVER_FLOW_CAPACITY;

        return code_error;
    }

    if ((stack -> size) < RESET_SIZE)
    {
        code_error = UNDER_FLOW_SIZE;

        return code_error;
    }

    if ((stack -> capacity) < RESET_CAPACITY)
    {
        code_error = UNDER_FLOW_CAPACITY;

        return code_error;
    }

    return NO_ERROR;
}


void stack_dump(stack_t *stack, stack_err_t code_error, const char *file_name, const char *function_name, int line)
{
    assert(file_name     != NULL);
    assert(function_name != NULL);

    //printf("------------------------------------------------------------------------------------------------\n"); //для отладки
    printf("The type of error is ");
    error_translator(code_error);

    printf("It was called from function %s at %s line %d\n", function_name, file_name, line);

    printf("size = %zu\n", stack -> size);
    printf("capacity = %zu\n", stack -> capacity);
    printf("array[%p]\n", stack -> array);

    if (code_error != ARRAY_POINTER_ERROR)
    {
        for (int index = 0; index < stack -> capacity; index++)
        {
            if (stack -> array[index] == POISON)
                printf("*[%zu] = %d [POISON]\n", index, stack -> array[index]);
            else
                printf("[%zu] = %d\n", index, stack -> array[index]);
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
            printf("ARRAY_POINTER_ERROR\n");
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
        case NO_ERROR:
            printf("NO_ERROR\n");
            break;
        default:
            printf("UNKNOWN_ERROR (%d)\n", code_error);
            break;
    }
}

