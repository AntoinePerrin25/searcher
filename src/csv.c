#include "csv.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <omp.h>

CsvFile *csv_read_header(const char *filename, char separator)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    CsvFile *csv = malloc(sizeof(CsvFile));
    if (!csv)
    {
        fclose(file);
        return NULL;
    }

    csv->type = CSV_WITH_HEADER;
    csv->separator = separator;
    char sep[3] = {separator, '\n', '\0'}; // Separator string for strtok
    // Read the header line and count the number of headers
    char line[1024];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        free(csv);
        fclose(file);
        return NULL;
    }
    line[strcspn(line, "\n")] = 0; // Remove newline character if present
    // Counting headers
    csv->CsvFile_U.with_header.count_headers = 0;
    char *token = strtok(line, sep);
    while (token)
    {
        csv->CsvFile_U.with_header.count_headers++;
        token = strtok(NULL, sep);
    }
    // Allocate memory for headers 1D array of strings
    csv->CsvFile_U.with_header.header = malloc(csv->CsvFile_U.with_header.count_headers * sizeof(char *));
    if (!csv->CsvFile_U.with_header.header)
    {
        free(csv);
        fclose(file);
        return NULL;
    }

    // Counting lines
    csv->CsvFile_U.with_header.count_lines = 0;
    while (fgets(line, sizeof(line), file))
    {
        csv->CsvFile_U.with_header.count_lines++;
    }
    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);
    // Read the header line again to store the headers
    if (fgets(line, sizeof(line), file) == NULL)
    {
        free(csv->CsvFile_U.with_header.header);
        free(csv);
        fclose(file);
        return NULL;
    }
    // Store the headers in the allocated memory
    token = strtok(line, sep);
    for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++)
    {
        if (token)
        {
            csv->CsvFile_U.with_header.header[i] = malloc(strlen(token) + 1);
            if (!csv->CsvFile_U.with_header.header[i])
            {
                for (size_t j = 0; j < i; j++)
                {
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
    csv->CsvFile_U.with_header.entries = malloc(csv->CsvFile_U.with_header.count_lines * sizeof(char **));
    if (!csv->CsvFile_U.with_header.entries)
    {
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++)
        {
            free(csv->CsvFile_U.with_header.header[i]);
        }
        free(csv->CsvFile_U.with_header.header);
        free(csv);
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines; i++)
    {
        csv->CsvFile_U.with_header.entries[i] = malloc(csv->CsvFile_U.with_header.count_headers * sizeof(char *));
        if (!csv->CsvFile_U.with_header.entries[i])
        {
            for (size_t j = 0; j < i; j++)
            {
                free(csv->CsvFile_U.with_header.entries[j]);
            }
            for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++)
            {
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
    while (fgets(line, sizeof(line), file))
    {
        token = strtok(line, sep);
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++)
        {
            if (token)
            {
                csv->CsvFile_U.with_header.entries[line_index][i] = malloc(strlen(token) + 1);
                if (!csv->CsvFile_U.with_header.entries[line_index][i])
                {
                    for (size_t j = 0; j < line_index; j++)
                    {
                        for (size_t k = 0; k < csv->CsvFile_U.with_header.count_headers; k++)
                        {
                            free(csv->CsvFile_U.with_header.entries[j][k]);
                        }
                        free(csv->CsvFile_U.with_header.entries[j]);
                    }
                    for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++)
                    {
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

CsvFile *csv_read_noheader(const char *filename, char separator)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    CsvFile *csv = malloc(sizeof(CsvFile));
    if (!csv)
    {
        fclose(file);
        return NULL;
    }

    // Initialize CSV file without header
    csv->type = CSV_NO_HEADER;
    csv->separator = separator;

    // Count the number of lines in the file
    char line[1024];
    csv->CsvFile_U.no_header.count_lines = 0;
    while (fgets(line, sizeof(line), file))
    {
        csv->CsvFile_U.no_header.count_lines++;
    }

    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Allocate memory for entries (1D array of strings)
    csv->CsvFile_U.no_header.entries = malloc(csv->CsvFile_U.no_header.count_lines * sizeof(char *));
    if (!csv->CsvFile_U.no_header.entries)
    {
        free(csv);
        fclose(file);
        return NULL;
    }

    // Read each line and store it
    size_t line_index = 0;
    while (fgets(line, sizeof(line), file) && line_index < csv->CsvFile_U.no_header.count_lines)
    {
        // Remove trailing newline if present
        line[strcspn(line, "\n")] = 0;

        // Allocate memory for this line's content
        csv->CsvFile_U.no_header.entries[line_index] = malloc(strlen(line) + 1);
        if (!csv->CsvFile_U.no_header.entries[line_index])
        {
            // Clean up previous allocations if this one fails
            for (size_t j = 0; j < line_index; j++)
            {
                free(csv->CsvFile_U.no_header.entries[j]);
            }
            free(csv->CsvFile_U.no_header.entries);
            free(csv);
            fclose(file);
            return NULL;
        }

        // Copy the line into the allocated memory
        strcpy(csv->CsvFile_U.no_header.entries[line_index], line);
        line_index++;
    }

    // Close the file after reading
    fclose(file);

    return csv;
}

int csv_print(CsvFile *csv, FILE *file, size_t limit)
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
    if (csv->type == CSV_WITH_HEADER)
    {
        for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++)
        {
            fprintf(file, "%s%s%s", i ? sep : "", i ? " " : "", csv->CsvFile_U.with_header.header[i]);
        }
        fprintf(file, "\n");
        for (size_t i = 0; i < limit && i < csv->CsvFile_U.with_header.count_lines; i++)
        {
            for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++)
            {
                fprintf(file, "%s%s%s", j ? sep : "", j ? " " : "", csv->CsvFile_U.with_header.entries[i][j]);
            }
            fprintf(file, "\n");
        }
    }
    else if (csv->type == CSV_NO_HEADER)
    {
        for (size_t i = 0; i < csv->CsvFile_U.no_header.count_lines && i < limit; i++)
        {
            fprintf(file, "%s\n", csv->CsvFile_U.no_header.entries[i]);
        }
    }
    return 0;
}

void csv_free(CsvFile *csv)
{
    if (!csv)
    {
        return; // Nothing to free
    }

    if (csv->type == CSV_WITH_HEADER)
    {
        // Free header array
        if (csv->CsvFile_U.with_header.header)
        {
            for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++)
            {
                if (csv->CsvFile_U.with_header.header[i])
                {
                    free(csv->CsvFile_U.with_header.header[i]);
                }
            }
            free(csv->CsvFile_U.with_header.header);
        }

        // Free entries 2D array
        if (csv->CsvFile_U.with_header.entries)
        {
            for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines; i++)
            {
                if (csv->CsvFile_U.with_header.entries[i])
                {
                    for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++)
                    {
                        if (csv->CsvFile_U.with_header.entries[i][j])
                        {
                            free(csv->CsvFile_U.with_header.entries[i][j]);
                        }
                    }
                    free(csv->CsvFile_U.with_header.entries[i]);
                }
            }
            free(csv->CsvFile_U.with_header.entries);
        }
    }
    else if (csv->type == CSV_NO_HEADER)
    {
        // Free entries 1D array
        if (csv->CsvFile_U.no_header.entries)
        {
            for (size_t i = 0; i < csv->CsvFile_U.no_header.count_lines; i++)
            {
                if (csv->CsvFile_U.no_header.entries[i])
                {
                    free(csv->CsvFile_U.no_header.entries[i]);
                }
            }
            free(csv->CsvFile_U.no_header.entries);
        }
    }

    // Free the CsvFile struct itself
    free(csv);
}

char *format_message(const char *format, ...)
{
    static char buffer[256];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return buffer;
}

// Helper function to check if a string contains another string (case insensitive)
static bool str_contains(const char *haystack, const char *needle)
{
    if (!haystack || !needle)
        return false;

    char *haystack_lower = strdup(haystack);
    char *needle_lower = strdup(needle);
    if (!haystack_lower || !needle_lower)
    {
        if (haystack_lower)
            free(haystack_lower);
        if (needle_lower)
            free(needle_lower);
        return false;
    }
    
    strlwr(haystack_lower);
    strlwr(needle_lower);

    bool result = strstr(haystack_lower, needle_lower) != NULL;

    free(haystack_lower);
    free(needle_lower);
    return result;
}

// Helper function to check if a string starts with another string (case insensitive)
static bool str_starts_with(const char *str, const char *prefix)
{
    if (!str || !prefix)
        return false;

    while (*prefix)
    {
        if (tolower(*str) != tolower(*prefix))
        {
            return false;
        }
        str++;
        prefix++;
    }
    return true;
}

// Helper function to check if a string ends with another string (case insensitive)
static bool str_ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return false;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
        return false;

    return strcasecmp(str + str_len - suffix_len, suffix) == 0;
}

