#ifndef ASM_ERROR_TYPES_H_
#define ASM_ERROR_TYPES_H_

enum assembler_type_error
{
    ASM_NO_ERROR                         = 0,
    ASM_ALLOCATION_FAILED_ERROR          = 1,
    ASM_UNKNOWN_COMMAND_ERROR            = 2,
    ASM_CANNOT_OPEN_INPUT_FILE_ERROR     = 3,
    ASM_CANNOT_OPEN_OUTPUT_FILE_ERROR    = 4,
    ASM_READING_FILE_ERROR               = 5,
    ASM_INCORRECT_NUMBER_OF_ARGUMENTS    = 6,
    ASM_INVALID_ARGUMENT_ERROR           = 7,
    ASM_CANNOT_GET_FILE_DESCRIPTOR_ERROR = 8,
    ASM_ERROR_EXPECTED_REGISTER          = 9,
    ASM_ERROR_INVALID_REGISTER           = 10
};

#endif // ASM_ERROR_TYPES_H_
