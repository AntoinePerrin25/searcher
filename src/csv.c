#include "csv.h"
#include <string.h>

#define rows_standard_capacity 50

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

    // Read the header line and count the number of headers
    char line[1024];
    if (fgets(line, sizeof(line), file) == NULL) {
        free(csv);
        fclose(file);
        return NULL;
    }
    // Counting headers
    csv->CsvFile_U.with_header.count_headers = 0;
    char* token = strtok(line, &csv->separator);
    while (token) {
        csv->CsvFile_U.with_header.count_headers++;
        token = strtok(NULL, &csv->separator);
    }
    // Allocate memory for headers 1D array of strings
    csv->CsvFile_U.with_header.header = malloc(csv->CsvFile_U.with_header.count_headers * sizeof(char*));
    if (!csv->CsvFile_U.with_header.header) {
        free(csv);
        fclose(file);
        return NULL;
    }
    csv->CsvFile_U.with_header.entries = malloc(rows_standard_capacity * sizeof(char**)); // Initial capacity for 50 rows
    if (!csv->CsvFile_U.with_header.entries) {
        free(csv->CsvFile_U.with_header.header);
        free(csv);
        fclose(file);
        return NULL;
    }




    return csv;
}

CsvFile* csv_read_noheader(const char* filename, char separator);


int csv_print(CsvFile* csv, FILE* file, int limit);


void csv_free(CsvFile* csv);


CsvFile* csv_search(CsvFile* csv, const char* query, int search_column, int search_type, bool correction);