// Calculate Levenshtein distance between two strings
static int levenshtein_distance(const char *s1, const char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    int matrix[len1 + 1][len2 + 1];

    for (size_t i = 0; i <= len1; i++)
    {
        matrix[i][0] = i;
    }

    for (size_t j = 0; j <= len2; j++)
    {
        matrix[0][j] = j;
    }

    for (size_t i = 1; i <= len1; i++)
    {
        for (size_t j = 1; j <= len2; j++)
        {
            int cost = (tolower(s1[i - 1]) != tolower(s2[j - 1]));

            int deletion = matrix[i - 1][j] + 1;
            int insertion = matrix[i][j - 1] + 1;
            int substitution = matrix[i - 1][j - 1] + cost;

            matrix[i][j] = (deletion < insertion) ? (deletion < substitution ? deletion : substitution) : (insertion < substitution ? insertion : substitution);
        }
    }

    return matrix[len1][len2];
}

// Checks if the field matches the query based on search type
static bool match_field(const char *field, const char *query, size_t search_type, bool correction, int *distance)
{
    *distance = 0;

    if (!field || !query)
        return false;

    // If correction is enabled, calculate Levenshtein distance
    if (correction)
    {
        *distance = levenshtein_distance(field, query);
        // Accept matches with distance less than or equal to 30% of query length
        int max_distance = strlen(query) * 0.3;
        if (max_distance < 5)
            max_distance = 5; // Minimum threshold

        if (*distance <= max_distance)
        {
            return true;
        }
    }

    // Check for exact match
    if (strcasecmp(field, query) == 0)
    {
        *distance = 0;
        return true;
    }

    // Check based on search type
    switch (search_type)
    {
    case 0: // All types of matches
        if (str_contains(field, query) ||
            str_starts_with(field, query) ||
            str_ends_with(field, query))
        {
            return true;
        }
        break;
    case 1: // Contains
        if (str_contains(field, query))
        {
            return true;
        }
        break;
    case 2: // Starts with
        if (str_starts_with(field, query))
        {
            return true;
        }
        break;
    case 3: // Ends with
        if (str_ends_with(field, query))
        {
            return true;
        }
        break;
    }

    return false;
}

