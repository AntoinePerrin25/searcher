#include "csv.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


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
    line[strcspn(line, "\n")] = 0; // Remove newline character if present
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


int csv_print(CsvFile* csv, FILE* file, size_t limit)
{
    char sep[2] = {csv->separator, '\0'}; // Separator string for strtok
    
    /*
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
    }
    */
    if (csv->type == CSV_WITH_HEADER) {
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
            fprintf(file, "%s%s%s", i ? sep : "", i ? " " : "", csv->CsvFile_U.with_header.header[i]);
        }
        fprintf(file, "\n");
        for (size_t i = 0; i < limit  && i < csv->CsvFile_U.with_header.count_lines; i++) {
            for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++) {
                fprintf(file, "%s%s%s", j ? sep : "", j ? " " : "", csv->CsvFile_U.with_header.entries[i][j]);
            }
            fprintf(file, "\n");
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



char* format_message(const char* format, ...) {
    static char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    return buffer;
}

// Helper function to check if a string contains another string (case insensitive)
static bool str_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return false;
    
    char *haystack_lower = strdup(haystack);
    char *needle_lower = strdup(needle);
    if (!haystack_lower || !needle_lower) {
        if (haystack_lower) free(haystack_lower);
        if (needle_lower) free(needle_lower);
        return false;
    }
    
    // Convert both strings to lowercase
    for (size_t i = 0; haystack_lower[i]; i++) {
        haystack_lower[i] = tolower(haystack_lower[i]);
    }
    for (size_t i = 0; needle_lower[i]; i++) {
        needle_lower[i] = tolower(needle_lower[i]);
    }
    
    bool result = strstr(haystack_lower, needle_lower) != NULL;
    
    free(haystack_lower);
    free(needle_lower);
    return result;
}

// Helper function to check if a string starts with another string (case insensitive)
static bool str_starts_with(const char *str, const char *prefix) {
    if (!str || !prefix) return false;
    
    while (*prefix) {
        if (tolower(*str) != tolower(*prefix)) {
            return false;
        }
        str++;
        prefix++;
    }
    return true;
}

// Helper function to check if a string ends with another string (case insensitive)
static bool str_ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return false;
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return false;
    
    return strcasecmp(str + str_len - suffix_len, suffix) == 0;
}

