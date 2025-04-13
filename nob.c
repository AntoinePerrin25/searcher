#define NOB_IMPLEMENTATION
#include <nob.h>

#define FLAG_IMPLEMENTATION
#include "include/flag.h"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool *help = flag_bool("h", false, "Help Flag to display Usage");
    flag_add_alias(help, "help");

    bool *debug = flag_bool("g", false, "Enable debug information");
    flag_add_alias(debug, "debug");

    bool *opti = flag_bool("O", false, "Enable O2 optimizations");

    bool *run = flag_bool("r", false, "Run the command after building it");
    flag_add_alias(run, "run");

    bool *pedantic = flag_bool("pedantic", false, "Enable pedantic flag");
    flag_add_alias(pedantic, "ped");

    bool *all = flag_bool("a", false, "Enable all common flags");
    flag_add_alias(all, "all");
    flag_add_alias(debug, "all");
    flag_add_alias(opti, "all");
    flag_add_alias(pedantic, "all");

    if (all)
    {
        *debug = true;
        *opti = true;
        *pedantic = true;
    }

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

            if (*debug)
            {
                nob_cmd_append(&cmd, "-g");
            }
            if (*opti)
            {
                nob_cmd_append(&cmd, "-O3");
            }
            if (*pedantic)
            {
                nob_cmd_append(&cmd, "-pedantic", "-std=c99");
            }
            nob_cmd_append(&cmd, "-Iinclude");
            nob_cmd_append(&cmd, "-Llib");
            nob_cmd_append(&cmd, "-o", "main");
            nob_cmd_append(&cmd, "src/main.c", "src/csv.c");
            if (!nob_cmd_run_sync(cmd)) return 1;
            else
            {
                if (*run)
                {
                    cmd = (Nob_Cmd){0};
                    nob_cmd_append(&cmd, "main");
                    if (!nob_cmd_run_sync(cmd)) return 1;
                }
            }
        }
    }
    return 0;
}