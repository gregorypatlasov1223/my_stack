#include <stdio.h>
#include <string.h>
#include "assembler.h"


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Expected 2 arguments, got %d\n", argc - 1);
        return ASM_INCORRECT_NUMBER_OF_ARGUMENTS;
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
        return error;
    }

    printf("instruction_file_to_buffer loaded successfully\n");
    printf("File size: %zu bytes\n", strlen(asm_instance.instruction_buffer));

    error = from_buffer_to_binary_file(&asm_instance);

    if (error != ASM_NO_ERROR)
    {
        fprintf(stderr, "Failed to assemble to binary\n");
        asm_error_translator(error);
        assembler_destructor(&asm_instance);

        return error;
    }

    assembler_destructor(&asm_instance);

    printf("Assembly completed successfully!\n");

    return 0;
}



