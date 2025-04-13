#include "csv.h"
#include <string.h>


CsvFile* csv_read_header(const char* filename, char separator)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    CsvFile* csv = malloc(sizeof(CsvFile));
    if (!csv) {
        fclose(file);
        return NULL;
    }

    csv->type = CSV_WITH_HEADER;
    csv->separator = separator;
    char sep[3] = {separator, '\n', '\0'}; // Separator string for strtok
    // Read the header line and count the number of headers
    char line[1024];
    if (fgets(line, sizeof(line), file) == NULL) {
        free(csv);
        fclose(file);
        return NULL;
    }
    // Counting headers
    csv->CsvFile_U.with_header.count_headers = 0;
    char* token = strtok(line, sep);
    while (token) {
        csv->CsvFile_U.with_header.count_headers++;
        token = strtok(NULL, sep);
    }
    // Allocate memory for headers 1D array of strings
    csv->CsvFile_U.with_header.header = malloc(csv->CsvFile_U.with_header.count_headers * sizeof(char*));
    if (!csv->CsvFile_U.with_header.header) {
        free(csv);
        fclose(file);
        return NULL;
    }

    // Counting lines
    csv->CsvFile_U.with_header.count_lines = 0;
    while (fgets(line, sizeof(line), file)) {
        csv->CsvFile_U.with_header.count_lines++;
    }
    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);
    // Read the header line again to store the headers
    if (fgets(line, sizeof(line), file) == NULL) {
        free(csv->CsvFile_U.with_header.header);
        free(csv);
        fclose(file);
        return NULL;
    }
    // Store the headers in the allocated memory
    token = strtok(line, sep);
    for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
        if (token) {
            csv->CsvFile_U.with_header.header[i] = malloc(strlen(token) + 1);
            if (!csv->CsvFile_U.with_header.header[i]) {
                for (size_t j = 0; j < i; j++) {
                    free(csv->CsvFile_U.with_header.header[j]);
                }
                free(csv->CsvFile_U.with_header.header);
                free(csv);
                fclose(file);
                return NULL;
            }
            strcpy(csv->CsvFile_U.with_header.header[i], token);
            token = strtok(NULL, sep);
        }
    }
    
    // Allocate memory for entries 2D array of strings composed of csv->CsvFile_U.with_header.count_headers * csv->CsvFile_U.with_header.count_lines
    csv->CsvFile_U.with_header.entries = malloc(csv->CsvFile_U.with_header.count_lines * sizeof(char**));
    if (!csv->CsvFile_U.with_header.entries) {
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
            free(csv->CsvFile_U.with_header.header[i]);
        }
        free(csv->CsvFile_U.with_header.header);
        free(csv);
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines; i++) {
        csv->CsvFile_U.with_header.entries[i] = malloc(csv->CsvFile_U.with_header.count_headers * sizeof(char*));
        if (!csv->CsvFile_U.with_header.entries[i]) {
            for (size_t j = 0; j < i; j++) {
                free(csv->CsvFile_U.with_header.entries[j]);
            }
            for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++) {
                free(csv->CsvFile_U.with_header.header[j]);
            }
            free(csv->CsvFile_U.with_header.header);
            free(csv->CsvFile_U.with_header.entries);
            free(csv);
            fclose(file);
            return NULL;
        }
    }
    // Read the entries and store them in the allocated memory
    size_t line_index = 0;
    while (fgets(line, sizeof(line), file)) {
        token = strtok(line, sep);
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
            if (token) {
                csv->CsvFile_U.with_header.entries[line_index][i] = malloc(strlen(token) + 1);
                if (!csv->CsvFile_U.with_header.entries[line_index][i]) {
                    for (size_t j = 0; j < line_index; j++) {
                        for (size_t k = 0; k < csv->CsvFile_U.with_header.count_headers; k++) {
                            free(csv->CsvFile_U.with_header.entries[j][k]);
                        }
                        free(csv->CsvFile_U.with_header.entries[j]);
                    }
                    for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++) {
                        free(csv->CsvFile_U.with_header.header[j]);
                    }
                    free(csv->CsvFile_U.with_header.header);
                    free(csv->CsvFile_U.with_header.entries);
                    free(csv);
                    fclose(file);
                    return NULL;
                }
                strcpy(csv->CsvFile_U.with_header.entries[line_index][i], token);
                token = strtok(NULL, sep);
            }
        }
        line_index++;
    }
    // Close the file


    fclose(file);

    // Return the CsvFile struct pointer
    return csv;
}

CsvFile* csv_read_noheader(const char* filename, char separator)
{
    printf("[TODO] csv_read_noheader is not implemented yet\n");
    return NULL; // TODO: Implement this function
}


int csv_print(CsvFile* csv, FILE* file, int limit)
{
    if (csv->type == CSV_WITH_HEADER) {
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers-1; i++) {
            fprintf(file, "%s%c ", csv->CsvFile_U.with_header.header[i], csv->separator);
        }
        fprintf(file, "%s\n", csv->CsvFile_U.with_header.header[csv->CsvFile_U.with_header.count_headers-1]);
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines && i < limit; i++) {
            for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers-1; j++) {
                fprintf(file, "%s%c ", csv->CsvFile_U.with_header.entries[i][j], csv->separator);
            }
            fprintf(file, "%s\n", csv->CsvFile_U.with_header.entries[i][csv->CsvFile_U.with_header.count_headers-1]);
        }
    } else if (csv->type == CSV_NO_HEADER) {
        for (size_t i = 0; i < csv->CsvFile_U.no_header.count_lines && i < limit; i++) {
            fprintf(file, "%s\n", csv->CsvFile_U.no_header.entries[i]);
        }
    }
    return 0;
}


void csv_free(CsvFile* csv)
{
    printf("[TODO] csv_free is not implemented yet\n");
    return; // TODO: Implement this function
}

CsvFile* csv_search(CsvFile* csv, const char* query, int search_column, int search_type, bool correction)
{
    printf("[TODO] csv_search is not implemented yet\n");
    return NULL; // TODO: Implement this function
}
