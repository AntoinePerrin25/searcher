#ifndef CSV_H
#define CSV_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief CSV File struct to handle formatted or not formatted separated values
 * @param type
 * @param separator
 * @param struct_withHeader
 * @param w_header
 * @param w_entries
 * @param w_count_lines
 * @param w_count_headers
 * @param w_capacity
 * @param struct_noHeader
 * @param n_entries
 * @param n_count_lines
 * @param n_capacity
 * */
typedef struct CsvFile{
    enum {
        CSV_WITH_HEADER,
        CSV_NO_HEADER
    } type;
    char separator;
    union CsvFile_U{
        struct with_header{
            char**  header;         // array of strings (column names)
            char*** entries;        // 2D array of strings (rows x columns)
            size_t  count_lines;    // number of rows (excluding header)
            size_t  count_headers;  // number of headers (columns)
        } with_header;

        struct no_header{
            char**  entries;        // 1D array of strings (each is a line of comma-separated values)
            size_t  count_lines;    // number of lines
        } no_header;
    }CsvFile_U;
} CsvFile;

// Read CSV file with header, open the file, read it and return a dynamically allocated CsvFile struct pointer, close the file after reading
CsvFile* csv_read_header(const char* filename, char separator);
// Read CSV file without header, open the file, read it and return a dynamically allocated CsvFile struct pointer, close the file after reading
CsvFile* csv_read_noheader(const char* filename, char separator);
// Print CSV File to file stream with a limit on the number of rows
int csv_print(CsvFile* csv, FILE* file, size_t limit);

// Free memory allocated for CsvFile struct
void csv_free(CsvFile* csv);

/**
 * @brief Global search through a CsvFile struct and return new CsvFile struct with the results
 * @param *csv - Pointer to the CsvFile struct to search in
 * @param query - String to search for
 * @param search_column - Column to search in (0=all columns)
 * @param search_type - Type of search (0=all, 1=contains, 2=starts_with, 3=ends_with)
 * @param correction - Enable fuzzy search/correction
 * @return CsvFile struct containing the search results
*/ 
CsvFile* csv_search(CsvFile* csv, const char* query, int search_column, int search_type, bool correction);

#endif // CSV_H