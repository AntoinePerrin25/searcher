#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    bool *help = flag_bool("h", false, "Display help information");
    flag_add_alias(help, "help");

    // Input files with header flag
    char **input_header = flag_str("ih", NULL, "Input CSV file with header");
    flag_add_alias(input_header, "input-header");

    // Input files without header flag
    char **input_noheader = flag_str("in", NULL, "Input CSV file without header");
    flag_add_alias(input_noheader, "input-noheader");

    // Number of results flag
    uint64_t *limit = flag_uint64("l", 10, "Limit number of results");
    flag_add_alias(limit, "limit");

    // Correction option flag
    bool *correction = flag_bool("c", false, "Enable fuzzy search/correction");
    flag_add_alias(correction, "correction");

    // Column search flag
    uint64_t *column = flag_uint64("col", 0, "Column to search (0=all)");
    flag_add_alias(column, "column");

    // Search type flag
    uint64_t *search_type = flag_uint64("t", 0, "Search type (0=all, 1=contains, 2=starts_with, 3=ends_with)");
    flag_add_alias(search_type, "type");

    // Query word flag
    char **query = flag_str("q", "", "Search query");
    flag_add_alias(query, "query");

    // Separator flag
    char **separator = flag_str("s", ",", "CSV separator character");
    flag_add_alias(separator, "separator");
    
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
    if (file_count == 0) {
        fprintf(stderr, "Error: No input files specified. Use -ih or -in to specify input files.\n");
        usage(stderr);
        return 1;
    }

    if (strlen(*query) == 0) {
        fprintf(stderr, "Error: No search query specified. Use -q or --query to specify a search query.\n");
        usage(stderr);
        return 1;
    }

    // Process each input file
    for (int i = 0; i < file_count; i++) {
        printf("Processing file: %s\n", input_files[i].filename);
        
        CsvFile* csv = NULL;
        
        // Open the CSV file based on whether it has a header or not
        if (input_files[i].has_header) {
            csv = csv_read_header(input_files[i].filename, input_files[i].separator);
        } else {
            csv = csv_read_noheader(input_files[i].filename, input_files[i].separator);
        }
        
        if (!csv) {
            fprintf(stderr, "Error: Could not open file %s\n", input_files[i].filename);
            continue;
        }
        
        // Perform the search
        CsvFile* result = csv_search(csv, *query, (int)*column, (int)*search_type, *correction);
        
        if (result) {
            // Print results
            printf("Search results for query '%s' in file %s:\n", *query, input_files[i].filename);
            int printed = csv_print(result, stdout, (int)*limit);
            printf("Found %d matching rows.\n", printed);
            
            // Clean up result
            csv_free(result);
        } else {
            printf("No results found for query '%s' in file %s.\n", *query, input_files[i].filename);
        }
        
        // Clean up csv
        csv_free(csv);
    }

    return 0;
}