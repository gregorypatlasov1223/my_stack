#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Expected 2 arguments, got %d\n", argc - 1);
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    printf("Starting assembler...\n");
    printf("Input file: %s\n", input_filename);
    printf("Output file: %s\n", output_filename);

    assembler asm_instance = {};

    assembler_type_error error = assembler_constructor(&asm_instance, input_filename, output_filename);
    if (error != ASM_NO_ERROR)
    {
        fprintf(stderr, "Failed to initialize assembler\n");
        asm_error_translator(error);
        return 1;
    }

    printf("Instruction file loaded successfully\n");
    printf("File size: %zu bytes\n", strlen(asm_instance.instruction_buffer));

    printf("=== FIRST PASS ===\n");
    error = first_pass(&asm_instance);
    if (error != ASM_NO_ERROR)
    {
        fprintf(stderr, "First pass failed\n");
        asm_error_translator(error);
        assembler_destructor(&asm_instance);
        return 1;
    }

    printf("=== SECOND PASS ===\n");
    error = second_pass(&asm_instance);
    if (error != ASM_NO_ERROR)
    {
        fprintf(stderr, "Second pass failed\n");
        asm_error_translator(error);
        assembler_destructor(&asm_instance);
        return 1;
    }

    assembler_destructor(&asm_instance);

    printf("Assembly completed successfully!\n");
    return 0;
}
