#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <cstddef>    // для size_t

const int NUMBER_OF_REGISTERS = 8;
const int MAX_SIZE_OF_REGISTERS = 32;

extern const char *const REGISTER_NAMES[NUMBER_OF_REGISTERS];

enum register_code
{
    REG_INVALID = -1,
    REG_RAX     = 0,
    REG_RBX     = 1,
    REG_RCX     = 2,
    REG_RDX     = 3,
    REG_REX     = 4,
    REG_RFX     = 5,
    REG_RGX     = 6,
    REG_RHX     = 7
};

enum code_type
{
    code_SHIT    = 0,
    code_PUSH    = 1,
    code_POP     = 2,
    code_OUT     = 3,
    code_ADD     = 4,
    code_SUB     = 5,
    code_MUL     = 6,
    code_DIV     = 7,
    code_SQRT    = 8,
    code_HLT     = 9,
    code_IN      = 10,

    code_PUSHR   = 33,
    code_POPR    = 34,

    code_JMP     = 41,
    code_JB      = 42,
    code_JBE     = 43,
    code_JA      = 44,
    code_JAE     = 45,
    code_JE      = 46,
    code_JNE     = 47,

};

size_t get_file_size(FILE *file);

const char *get_register_name(register_code reg);
register_code get_register_by_name(const char *name);

#endif // COMMON_H_