// Calculate Levenshtein distance between two strings
static int levenshtein_distance(const char *s1, const char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    
    int matrix[len1 + 1][len2 + 1];
    
    for (size_t i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    
    for (size_t j = 0; j <= len2; j++) {
        matrix[0][j] = j;
    }
    
    for (size_t i = 1; i <= len1; i++) {
        for (size_t j = 1; j <= len2; j++) {
            int cost = (tolower(s1[i - 1]) != tolower(s2[j - 1]));
            
            int deletion = matrix[i - 1][j] + 1;
            int insertion = matrix[i][j - 1] + 1;
            int substitution = matrix[i - 1][j - 1] + cost;
            
            matrix[i][j] = (deletion < insertion) ? 
                (deletion < substitution ? deletion : substitution) : 
                (insertion < substitution ? insertion : substitution);
        }
    }
    
    return matrix[len1][len2];
}

// Checks if the field matches the query based on search type
static bool match_field(const char *field, const char *query, int search_type, bool correction, int *distance) {
    *distance = 0;
    
    if (!field || !query) return false;
    
    // If correction is enabled, calculate Levenshtein distance
    if (correction) {
        *distance = levenshtein_distance(field, query);
        // Accept matches with distance less than or equal to 30% of query length
        int max_distance = strlen(query) * 0.3;
        if (max_distance < 5) max_distance = 5; // Minimum threshold
        
        if (*distance <= max_distance) {
            return true;
        }
    }
    
    // Check for exact match
    if (strcasecmp(field, query) == 0) {
        *distance = 0;
        return true;
    }
    
    // Check based on search type
    switch (search_type) {
        case 0: // All types of matches
            if (str_contains(field, query) || 
                str_starts_with(field, query) || 
                str_ends_with(field, query)) {
                return true;
            }
            break;
        case 1: // Contains
            if (str_contains(field, query)) {
                return true;
            }
            break;
        case 2: // Starts with
            if (str_starts_with(field, query)) {
                return true;
            }
            break;
        case 3: // Ends with
            if (str_ends_with(field, query)) {
                return true;
            }
            break;
    }
    
    return false;
}

// Free a CSV result linked list
void csv_result_free(CsvResult *result) {
    CsvResult *current = result;
    while (current) {
        CsvResult *next = current->next;
        if (current->result) free(current->result);
        if (current->column_name) free(current->column_name);
        free(current);
        current = next;
    }
}

// Implementation of csv_search function
CsvResult* csv_search(CsvFile* csv, const char* query, int search_column, int search_type, bool correction) {
    if (!csv || !query) return NULL;
    
    CsvResult *head = NULL;  // Head of the linked list
    CsvResult *tail = NULL;  // Tail of the linked list
    int match_count = 0;     // Count of matches
    int distance = 0;        // Levenshtein distance
    
    if (csv->type == CSV_WITH_HEADER) {
        // Search in headers first (if search_column is 0 or valid column index)
        if (search_column == 0) {
            for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
                if (match_field(csv->CsvFile_U.with_header.header[i], query, search_type, correction, &distance)) {
                    // Create a new result node
                    CsvResult *new_result = malloc(sizeof(CsvResult));
                    if (!new_result) continue;
                    
                    new_result->isHeadered = true;
                    new_result->isCorrected = (distance > 0);
                    new_result->distance = distance;
                    new_result->result = strdup(csv->CsvFile_U.with_header.header[i]);
                    new_result->filename = NULL;  // No filename for header
                    new_result->column_name = strdup(csv->CsvFile_U.with_header.header[i]);
                    new_result->line_number = 0;  // Headers are at line 0
                    new_result->next = NULL;
                    
                    // Add to the linked list
                    if (!head) {
                        head = new_result;
                        tail = new_result;
                    } else {
                        tail->next = new_result;
                        tail = new_result;
                    }
                    match_count++;
                }
            }
        } else if (search_column > 0 && (size_t)search_column <= csv->CsvFile_U.with_header.count_headers) {
            // Check only the specified header column
            size_t col = search_column - 1;  // Convert to 0-based index
            if (match_field(csv->CsvFile_U.with_header.header[col], query, search_type, correction, &distance)) {
                CsvResult *new_result = malloc(sizeof(CsvResult));
                if (new_result) {
                    new_result->isHeadered = true;
                    new_result->isCorrected = (distance > 0);
                    new_result->distance = distance;
                    new_result->result = strdup(csv->CsvFile_U.with_header.header[col]);
                    new_result->filename = NULL;
                    new_result->column_name = strdup(csv->CsvFile_U.with_header.header[col]);
                    new_result->line_number = 0;
                    new_result->next = NULL;
                    
                    head = new_result;
                    tail = new_result;
                    match_count++;
                }
            }
        }
        
        // Search in data entries
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines; i++) {
            if (search_column == 0) {
                // Search all columns
                for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++) {
                    if (match_field(csv->CsvFile_U.with_header.entries[i][j], query, search_type, correction, &distance)) {
                        CsvResult *new_result = malloc(sizeof(CsvResult));
                        if (!new_result) continue;
                        
                        new_result->isHeadered = false;
                        new_result->isCorrected = (distance > 0);
                        new_result->distance = distance;
                        new_result->result = strdup(csv->CsvFile_U.with_header.entries[i][j]);
                        new_result->filename = NULL;
                        new_result->column_name = strdup(csv->CsvFile_U.with_header.header[j]);
                        new_result->line_number = i + 1;  // +1 because lines are 1-indexed for display
                        new_result->next = NULL;
                        
                        if (!head) {
                            head = new_result;
                            tail = new_result;
                        } else {
                            tail->next = new_result;
                            tail = new_result;
                        }
                        match_count++;
                    }
                }
            } else if (search_column > 0 && (size_t)search_column <= csv->CsvFile_U.with_header.count_headers) {
                // Search only the specified column
                size_t col = search_column - 1;  // Convert to 0-based index
                if (match_field(csv->CsvFile_U.with_header.entries[i][col], query, search_type, correction, &distance)) {
                    CsvResult *new_result = malloc(sizeof(CsvResult));
                    if (!new_result) continue;
                    
                    new_result->isHeadered = false;
                    new_result->isCorrected = (distance > 0);
                    new_result->distance = distance;
                    new_result->result = strdup(csv->CsvFile_U.with_header.entries[i][col]);
                    new_result->filename = NULL;
                    new_result->column_name = strdup(csv->CsvFile_U.with_header.header[col]);
                    new_result->line_number = i + 1;
                    new_result->next = NULL;
                    
                    if (!head) {
                        head = new_result;
                        tail = new_result;
                    } else {
                        tail->next = new_result;
                        tail = new_result;
                    }
                    match_count++;
                }
            }
        }
    } else if (csv->type == CSV_NO_HEADER) {
        // For non-header CSV, just search each line
        for (size_t i = 0; i < csv->CsvFile_U.no_header.count_lines; i++) {
            if (match_field(csv->CsvFile_U.no_header.entries[i], query, search_type, correction, &distance)) {
                CsvResult *new_result = malloc(sizeof(CsvResult));
                if (!new_result) continue;
                
                new_result->isHeadered = false;
                new_result->isCorrected = (distance > 0);
                new_result->distance = distance;
                new_result->result = strdup(csv->CsvFile_U.no_header.entries[i]);
                new_result->filename = NULL;
                new_result->column_name = NULL;  // No columns in no-header CSV
                new_result->line_number = i + 1;
                new_result->next = NULL;
                
                if (!head) {
                    head = new_result;
                    tail = new_result;
                } else {
                    tail->next = new_result;
                    tail = new_result;
                }
                match_count++;
            }
        }
    }
    
    return head;
}

