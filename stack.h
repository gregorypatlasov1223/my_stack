#ifndef STACK_H_
#define STACK_H_

#include "error_types.h"

#define INITIAL_SIZE 0
#define RESET_CAPACITY 0
#define RESET_SIZE 0
#define MULTIPLIER 2
#define STACK_MAX_CAPACITY 333
#define STACK_MAX_SIZE  100
#define POISON 765911

#define NUMBER_OF_CANARIES 2

typedef int type_of_element;

const type_of_element CANARY_VALUE  = 16383;

struct stack_t;

stack_err_t stack_pop(stack_t *stack, type_of_element *value);
stack_err_t stack_push(stack_t *stack, type_of_element value);

void stack_destructor(stack_t *stack);
stack_err_t stack_constructor(stack_t *stack, size_t requested_capacity);

stack_err_t stack_verify(stack_t *stack);
void stack_dump(stack_t *stack, stack_err_t code_error, const char *file_name, const char *function_name, int line);

void error_translator(stack_err_t code_error);

stack_err_t check_canaries(const stack_t *stack);
stack_err_t setup_canaries(stack_t *stack);
size_t get_data_capacity(size_t total_capacity);

stack_err_t print_stack(stack_t *stack, FILE *output_file);

struct stack_t
{
    type_of_element *array;
    size_t capacity;
    size_t size;
};

#endif // STACK_H_