// Free a CSV result linked list
void csv_result_free(CsvResult *result)
{
    CsvResult *current = result;
    while (current)
    {
        CsvResult *next = current->next;
        free(current);
        current = next;
    }
}

// Implementation of csv_search function
CsvResult* csv_search(CsvFile* csv, const char* query, size_t search_column, size_t search_type, bool correction) 
{
    CsvResult* head = NULL;
    CsvResult* local_results[omp_get_max_threads()];
    int local_counts[omp_get_max_threads()];
    memset(local_results, 0, sizeof(local_results));
    memset(local_counts, 0, sizeof(local_counts));

    if (csv->type == CSV_WITH_HEADER) {
        // Search headers first if needed
        if (search_column == 0 || search_column <= csv->CsvFile_U.with_header.count_headers) {
            #pragma omp parallel
            {
                int tid = omp_get_thread_num();
                int distance = 0;
                CsvResult* local_head = NULL;
                CsvResult* local_tail = NULL;

                #pragma omp for
                for (size_t i = 0; i < csv->CsvFile_U.with_header.count_headers; i++) {
                    if (search_column == 0 || search_column == i + 1) {
                        if (match_field(csv->CsvFile_U.with_header.header[i], query, search_type, correction, &distance)) {
                            CsvResult* new_result = calloc(1, sizeof(CsvResult));
                            if (new_result) {
                                new_result->isHeadered = true;
                                new_result->isCorrected = (distance > 0);
                                new_result->distance = distance;
                                new_result->column_name = csv->CsvFile_U.with_header.header[i];
                                new_result->result = csv->CsvFile_U.with_header.header[i];
                                new_result->line_number = 0;
                                new_result->next = NULL;

                                if (!local_head) {
                                    local_head = local_tail = new_result;
                                } else {
                                    local_tail->next = new_result;
                                    local_tail = new_result;
                                }
                                local_counts[tid]++;
                            }
                        }
                    }
                }
                local_results[tid] = local_head;
            }
        }

        // Search data entries
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            int distance = 0;
            CsvResult* local_head = local_results[tid];
            CsvResult* local_tail = NULL;

            if (local_head) {
                local_tail = local_head;
                while (local_tail->next) local_tail = local_tail->next;
            }

            #pragma omp for
            for (size_t i = 0; i < csv->CsvFile_U.with_header.count_lines; i++) {
                if (search_column == 0) {
                    for (size_t j = 0; j < csv->CsvFile_U.with_header.count_headers; j++) {
                        if (match_field(csv->CsvFile_U.with_header.entries[i][j], query, search_type, correction, &distance)) {
                            CsvResult* new_result = calloc(1, sizeof(CsvResult));
                            if (new_result) {
                                new_result->isHeadered = false;
                                new_result->isCorrected = (distance > 0);
                                new_result->distance = distance;
                                new_result->column_name = csv->CsvFile_U.with_header.header[j];
                                new_result->result = csv->CsvFile_U.with_header.entries[i][j];
                                new_result->line_number = i + 1;
                                new_result->next = NULL;

                                if (!local_head) {
                                    local_head = local_tail = new_result;
                                } else {
                                    local_tail->next = new_result;
                                    local_tail = new_result;
                                }
                                local_counts[tid]++;
                            }
                        }
                    }
                } else if (search_column <= csv->CsvFile_U.with_header.count_headers) {
                    size_t col = search_column - 1;
                    if (match_field(csv->CsvFile_U.with_header.entries[i][col], query, search_type, correction, &distance)) {
                        CsvResult* new_result = calloc(1, sizeof(CsvResult));
                        if (new_result) {
                            new_result->isHeadered = false;
                            new_result->isCorrected = (distance > 0);
                            new_result->distance = distance;
                            new_result->column_name = strdup(csv->CsvFile_U.with_header.header[col]);
                            new_result->result = strdup(csv->CsvFile_U.with_header.entries[i][col]);
                            new_result->line_number = i + 1;
                            new_result->next = NULL;

                            if (!local_head) {
                                local_head = local_tail = new_result;
                            } else {
                                local_tail->next = new_result;
                                local_tail = new_result;
                            }
                            local_counts[tid]++;
                        }
                    }
                }
            }
            local_results[tid] = local_head;
        }
    } 
    // Merge results from all threads
    CsvResult* tail = NULL;
    for (int i = 0; i < omp_get_max_threads(); i++) {
        if (local_results[i]) {
            if (!head) {
                head = local_results[i];
            } else {
                tail->next = local_results[i];
            }
            tail = local_results[i];
            while (tail->next) tail = tail->next;
        }
    }

    return head;
}

