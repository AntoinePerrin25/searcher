#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define FLAG_IMPLEMENTATION
#include "flag.h"

void usage(FILE* stream)
{
    fprintf(stream, "Usage: search [OPTIONS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

#include "csv.h"



// Maximum number of input files
#define MAX_FILES 10

typedef struct InputFile {
    char* filename;
    bool has_header;
    char separator;
} InputFile;


int main(int argc, char **argv) {

    // Help flag
    bool *help            = flag_bool_aliases("h"  , false, "Display help information",       "help");
    bool *correction      = flag_bool_aliases("c"  , false, "Enable fuzzy search/correction", "correction");
    char **input_header   = flag_str_aliases ("ih" , NULL , "Input CSV file with header",     "input-header");
    char **input_noheader = flag_str_aliases ("in" , NULL , "Input CSV file without header",  "input-noheader");
    char **query          = flag_str_aliases ("q"  , ""   , "Search query",                   "query");
    char **separator      = flag_str_aliases ("s"  , ","  , "CSV separator character",        "separator");
    size_t *limit         = flag_size_aliases("l"  , 10   , "Limit number of results",        "limit");
    size_t *column        = flag_size_aliases("col", 0    , "Column to search (0=all)",       "column");
    size_t *search_type   = flag_size_aliases("t"  , 0    , "Search type (0=all, 1=contains, 2=starts_with, 3=ends_with)", "type");
    
    // Parse the command line arguments
    if (!flag_parse(argc, argv)) {
        fprintf(stderr, "Error parsing flags: ");
        flag_print_error(stderr);
        fprintf(stderr, "\n");
        usage(stderr);
        return 1;
    }

    // Display help if requested
    if (*help) {
        usage(stdout);
        return 0;
    }

    // Setup input files array
    InputFile input_files[MAX_FILES];
    int file_count = 0;

    // Add input files with header
    if (*input_header != NULL) {
        input_files[file_count].filename = *input_header;
        input_files[file_count].has_header = true;
        input_files[file_count].separator = (*separator)[0];
        file_count++;
    }

    // Add input files without header
    if (*input_noheader != NULL) {
        input_files[file_count].filename = *input_noheader;
        input_files[file_count].has_header = false;
        input_files[file_count].separator = (*separator)[0];
        file_count++;
    }

    // Check for additional input files in the rest arguments
    int rest_argc = flag_rest_argc();
    char **rest_argv = flag_rest_argv();
    
    for (int i = 0; i < rest_argc && file_count < MAX_FILES; i++) {
        if (strcmp(rest_argv[i], "-ih") == 0 || strcmp(rest_argv[i], "--input-header") == 0) {
            if (i + 1 < rest_argc) {
                input_files[file_count].filename = rest_argv[i + 1];
                input_files[file_count].has_header = true;
                input_files[file_count].separator = (*separator)[0];
                file_count++;
                i++; // Skip the next argument
            }
        }
        else if (strcmp(rest_argv[i], "-in") == 0 || strcmp(rest_argv[i], "--input-noheader") == 0) {
            if (i + 1 < rest_argc) {
                input_files[file_count].filename = rest_argv[i + 1];
                input_files[file_count].has_header = false;
                input_files[file_count].separator = (*separator)[0];
                file_count++;
                i++; // Skip the next argument
            }
        }
    }

    // Validate required arguments
    if (!file_count) {
        fprintf(stderr, "Error: No input files specified. Use -ih or -in to specify input files.\n");
        usage(stderr);
        return 1;
    }

    if (!strlen(*query)) {
        fprintf(stderr, "Error: No search query specified. Use -q or --query to specify a search query.\n");
        usage(stderr);
        return 1;
    }

    // Process each input file
    for (int i = 0; i < file_count; i++) {
        printf("Processing file: %s\n", input_files[i].filename);
        
        CsvFile* csv;
        
        // Open the CSV file based on whether it has a header or not
        if (input_files[i].has_header) {
            csv = csv_read_header(input_files[i].filename, input_files[i].separator);
        } else {
            csv = csv_read_noheader(input_files[i].filename, input_files[i].separator);
        }
        
        if (!csv) {
            fprintf(stderr, "Error: Could not open file '%s'\n", input_files[i].filename);
            continue;
        }
        
        // Perform the search
        clock_t start_time = clock();
        double totaltime = 0.0;
        CsvResult* results = csv_search(csv, *query, *column, *search_type, *correction);
        
        totaltime = (double)(clock() - start_time) / CLOCKS_PER_SEC;


        // Display the results with pagination
        if (results) {
            csv_results_display(results, *limit, *query, totaltime);            
            // Free the results
            csv_result_free(results);
        } else {
            printf("No results found for query '%s' in file '%s'.\n", *query, input_files[i].filename);
        }
        
        // Free the CSV data
        csv_free(csv);
    }

    return 0;
}