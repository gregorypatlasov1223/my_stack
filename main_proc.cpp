#include <stdio.h>
#include <stdlib.h>

#include "error_types.h"
#include "proc_error_types.h"
#include "stack.h"
#include "processor.h"

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Error opening files in %s\n", argv[0]);
        return 1;
    }

    const char* binary_filename = argv[1];

    processor proc_struct = {};
    processor_error_type error_result = processor_constructor(&proc_struct, 100);

    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Execution binary file failed with error: \n");
        proc_error_translator(error_result);
        processor_destructor(&proc_struct);

        return 1;
    }

    error_result = read_binary_file_to_buffer(&proc_struct, binary_filename);
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Reading binary file failed with error: ");
        proc_error_translator(error_result);
        processor_destructor(&proc_struct);

        return 1;
    }

    if (proc_struct.code_buffer_size == 0)
    {
        fprintf(stderr, "Error: Binary file is empty or invalid\n");
        processor_destructor(&proc_struct);
        
        return 1;
    }

    error_result = execute_processor(&proc_struct);
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Execution binary file failed with error: ");
        proc_error_translator(error_result);
        processor_destructor(&proc_struct);

        return 1;
    }
    printf("Execution completed successfully\n");
    processor_destructor(&proc_struct);

    return 0;
}
