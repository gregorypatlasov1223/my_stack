#ifndef PROC_ERROR_TYPES_H_
#define PROC_ERROR_TYPES_H_

enum processor_error_type
{
    PROC_ERROR_NO                      = 0,
    PROC_ERROR_ALLOCATION_FAILED       = 1,
    PROC_ERROR_CANNOT_OPEN_BINARY_FILE = 2,
    PROC_ERROR_READING_FILE            = 3,
    PROC_ERROR_UNKNOWN_OPERATION_CODE  = 4,
    PROC_ERROR_STACK_OPERATION_FAILED  = 5,
    PROC_ERROR_INVALID_STATE           = 6,
    PROC_ERROR_INVALID_JUMP            = 7
};

#endif // PROC_ERROR_TYPES_H_
