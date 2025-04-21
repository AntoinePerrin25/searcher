# CSV Searcher

A fast and efficient CSV file search tool with fuzzy matching capabilities.

## Features

- Search through CSV files with or without headers
- Multiple search modes:
  - Contains
  - Starts with
  - Ends with 
  - Full text search
- Fuzzy search with Levenshtein distance correction
- Parallel search using OpenMP
- Interactive pagination of results
- Support for multiple input files
- Performance metrics (search time)

## Building

Requires GCC with OpenMP support.

Compiling thanks to tsoding NoBuild (Nob) file : https://github.com/tsoding/nob.h
First compile nob :

```bash
gcc -o nob nob.c
```

```bash
# Build with all optimizations
./nob -a

# Or build with specific flags
./nob -g -O -pedantic
```

Available Build flags:
- `-g`        : Include debug information
- `-O`        : Enable O2 optimizations
- `-pedantic` : Enable strict C99 compliance
- `-omp`      : Enable OpenMP support 
- `-a`        : Enable all flags above

## Usage

```bash
search [OPTIONS] 

Options:
    -h, --help            Display help information
    -c, --correction      Enable fuzzy search/correction
    -ih, --input-header   Input CSV file with header
    -in, --input-noheader Input CSV file without header
    -q, --query           Search query
    -s, --separator       CSV separator character
        Default: ,
    -l, --limit          Limit number of results
        Default: 10
    -col, --column        Column to search (0=all)
        Default: 0
    -t, --type           Search type (0=all, 1=contains, 2=starts_with, 3=ends_with)
        Default: 0
```

## Examples

Search in a CSV file with headers:
```bash
search -ih data.csv -q "John" -col 1
```

Search with fuzzy matching:
```bash
search -ih users.csv -q "Smith" -c -t 1
```

## Navigation

When viewing results:
- N: Next page
- P: Previous page
- F: First page
- L: Last page
- #: Go to page number
- Q: Quit

## Performance

- Parallel search implementation using OpenMP
- Efficient string comparison algorithms
- Memory optimized data structures
- Search time measurements included in results

## TODO


- [] Malloc entire file and point entries to file buffer instead of dynamically copying everything
- [] Allow taking multiple files at once
- [] Display source file if multiple files treated

## License

This project is licensed under the MIT License.