// Display search results with pagination
int csv_results_display(CsvResult *results, size_t limit, char *querry, double time)
{
    const char titleColumn[] = "Column";
    int i_format_column = sizeof(titleColumn) - 1;
    
    CsvResult *current;
    size_t total_results = 0;
    // Count total results
    for (current = results; current; current = current->next)
    {
        if (current->column_name)
        {
            int len = strlen(current->column_name);
            if (i_format_column < len)
            {
                i_format_column = len;
            }
        }
        total_results++;
    }
    size_t current_page = 1;
    size_t total_pages = (total_results + limit - 1) / limit; // Ceiling division

    char choice[5];
    int ret = 0;
    // TODO: Compute max column width
    do
    {
        // Reset current and go to page #

        size_t idx = 0;
        for (current = results; idx < (current_page - 1) * limit; idx++)
        {
            current = current->next;
        }

        // Clear screen
        printf("\033[2J\033[H"); // ANSI escape codes to clear screen and move cursor to home position
        printf("Search Results for Query: %s\n", querry);
        printf("Time taken: %f seconds\n", time);

        // Display page header
        printf("\n===== Search Results (Page %zu of %zu) =====\n\n", current_page, total_pages);
        printf("%-5s %-*s %-8s %-30s %-10s\n", "No.", i_format_column, titleColumn, "Line", "Result", results->isCorrected ? "Corrected" : "");
        printf("--------------------------------------------------------------\n");

        for (size_t i = 0; current && i < limit; i++, current = current->next, idx++)
        {
            printf("%-5zu %-*s %-8zu %-30s %-5s\n",
                   idx + 1,
                   i_format_column,
                   current->column_name ? current->column_name : "N/A",
                   current->line_number,
                   current->result,
                   current->isCorrected ? format_message("%d", current->distance) : "");
        }

        // Display navigation options
        printf("\n===== Navigation =====\n");
        printf("(#) Page # | (N)ext page | (P)revious page | (L)ast page | (F)irst page | (Q)uit\n");
        printf("Choice: ");

        // Get user choice
        fflush(stdin);
        if (!fgets(choice, sizeof(choice), stdin))
            break;
        choice[strcspn(choice, "\n")] = 0; // Remove newline character
        // Convert to lowercase for case-insensitive comparison
        strlwr(choice);
        // Process user choice
        switch (choice[0])
        {
        case 'n':
            if (current_page < total_pages)
            {
                current_page++;
            }
            break;
        case 'p':
            if (current_page > 1)
            {
                current_page--;
            }
            break;
        case 'l':
            current_page = total_pages;
            break;
        case 'f':
            current_page = 1;
            break;
        case 'q':
            ret = 1; // Signal to quit
            break;
        default:
            {
            // Go to page nbr choice
            char *end;
            size_t pageGOTO = strtoul(choice, &end, 0);
            printf("PAGE GOTO %zu, *end = %d\n\n", pageGOTO, *end);
            if (!(*end) && pageGOTO && pageGOTO <= total_pages)
            {
                current_page = pageGOTO;
            }
            break;
            }
        }

    } while (ret == 0);

    return total_results;
}