#ifndef ERROR_TYPES_H_
#define ERROR_TYPES_H_

enum stack_err_t
{
    INITIALIZE_ERROR    = -1,
    ERROR_DIV_BY_0      = 0,
    NO_ERROR            = 1,
    UNDER_FLOW_CAPACITY = 2,
    OVER_FLOW_CAPACITY  = 3,
    UNDER_FLOW_SIZE     = 4,
    OVER_FLOW_SIZE      = 5,
    ARRAY_POINTER_ERROR = 13,
    STACK_POINTER_ERROR = 52,
    CANARY_DAMAGED      = 77
};

#endif // ERROR_TYPES_H_

