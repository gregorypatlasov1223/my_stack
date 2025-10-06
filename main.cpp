#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "calculator.h"
#include "arithm_oper.h"

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
    // my_stack.size = -1;    // UNDER_FLOW_CAPACITY error

    // my_stack.array[0] = 0x12345678;  // Повредит переднюю канарейку
    // stack_verify(&my_stack);

    if (stack_push(&my_stack, 10) != NO_ERROR)
        printf("Push failed\n");
    if (stack_push(&my_stack, 20) != NO_ERROR)
        printf("Push failed\n");
    if (stack_push(&my_stack, 30) != NO_ERROR)
        printf("Push failed\n");

    type_of_element value = 0;

    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    if (stack_pop(&my_stack, &value) == NO_ERROR)
        printf("Popped value = %d\n", value);
    // if (stack_pop(&my_stack, &value) == NO_ERROR)
    //     printf("Popped value = %d\n", value);

    stack_dump(&my_stack, NO_ERROR, __FILE__, __func__, __LINE__);

    stack_destructor(&my_stack);

    return 0;
}









