#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include "stack.h"
#include "common.h"
#include "arithm_oper.h"
#include "error_types.h"

const char *const  PUSH  = "PUSH";
const char *const  POP   = "POP";
const char *const  PUSHR = "PUSHR";
const char *const  POPR  = "POPR";
const char *const  OUT   = "OUT";
const char *const  ADD   = "ADD";
const char *const  SUB   = "SUB";
const char *const  MUL   = "MUL";
const char *const  DIV   = "DIV";
const char *const SQRT   = "SQRT";
const char *const  HLT   = "HLT";

int get_command(code_type *code_command, type_of_element *number,
                char *name_of_command, FILE *input_file);

stack_err_t calc_online(stack_t *stack);

#endif // CALCULATOR_H_
