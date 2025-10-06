#ifndef ARITHM_OPER_H_
#define ARITHM_OPER_H_

#include "stack.h"
#include "error_types.h"

stack_err_t stack_add(stack_t *stack);
stack_err_t stack_sub(stack_t *stack);
stack_err_t stack_mul(stack_t *stack);
stack_err_t stack_div(stack_t *stack);
stack_err_t stack_sqrt(stack_t *stack);

#endif // ARITHM_OPER_H_
