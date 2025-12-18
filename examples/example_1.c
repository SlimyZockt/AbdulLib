#include <stdio.h>
#include <stdio.h>

#define ALIB_BASE_INC_STRIP_PREFIX 1
#include "../base/base_inc.h"
#include "../base/base_inc.c"


int main(int argc, char **argv) {
    ALib(u64) board_width = 0;
    ALib(u64) board_hight = 0;


    printf("=== Game Of Life ===\n");

    ALib(Arena)* arena = arena_alloc();
    // char* buf = push_array(arena, char, 10);
    for (;;) {
       // char* out = fgets(buf, sizeof(char)*10, stdin);

       // if (out == NULL) { continue; }
       //
       // printf("%s \n", out);

    }




}



