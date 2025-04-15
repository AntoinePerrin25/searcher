#define NOB_IMPLEMENTATION
#include <nob.h>
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
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool *help = flag_bool("h", false, "Help Flag to display Usage");
    flag_add_alias(help, "help");

    bool *debug = flag_bool("g", false, "Enable debug information");
    flag_add_alias(debug, "debug");

    bool *opti = flag_bool("O", false, "Enable O2 optimizations");
    
    bool *pedantic = flag_bool("ped", false, "Enable pedantic flag");
    flag_add_alias(pedantic, "pedantic");
    
    bool *unused = flag_bool("u", false, "Disable unused warnings");
    flag_add_alias(unused, "unused");
    
    bool *all = flag_bool("a", true, "Enable all common flags");
    flag_add_alias(all, "all");
    flag_add_alias(debug, "all");
    flag_add_alias(opti, "all");
    flag_add_alias(pedantic, "all");
    flag_add_alias(unused, "all");

    bool *run = flag_bool("r", false, "Run the command after building it");
    flag_add_alias(run, "run");
    
    bool *gui = flag_bool("gui", false, "Build the GUI version with raylib");
    
    bool *download_raylib = flag_bool("dl-raylib", false, "Download and build raylib");

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
            // Check if we need to download raylib
            if (*download_raylib) {
                printf("Downloading and building raylib...\n");
                
                // Create lib directory if it doesn't exist
                Nob_Cmd mkdir_cmd = {0};
                #ifdef _WIN32
                nob_cmd_append(&mkdir_cmd, "mkdir", "lib");
                #else
                nob_cmd_append(&mkdir_cmd, "mkdir", "-p", "lib");
                #endif
                nob_cmd_run_sync(mkdir_cmd);
                
                // Clone raylib repository
                Nob_Cmd git_cmd = {0};
                nob_cmd_append(&git_cmd, "git", "clone", "https://github.com/raysan5/raylib.git", "lib/raylib");
                if (!nob_cmd_run_sync(git_cmd)) {
                    fprintf(stderr, "Failed to clone raylib repository\n");
                    return 1;
                }
                
                // Build raylib
                Nob_Cmd build_cmd = {0};
                #ifdef _WIN32
                // On Windows, we'll use MinGW's make or CMake
                nob_cmd_append(&build_cmd, "cd", "lib\\raylib\\src", "&&", "mingw32-make", "PLATFORM=PLATFORM_DESKTOP");
                #else
                nob_cmd_append(&build_cmd, "cd", "lib/raylib/src", "&&", "make", "PLATFORM=PLATFORM_DESKTOP");
                #endif
                if (!nob_cmd_run_sync(build_cmd)) {
                    fprintf(stderr, "Failed to build raylib\n");
                    return 1;
                }
                
                printf("Raylib downloaded and built successfully\n");
            }
            
            if (*gui) {
                // Build the GUI version with raylib
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra");
                
                if (*debug)
                    nob_cmd_append(&cmd, "-g");
                if (*opti)
                    nob_cmd_append(&cmd, "-O2");
                if (*pedantic)
                    nob_cmd_append(&cmd, "-pedantic", "-std=c99");
                if (*unused)
                    nob_cmd_append(&cmd, "-Wno-unused-parameter");
                    
                nob_cmd_append(&cmd, "-Iinclude");
                
                nob_cmd_append(&cmd, "-Llib/raylib/src");
                nob_cmd_append(&cmd, "-o", "gui_search.exe");
                nob_cmd_append(&cmd, "src/csv.c", "src/gui/gui_search.c");
                nob_cmd_append(&cmd, "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm", "-lws2_32");
                
                if (!nob_cmd_run_sync(cmd)) return 1;
                else {
                    if (*run) {
                        cmd = (Nob_Cmd){0};
                        nob_cmd_append(&cmd, "./gui_search");
                        if (!nob_cmd_run_sync(cmd)) return 1;
                    }
                }
            } else {
                // Build the regular command-line version
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra");

                if (*debug)
                    nob_cmd_append(&cmd, "-g");
                if (*opti)
                    nob_cmd_append(&cmd, "-O2");
                if (*pedantic)
                    nob_cmd_append(&cmd, "-pedantic", "-std=c99");
                if (*unused)
                    nob_cmd_append(&cmd, "-Wno-unused-parameter");
                    
                nob_cmd_append(&cmd, "-Iinclude");
                nob_cmd_append(&cmd, "-Llib");
                nob_cmd_append(&cmd, "-o", "search");
                nob_cmd_append(&cmd, "src/main.c", "src/csv.c");
                if (!nob_cmd_run_sync(cmd)) return 1;
                else
                {
                    if (*run)
                    {
                        cmd = (Nob_Cmd){0};
                        nob_cmd_append(&cmd, "./search");
                        if (!nob_cmd_run_sync(cmd)) return 1;
                    }
                }
            }
        }
    }
    return 0;
}