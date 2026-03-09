#define _POSIX_C_SOURCE 199309L
#define ALIB_BASE_INC_STRIP_PREFIX 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>

#include "../../base/base_inc.c"
#include "../../base/base_inc.h"

Arena *g_arena = NULL;

int main(int argc, char **argv) {
  g_arena = arena_alloc();
  Arena *term_arena = arena_alloc();

  printfln("test");
}
