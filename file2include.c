/*****************************************************************************
 * IMG2MIDRES - Utility to convert images into midres pictures               *
 *****************************************************************************
 * Copyright 2020 Marco Spedaletti (asimov@mclink.it)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *----------------------------------------------------------------------------
 * Concesso in licenza secondo i termini della Licenza Apache, versione 2.0
 * (la "Licenza"); � proibito usare questo file se non in conformit� alla
 * Licenza. Una copia della Licenza � disponibile all'indirizzo:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Se non richiesto dalla legislazione vigente o concordato per iscritto,
 * il software distribuito nei termini della Licenza � distribuito
 * "COS� COM'�", SENZA GARANZIE O CONDIZIONI DI ALCUN TIPO, esplicite o
 * implicite. Consultare la Licenza per il testo specifico che regola le
 * autorizzazioni e le limitazioni previste dalla medesima.
 ****************************************************************************/

// This directive is used to deactivate the safety warning for the use 
// of fopen under Microsoft Visual Studio.
#pragma warning(disable : 4996)

/****************************************************************************
 ** INCLUDE SECTION
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/****************************************************************************
 ** RESIDENT VARIABLES SECTION
 ****************************************************************************/

// Error levels.

#define ERL_WRONG_OPTIONS               1
#define ERL_MISSING_INPUT_FILENAME      2
#define ERL_MISSING_OUTPUT_FILENAME     3
#define ERL_MISSING_INPUT_FILE          4
#define ERL_OUT_OF_MEMORY               5
#define ERL_CANNOT_CREATE_OUTPUT        6

// Maximum number of files.
#define FILE_COUNT                      255

// Filenames to convert
char* filename_input[FILE_COUNT];

// Names to use instead of default one
char* filename_replaced[FILE_COUNT];

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

    printf("file2include - Convert one or more binary file in an include/source file\n");
    printf("Copyright(c) 2020-2021 by Marco Spedaletti.\n");
    printf("Licensed under Apache 2.0 License\n\n");
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
    printf(" -n <filename>      filename to use inside the include/source file\n");
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
            case 'n': // "-n <filename>"
                filename_replaced[filename_input_count-1] = _argv[i + 1];
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

// Extract the basename part of a complete file path

char* basename(char* _path) {
    char* base = strrchr(_path, '/');
    if (base == NULL) {
        base = strrchr(_path, '\\');
    }
    return base ? base + 1 : _path;
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
            if (filename_replaced[i]) {
                printf("Input filename          : %s (replaced as %s)\n", filename_input[i], filename_replaced[i]);
            }
            else {
                printf("Input filename          : %s\n", filename_input[i]);
            }
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
        unsigned char* bname;

        unsigned char filenameMangled[128];
        memset(filenameMangled, 0, 128);

        if (filename_replaced[i]) {
            bname = basename(filename_replaced[i]);
        } else {
            bname = basename(filename_input[i]);
        }
        for (j = 0, c = strlen(bname); j < c; ++j) {
            if (isalnum(bname[j])) {
                filenameMangled[j] = toupper(bname[j]);
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
        j = 0;
        if (size > 1) {
            for (j = 0; j < (size - 1); ++j) {
                fprintf(fileOutputSource, "0x%2.2x, ", content[j]);
                if (((j+1) % 8) == 0) {
                    fprintf(fileOutputSource, "\n\t\t");
                }
            }
        }
        if (size > 0) {
            fprintf(fileOutputSource, "0x%02.2x", content[j]);
        }
        fprintf(fileOutputSource, "\t};\n\n");

        fprintf(fileOutputInclude, "\t#define FILE_%s\t%d\n", filenameMangled, i);
        fprintf(fileOutputInclude, "\t#define FILE_%s_SIZE\t%d\n", filenameMangled, size);

        free(content);

        fclose(fileInput);
    }

    fprintf(fileOutputSource, "\tunsigned char * _includedFiles[%d] = {\n", filename_input_count);

    j = 0;
    if (filename_input_count > 1) {
        for (j = 0; j < (filename_input_count - 1); ++j) {
            fprintf(fileOutputSource, "\t\t&_includedFile%03.3d[0]\n,", j);
            if (((j + 1) % 8) == 0) {
                fprintf(fileOutputSource, "\n\t\t");
            }
        }
    }
    if (filename_input_count > 0) {
        fprintf(fileOutputSource, "\t\t&_includedFile%03.3d[0]\n", j);
    }

    fprintf(fileOutputSource, "\t};\n\n");

    fprintf(fileOutputInclude, "\textern unsigned char * _includedFiles[%d];\n", filename_input_count);

    fprintf(fileOutputInclude, "\t#define   INCLUDED_FILES_COUNT    %d\n", filename_input_count);
    fprintf(fileOutputInclude, "#endif\n");

    return 0;

}