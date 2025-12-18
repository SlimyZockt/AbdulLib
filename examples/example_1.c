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


#define printfln_array(fstr, arr, count)       \
    Statement(                                 \
            printf("[");                       \
            for EachIndex(it, count) {         \
            printf(fstr, arr[it]);             \
            printf(", ");                      \
            }                                  \
            printf("]\n");                     \
            )

#define printfln(str, ...) printf(str "\n", ##__VA_ARGS__)

#define MAIN_SCREEN_BUFFER "\033[?1049l"
#define ALTERNATE_SCREEN_BUFFER "\033[?1049h"
#define SHOW_CURSOR "\033[?25h"
#define HIDE_CURSOR "\033[?25l"
#define CURSOR_HOME "\033[H"
#define CLEAR_SCREEN "\033[2J"

#define FrameBufferPos(x, y) ((3+x) + (y*screen_width))

ALibEnum(Key,u8) {
    KEY_NONE  = 0,
    KEY_UP    = 1,
    KEY_DOWN  = 2,
    KEY_LEFT  = 3,
    KEY_RIGHT = 4,
    KEY_QUIT  = 5,
};

struct termios g_og_termios = {0};
Arena *g_arena = NULL;

void disable_raw_mode() {
    write(STDOUT_FILENO, MAIN_SCREEN_BUFFER, 8);
    write(STDOUT_FILENO, SHOW_CURSOR, 6);
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

    write(STDOUT_FILENO, ALTERNATE_SCREEN_BUFFER, 8);
    write(STDOUT_FILENO, HIDE_CURSOR, 6);
    write(STDOUT_FILENO, CLEAR_SCREEN, 4);
    write(STDOUT_FILENO, CURSOR_HOME, 3);
}

Key read_key() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return KEY_NONE;

    if (c == 'q' || c == 'Q') return KEY_QUIT;

    // Arrow keys: \033[A (up), \033[B (down), \033[C (right), \033[D (left)
    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_NONE;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_NONE;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
    }

    // WASD / vim keys
    switch (c) {
        case 'w': case 'k': return KEY_UP;
        case 's': case 'j': return KEY_DOWN;
        case 'a': case 'h': return KEY_LEFT;
        case 'd': case 'l': return KEY_RIGHT;
    }

    return KEY_NONE;
}

int main(int argc, char **argv) {
    g_arena = arena_alloc();
    u64 screen_width = 0;
    u64 screen_hight = 0;

    {// Setup Term
        struct winsize ws = {0};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        screen_width = ws.ws_col;
        screen_hight = ws.ws_row;

        printfln("W: %lu", screen_width);
        printfln("H: %lu", screen_hight);
    }

    u64 frame_buffer_size = 3 + (screen_hight * screen_width);
    char *frame_buffer = push_array(g_arena, char, frame_buffer_size);

    frame_buffer[0] = '\033';
    frame_buffer[1] = '[';
    frame_buffer[2] = 'H';

    u64 cx = screen_width / 2.0f;
    u64 cy = screen_hight / 2.0f;

    printfln("=== Game Of Life ===");

    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 16666666,
    };

    enable_raw_mode();

    for (;;) {
        switch (read_key()) {
            case KEY_QUIT: goto exit;
            case KEY_UP:    cy -= 1; break;    
            case KEY_DOWN:  cy += 1; break;  
            case KEY_LEFT:  cx -= 1; break;  
            case KEY_RIGHT: cx += 1; break; 
            default: break;
        }

        {// draw cursor

            for EachIndex(y, screen_hight) {
                for EachIndex(x, screen_width) {
                    frame_buffer[FrameBufferPos(x,y)] = ' ';
                    if (y == cy && x == cx) {
                        frame_buffer[FrameBufferPos(x,y)] = '@';
                    }
                }
            }

            memcpy(frame_buffer + FrameBufferPos(0,(screen_hight-1)), "WASD/HJKL: move | Q: exit", 25);

            write(STDOUT_FILENO, frame_buffer, frame_buffer_size);
        }


        nanosleep(&ts, NULL);
        continue;
exit: break;
    }

}
