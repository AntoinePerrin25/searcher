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


// Linked list node for search results
// Composed of a result, which file it came from, which line and column it came from, and a pointer to the next node
typedef struct CsvResult
{
    bool isHeadered :1; // true if the result is from the header, false if it's from the entries
    bool isCorrected :1; // true if the result was corrected, false if it was not
    int distance : 6; // distance from the original string to the corrected string
    const char* result;
    const char* filename;
    const char* column_name;
    size_t line_number;
    struct CsvResult* next;
} CsvResult;



// Read CSV file with header, open the file, read it and return a dynamically allocated CsvFile struct pointer, close the file after reading
CsvFile* csv_read_header(const char* filename, char separator);
// Read CSV file without header, open the file, read it and return a dynamically allocated CsvFile struct pointer, close the file after reading
CsvFile* csv_read_noheader(const char* filename, char separator);
// Print CSV File to file stream with a limit on the number of rows
int csv_print(CsvFile* csv, FILE* file, size_t limit);

// Free memory allocated for CsvFile struct
void csv_free(CsvFile* csv);

/**
 * @brief Global search through a CsvFile struct and return search results as a linked list
 * @param *csv - Pointer to the CsvFile struct to search in
 * @param query - String to search for
 * @param search_column - Column to search in (0=all columns)
 * @param search_type - Type of search (0=all, 1=contains, 2=starts_with, 3=ends_with)
 * @param correction - Enable fuzzy search/correction
 * @return CsvResult struct linked list containing the search results
 */ 
CsvResult* csv_search(CsvFile* csv, const char* query, size_t search_column, size_t search_type, bool correction);

/**
 * @brief Display search results with pagination
 * @param results - Pointer to the first CsvResult in the linked list
 * @param limit - Number of results to display per page
 * @return Number of total results
 */
int csv_results_display(CsvResult *results, size_t limit);

/**
 * @brief Free memory allocated for CsvResult linked list
 * @param result - Pointer to the first CsvResult in the linked list
 */
void csv_result_free(CsvResult *result);

#endif // CSV_H