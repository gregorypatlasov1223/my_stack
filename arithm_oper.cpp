#include <math.h>
#include <stdio.h>

#include "stack.h"
#include "error_types.h"
#include "arithm_oper.h"


stack_err_t stack_add(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    type_of_element number1 = 0;
    type_of_element number2 = 0;

    stack_pop(stack, &number1);
    stack_pop(stack, &number2);

    type_of_element add = number1 + number2;

    stack_push(stack, add);

    code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    return NO_ERROR;
}


stack_err_t stack_sub(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    type_of_element number1 = 0;
    type_of_element number2 = 0;

    stack_pop(stack, &number1);
    stack_pop(stack, &number2);

    type_of_element sub = number2 - number1;

    stack_push(stack, sub);

    code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    return NO_ERROR;
}


stack_err_t stack_mul(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    type_of_element number1 = 0;
    type_of_element number2 = 0;

    stack_pop(stack, &number1);
    stack_pop(stack, &number2);

    type_of_element mul = number1 * number2;

    stack_push(stack, mul);

    code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    return NO_ERROR;
}

stack_err_t stack_div(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    type_of_element number1 = 0;
    type_of_element number2 = 0;

    stack_pop(stack, &number1);
    stack_pop(stack, &number2);

    if (number1 == 0)
    {
        stack_push(stack, number1);
        stack_push(stack, number2);

        fprintf(stderr, "Division by zero error\n");

        return ERROR_DIV_BY_0;
    }

    type_of_element div = number2 / number1;

    stack_push(stack, div);

    code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    return NO_ERROR;
}


stack_err_t stack_sqrt(stack_t *stack)
{
    stack_err_t code_error = stack_verify(stack);

    if (code_error != NO_ERROR)
        return code_error;

    type_of_element number = 0;

    stack_pop(stack, &number);

    type_of_element sqrt_result = (type_of_element)sqrt(number);

    stack_push(stack, sqrt_result);

    code_error = stack_verify(stack);
    if (code_error != NO_ERROR)
        return code_error;

    return NO_ERROR;
}