// Display search results with pagination
int csv_results_display(CsvResult *results, size_t limit) {
    if (!results) {
        printf("No results found.\n");
        return 0;
    }
    
    CsvResult *current = results;
    size_t total_results = 0;
    // Count total results
    while (current) {
        total_results++;
        current = current->next;
    }
    
    size_t current_page = 1;
    size_t total_pages = (total_results + limit - 1) / limit;  // Ceiling division
    
    char choice;
    int ret = 0;
    
    do {
        // Calculate start and end indices for current page
        size_t start_idx = (current_page - 1) * limit;
        size_t end_idx = start_idx + limit - 1;
        if (end_idx >= total_results) {
            end_idx = total_results - 1;
        }
        
        // Clear screen
        printf("\033[2J\033[H");  // ANSI escape codes to clear screen and move cursor to home position
        
        // Display page header
        printf("\n===== Search Results (Page %zu of %zu) =====\n\n", current_page, total_pages);
        printf("%-5s %-20s %-8s %-10s %-30s\n", "No.", "Column", "Line", "Corrected", "Result");
        printf("--------------------------------------------------------------\n");
        
        // Display results for current page
        current = results;
        for (size_t i = 0; i < start_idx && current; i++) {
            current = current->next;
        }
        
        for (size_t i = start_idx; i <= end_idx && current; i++) {
            printf("%-5zu %-20s %-8zu %-5s %-30s\n", 
                i + 1,
                current->column_name ? current->column_name : "N/A",
                current->line_number,
                current->isCorrected ? format_message("%d", current->distance) : "",
                current->result);
                
            
            current = current->next;
        }
        
        // Display navigation options
        printf("\n===== Navigation =====\n");
        printf("(n) Next page | (p) Previous page | (l) Last page | (f) First page | (q) Quit\n");
        printf("Choice: ");
        
        // Get user choice
        choice = getchar();
        // Consume the newline character
        while (getchar() != '\n');
        
        // Process user choice
        switch (choice) {
            case 'n':
            case 'N':
                if (current_page < total_pages) {
                    current_page++;
                }
                break;
            case 'p':
            case 'P':
                if (current_page > 1) {
                    current_page--;
                }
                break;
            case 'l':
            case 'L':
                current_page = total_pages;
                break;
            case 'f':
            case 'F':
                current_page = 1;
                break;
            case 'q':
            case 'Q':
                ret = 1;  // Signal to quit
                break;
            default:
                // Ignore other inputs
                break;
        }
        
    } while (ret == 0);
    
    return total_results;
}