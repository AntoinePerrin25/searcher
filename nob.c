#define NOB_IMPLEMENTATION
#include "include/nob.h"
//
#define FLAG_IMPLEMENTATION
#include "include/flag.h"


void usage(FILE* stream)
{
    fprintf(stream, "Usage: nob [OPTIONS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "include/flag.h", "include/nob.h");

    bool *help = flag_bool_aliases("h", false, "Help Flag to display Usage", "help");
    bool *debug = flag_bool_aliases("g", false, "Enable debug information", "debug");
    bool *opti = flag_bool_aliases("O", false, "Enable O2 optimizations", "opti");
    bool *pedantic = flag_bool_aliases("pedantic", false, "Enable pedantic flag", "ped");
    bool *openMP = flag_bool_aliases("openmp", false, "Enable OpenMP support", "omp");
    bool *all = flag_bool_aliases("a", false, "Enable all common flags", "all");
    
    if (!flag_parse(argc, argv))
    {
        printf("Error parsing flags\n");
        usage(stderr);
    }
    else
    {
        if (*help)
        {
            printf("Printing Flags\n");
            usage(stdout);
        }
        else
        {
            Nob_Cmd cmd = {0};
            nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra");


            if (*all || *debug)
                nob_cmd_append(&cmd, "-g");
            if (*all || *opti)
                nob_cmd_append(&cmd, "-O2");
            if (*all || *pedantic)
                nob_cmd_append(&cmd, "-pedantic", "-std=c99");
            if (*all || *openMP)
                nob_cmd_append(&cmd, "-fopenmp");
            
            nob_cmd_append(&cmd, "-Iinclude");
            nob_cmd_append(&cmd, "-o", "search");
            nob_cmd_append(&cmd, "src/main.c", "src/csv.c");
            if (!nob_cmd_run_sync(cmd)) return 1;
        }
    }
    return 0;
}