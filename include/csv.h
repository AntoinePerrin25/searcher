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
    union {
        struct {
            char**  header;         // array of strings (column names)
            char*** entries;        // 2D array of strings (rows x columns)
            size_t  count_lines;    // number of rows (excluding header)
            size_t  count_headers;  // number of headers (columns)
            size_t  capacity;       // allocated capacity for rows
        } with_header;

        struct {
            char**  entries;        // 1D array of strings (each is a line of comma-separated values)
            size_t  count_lines;    // number of lines
            size_t  capacity;       // allocated capacity for lines
        } no_header;
    };
} CsvFile;

// Open CSV File, read it and return a dynamically allocated CsvFile struct pointer, close the file after reading
CsvFile* csv_open(const char* filename, char separator, bool with_header);
// Close CSV File and free memory
CsvFile csv_close(CsvFile csv);
// Print CSV File to file stream
CsvFile csv_print(CsvFile csv, FILE* file, int limit);


void csv_free(CsvFile* csv);

/**
 * @brief Global search fu through a CsvFile struct and return new CsvFile struct with the results
 * @param *csv
 * @param querry
 * @param search_column (enum {all, column1, column2, ...})
 * @param search_type (enum {all, contains, starts_with, ends_with})
 * @param correction boolean (if true, the search will return closest matches)
 * @return CsvFile struct to the new CsvFile struc with search results
*/ 
CsvFile csv_search(CsvFile* csv, const char* query, int search_column, int search_type, bool correction);
