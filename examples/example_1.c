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
            for EachIndex(it, (count)) {       \
            printf((fstr), (arr)[it]);         \
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

#define FrameBufferPos(x, y) ((3+(x)) + ((y)*screen_width))
#define BoardPos(x, y) (((x)) + ((y)*screen_width))

ALibEnum(Key,u8) {
    KEY_NONE  = 0,
    KEY_UP    = 1,
    KEY_DOWN  = 2,
    KEY_LEFT  = 3,
    KEY_RIGHT = 4,
    KEY_QUIT  = 5,
    KEY_PLACE = 6,
    KEY_PLAY  = 7,
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

    switch (c) {
        case 'q': case 'Q': return KEY_QUIT;
        case 'w': case 'k': return KEY_UP;
        case 's': case 'j': return KEY_DOWN;
        case 'a': case 'h': return KEY_LEFT;
        case 'd': case 'l': return KEY_RIGHT;
        case 'p': case 'P': return KEY_PLAY;
        case 'e': case 'E': return KEY_PLACE;
    }

    return KEY_NONE;
}

u8 get_alive_neighbors(char *cell, u64 x, u64 y, u64 width, u64 height){
    u8 count = 0;

    if (x > 0 && cell[-1] == '#')                                  count += 1; // left
    if (x + 1 < width && cell[1] == '#')                           count += 1; // right
    if (y > 0 && cell[-width] == '#')                              count += 1; // up
    if (y + 1 < height && cell[width] == '#')                      count += 1; // down
    if (x > 0 && y > 0 && cell[-1 - width] == '#')                 count += 1; // left+up
    if (x + 1 < width && y > 0 && cell[1 - width] == '#')          count += 1; // right+up
    if (x > 0 && y + 1 < height && cell[-1 + width] == '#')        count += 1; // left+down
    if (x + 1 < width && y + 1 < height && cell[1 + width] == '#') count += 1; // right+down

    return count;
}

int main(int argc, char **argv) {
    g_arena = arena_alloc();
    u64 screen_width = 0;
    u64 screen_height = 0;

    {// Setup Term
        struct winsize ws = {0};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        screen_width = ws.ws_col;
        screen_height = ws.ws_row;
    }

    u64 board_size = (screen_height * screen_width);
    u64 frame_buffer_size = 3 + board_size;
    char *frame_buffer = push_array(g_arena, char, frame_buffer_size);
    char *board = push_array(g_arena, char, board_size);
    memset(board, ' ', board_size);
    char *next_board = push_array(g_arena, char, board_size);
    memset(next_board, ' ', board_size);

    frame_buffer[0] = '\033';
    frame_buffer[1] = '[';
    frame_buffer[2] = 'H';

    u64 cx = screen_width / 2.0f;
    u64 cy = screen_height / 2.0f;

    printfln("=== Game Of Life ===");

    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 16666666,
    };

    enable_raw_mode();

    b8 is_simulating = 0;
    b8 animate_toggle = 0;
    u64 frame_count = 0;
    const u64 sim_interval = 6;
    const u64 animaion_dur = 15;
    for (;;) {
        if( ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1){ // Setup Term
             
        }

        b8 should_toggle_tile = 0;

        switch (read_key()) {
            case KEY_QUIT:  goto exit;
            case KEY_UP:    if(cy > 0) cy -= 1;                        break;    
            case KEY_DOWN:  if(cy < screen_height-2) cy += 1;          break;  
            case KEY_LEFT:  if(cx > 0) cx-= 1;                         break;  
            case KEY_RIGHT: if(cx < screen_width-1) cx+= 1;            break; 
            // case KEY_PLACE: if(!is_simulating) should_toggle_tile = 1; break; 
            case KEY_PLACE: should_toggle_tile = 1; break; 
            case KEY_PLAY:  is_simulating = !is_simulating;            break; 
            default:                                                   break;
        }

        {// draw cursor
            if (should_toggle_tile) {
                should_toggle_tile = 0;
                board[BoardPos(cx,cy)] = board[BoardPos(cx,cy)] == '#' ? ' ' : '#';
                animate_toggle = 1;
            }

            if (is_simulating && frame_count % sim_interval == 0){ // game of life
                for EachIndex(y, screen_height) {
                    for EachIndex(x, screen_width) {
                        u64 i = BoardPos(x,y);
                        u8 alive_count = get_alive_neighbors(board+i, x, y, screen_width, screen_height);
                        if (board[i] == '#') {
                            if (alive_count < 2 || 3 < alive_count) next_board[i] = ' ';
                            if (alive_count == 2 || 3 == alive_count) next_board[i] = '#';
                        }
                        if (board[i] == ' ') {
                            if (alive_count != 3) next_board[i] = ' ';
                            if (alive_count == 3) next_board[i] = '#';
                        }
                    }
                }
                char *tmp = board;
                board = next_board;
                next_board = tmp;
            }

            for EachIndex(y, screen_height) {
                for EachIndex(x, screen_width) {
                    frame_buffer[FrameBufferPos(x, y)] = board[BoardPos(x, y)];
                }
            }
            
            frame_buffer[FrameBufferPos(cx,cy)] = '@';
            if (animate_toggle) {
                frame_buffer[FrameBufferPos(cx,cy)] = '$';
            }
            if (animate_toggle && frame_count % animaion_dur == 0) {
                animate_toggle = 0;
            }

            char *pause_state = is_simulating ? "Play " : "Pause";
            memcpy(frame_buffer + FrameBufferPos(0,screen_height-1), "WASD/HJKL: Move | Q: Exit | E: Place | P: ", 42);
            memcpy(frame_buffer + FrameBufferPos(42,screen_height-1), pause_state, 6);
            write(STDOUT_FILENO, frame_buffer, frame_buffer_size);
        }

    
        frame_count += 1;
        nanosleep(&ts, NULL);
        continue;
        exit: break;
    }

}

