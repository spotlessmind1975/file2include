// This directive is used to deactivate the safety warning for the use 
// of fopen under Microsoft Visual Studio.
#pragma warning(disable : 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ERL_WRONG_OPTIONS               1
#define ERL_MISSING_INPUT_FILENAME      2
#define ERL_MISSING_OUTPUT_FILENAME     3
#define ERL_MISSING_INPUT_FILE          4
#define ERL_OUT_OF_MEMORY               5
#define ERL_CANNOT_CREATE_OUTPUT        6

#define FILE_COUNT                      255

// Filenames to convert
char* filename_input[FILE_COUNT];

// Count of filename to convert
unsigned int filename_input_count = 0;

// Output filename (include file)
char* filename_output_include = NULL;

// Output filename (source file)
char* filename_output_source = NULL;

// Execution verbose?
int verbose = 0;

// This function prints a short guide to the available options.
void usage_and_exit(int _level, int _argc, char* _argv[]) {

    printf("file2include - Convert one or more binary file in an include/source file.\n");
    printf("Copyright(c) 2020 by Marco Spedaletti.\n");
    printf("Licensed under CC-BY-NC-SA\n\n");
    printf("Usage: %s [options]", _argv[0]);
    printf("\n");
    printf("options:\n");
    printf("\n");
    printf("[mandatory]\n");
    printf("\n");
    printf(" -i <filename>      input filename of binary file to include\n");
    printf(" -c <filename>      output filename of source file\n");
    printf(" -h <filename>      output filename of include file\n");
    printf("\n");
    printf("[optional]\n");
    printf("\n");
    printf(" -v                 make execution verbose\n");
    printf(" ");

    exit(_level);

}

// This function allows to parse the options entered on the command line. 
// Options must start with a minus character ('-') and only the first letter 
// is considered.

void parse_options(int _argc, char* _argv[]) {

    // Used as index.
    int i;

    // We check for each option...
    for (i = 1; i < _argc; ++i) {

        // Parse it only if begins with '-'
        if (_argv[i][0] == '-') {

            switch (_argv[i][1]) {
            case 'i': // "-i <filename>"
                filename_input[filename_input_count] = _argv[i + 1];
                ++filename_input_count;
                ++i;
                break;
            case 'h': // "-h <filename>"
                filename_output_include = _argv[i + 1];
                ++i;
                break;
            case 'c': // "-c <filename>"
                filename_output_source = _argv[i + 1];
                ++i;
                break;
            case 'v': // "-v"
                verbose = 1;
                break;
            default:
                printf("Unknown option: %s", _argv[i]);
                usage_and_exit(ERL_WRONG_OPTIONS, _argc, _argv);
            }

        }
    }

}

// Main function
int main(int _argc, char* _argv[]) {

    FILE* fileInput = NULL;
    FILE* fileOutputInclude = NULL;
    FILE* fileOutputSource = NULL;
    unsigned int i = 0, j = 0;

    parse_options(_argc, _argv);

    if (filename_input_count == 0 ) {
        printf("Missing input filename.\n");
        usage_and_exit(ERL_MISSING_INPUT_FILENAME, _argc, _argv);
    }

    if (filename_output_source == NULL) {
        printf("Missing output source filename.\n");
        usage_and_exit(ERL_MISSING_OUTPUT_FILENAME, _argc, _argv);
    }

    if (filename_output_include == NULL) {
        printf("Missing output include filename.\n");
        usage_and_exit(ERL_MISSING_OUTPUT_FILENAME, _argc, _argv);
    }

    if (verbose) {
        for (i = 0; i < filename_input_count; ++i) {
            printf("Input filename          : %s\n", filename_input[i]);
        }
        printf("Output include filename : %s\n", filename_output_source);
        printf("Output source filename  : %s\n", filename_output_source);
    }

    fileOutputSource = fopen(filename_output_source, "wt");
    if (fileOutputSource == NULL) {
        printf("Cannot create file '%s'.\n", filename_output_source);
        usage_and_exit(ERL_CANNOT_CREATE_OUTPUT, _argc, _argv);
    }

    fileOutputInclude = fopen(filename_output_include, "wt");
    if (fileOutputSource == NULL) {
        printf("Cannot create file '%s'.\n", filename_output_include);
        usage_and_exit(ERL_CANNOT_CREATE_OUTPUT, _argc, _argv);
    }

    fprintf(fileOutputInclude, "#ifndef __INCLUDED_FILES__\n");

    for (i = 0; i < filename_input_count; ++i) {
        unsigned char* content;
        unsigned int size;
        int c;

        unsigned char filenameMangled[128];
        memset(filenameMangled, 0, 128);
        for (j = 0, c = strlen(filename_input[i]); j < c; ++j) {
            if (isalpha(filename_input[i][j])) {
                filenameMangled[j] = toupper(filename_input[i][j]);
            } else {
                filenameMangled[j] = '_';
            }
        }
        
        fileInput = fopen(filename_input[i], "rb");
        if (fileInput == NULL) {
            printf("Missing file '%s'.\n", filename_input[i]);
            usage_and_exit(ERL_MISSING_INPUT_FILE, _argc, _argv);
        }

        fseek(fileInput, 0, SEEK_END);
        size = ftell(fileInput);
        fseek(fileInput, 0, SEEK_SET);

        content = malloc(size);
        if (content == NULL) {
            printf("Out of memory during reading of '%s'.\n", filename_input[i]);
            usage_and_exit(ERL_OUT_OF_MEMORY, _argc, _argv);
        }
        fread(content, size, 1, fileInput);

        if (verbose) {
            printf("\t%s: %d bytes\n", filename_input[i], size);
        }

        fprintf(fileOutputSource, "\tunsigned char _includedFile%03.3d[%d] = {\n\t\t", i, size );
        if (size > 1) {
            for (j = 0; j < (size - 1); ++j) {
                fprintf(fileOutputSource, "0x%2.2x, ", content[j]);
                if (((j+1) % 8) == 0) {
                    fprintf(fileOutputSource, "\n\t\t");
                }
            }
        }
        if (size > 0) {
            fprintf(fileOutputSource, "0x%02.2x", content[0]);
        }
        fprintf(fileOutputSource, "\t};\n\n");

        fprintf(fileOutputInclude, "\t#define FILE_%s\t%d\n", filenameMangled, i);
        fprintf(fileOutputInclude, "\textern unsigned char _includedFile%03.3d[%d];\n", i, size);

        free(content);

        fclose(fileInput);
    }

    fprintf(fileOutputInclude, "#endif\n");

    return 0;

}