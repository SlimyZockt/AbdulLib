#define _POSIX_C_SOURCE 199309L
#define ALIB_BASE_INC_STRIP_PREFIX 1
#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>


#include "../base/base_inc.h"
#include "../base/base_inc.c"

#define printfln_array(fstr, arr, count)   \
    Statement(                                 \
            printf("[");                           \
            for EachIndex(it, count) {             \
            printf(fstr, arr[it]);             \
            printf(", ");                      \
            }                                      \
            printf("]\n");                         \
            )

#define printfln(str, ...) printf(str "\n", ##__VA_ARGS__)

struct termios g_og_termios = {0};
Arena *g_arena = NULL;

void disable_raw_mode() {
    write(STDOUT_FILENO, "\033[?1049l", 8);
    write(STDOUT_FILENO, "\033[?25h", 6);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_og_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &g_og_termios);
    atexit(disable_raw_mode);

    struct termios raw = g_og_termios;
    raw.c_lflag &= ~(ECHO | ICANON); 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    write(STDOUT_FILENO, "\033[?1049h", 8);
    write(STDOUT_FILENO, "\033[?25l", 6);
}

int main(int argc, char **argv) {
    g_arena = arena_alloc();
    ALib(u64) board_width = 0;
    ALib(u64) board_hight = 0;

    {// Setup Term
        struct winsize ws = {0};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        board_width = ws.ws_col;
        board_hight = ws.ws_row;

        printfln("W: %lu", board_width);
        printfln("H: %lu", board_hight);
    }
    char *buffer = push_array(g_arena, char, board_width * board_hight * 4);

    float x = screen_w / 2.0f;
    float y = screen_h / 2.0f;

    printfln("=== Game Of Life ===");

    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 16666666,
    };

    enable_raw_mode();

    for (;;) {
        char c;
        {// read keys 
            if (read(STDIN_FILENO, &c, 1) == 1 && c == 'q') break;
        }

        fflush(stdout);
        nanosleep(&ts, NULL);
    }

}
