#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "../vendored/nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER ""

#define OUT_FILE "example_1"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    nob_cc(&cmd);
    nob_cmd_append(&cmd, "-std=c11", "-g");
    // nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, SRC_FOLDER "example_1.c");
    nob_cc_output(&cmd, BUILD_FOLDER OUT_FILE);

    // example_1
    if (!nob_cmd_run(&cmd)) return 1; 

    nob_cmd_append(&cmd, BUILD_FOLDER OUT_FILE);

    if (!nob_cmd_run(&cmd)) {
        return 1;
    }

    return 0;
}

