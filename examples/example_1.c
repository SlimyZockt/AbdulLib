#define _POSIX_C_SOURCE 199309L
#define ALIB_BASE_INC_STRIP_PREFIX 1
#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>


#include "../base/base_inc.h"
#include "../base/base_inc.c"

#define MAIN_SCREEN_BUFFER "\033[?1049l"
#define ALTERNATE_SCREEN_BUFFER "\033[?1049h"
#define SHOW_CURSOR "\033[?25h"
#define HIDE_CURSOR "\033[?25l"
#define CURSOR_HOME "\033[H"
#define CLEAR_SCREEN "\033[2J"

#define Pos(x, y, w) (((x)) + ((y)*w))
#define FrameBufferPos(x, y, w) Pos((x) + 3, y, w)

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


ALibStruct(TermData){
    char *frame_buffer;
    char *board;
    char *next_board;
    u64   board_size;
    u64   frame_buffer_size;
    u64   screen_width;
    u64   screen_height;
};

void term_setup(TermData *td, struct winsize *ws, Arena *arena) {
    td->screen_width = ws->ws_col;
    td->screen_height = ws->ws_row;

    td->board_size = (td->screen_height * td->screen_width);
    td->frame_buffer_size = 3 + td->board_size;
    td->frame_buffer = push_array(arena, char, td->frame_buffer_size);
    td->board = push_array(arena, char, td->board_size);
    memset(td->board, ' ', td->board_size);
    td->next_board = push_array(arena, char, td->board_size);

    td->frame_buffer[0] = '\033';
    td->frame_buffer[1] = '[';
    td->frame_buffer[2] = 'H';
}


int main(int argc, char **argv) {
    g_arena = arena_alloc();
    Arena *term_arena = arena_alloc();

    struct winsize ws = {0};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    // printfln("=== Game Of Life ===");
    TermData td = {0};
    term_setup(&td, &ws, term_arena);

    u64 cx = td.screen_width / 2.0f;
    u64 cy = td.screen_height / 2.0f;

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
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1){ // Resize Term
            if (ws.ws_col != td.screen_width || ws.ws_row != td.screen_height) {
                Temp temp={0};
                DeferLoop(temp = temp_begin(g_arena), temp_end(temp)){
                    u64 old_width = td.screen_width;
                    u64 old_height = td.screen_height;
                    char *old_board = push_array(temp.arena, char, td.board_size);
                    memcpy(old_board, td.board, td.board_size);
                    arena_clear(term_arena);
                    term_setup(&td, &ws, term_arena);

                    u64 copy_width = (old_width < td.screen_width) ? old_width : td.screen_width;
                    u64 copy_height = (old_height < td.screen_height) ? old_height : td.screen_height;
                    for EachIndex(y, copy_height) {
                        for EachIndex(x, copy_width) {
                            td.board[Pos(x, y, td.screen_width)] =
                                old_board[Pos(x, y, old_width)];
                        }
                    }

                    if (cx >= td.screen_width) cx = td.screen_width - 1;
                    if (cy >= td.screen_height - 1) cy = td.screen_height - 2;

                    write(STDOUT_FILENO, CLEAR_SCREEN, 4);
                }
            }
        }

        b8 should_toggle_tile = 0;

        switch (read_key()) {
            case KEY_QUIT:  goto exit;
            case KEY_UP:    if(cy > 0) cy -= 1;                        break;    
            case KEY_DOWN:  if(cy < td.screen_height-2) cy += 1;          break;  
            case KEY_LEFT:  if(cx > 0) cx-= 1;                         break;  
            case KEY_RIGHT: if(cx < td.screen_width-1) cx+= 1;            break; 
            // case KEY_PLACE: if(!is_simulating) should_toggle_tile = 1; break; 
            case KEY_PLACE: should_toggle_tile = 1; break; 
            case KEY_PLAY:  is_simulating = !is_simulating;            break; 
            default:                                                   break;
        }

        {// draw cursor
            if (should_toggle_tile) {
                should_toggle_tile = 0;
                td.board[Pos(cx,cy,td.screen_width)] = td.board[Pos(cx,cy,td.screen_width)] == '#' ? ' ' : '#';
                animate_toggle = 1;
            }

            if (is_simulating && frame_count % sim_interval == 0){ // game of life
                for EachIndex(y, td.screen_height) {
                    for EachIndex(x, td.screen_width) {
                        u64 i = Pos(x,y,td.screen_width);
                        u8 alive_count = get_alive_neighbors(td.board+i, x, y, td.screen_width, td.screen_height);
                        if (td.board[i] == '#') {
                            if (alive_count < 2 || 3 < alive_count) td.next_board[i] = ' ';
                            if (alive_count == 2 || 3 == alive_count) td.next_board[i] = '#';
                        }
                        if (td.board[i] == ' ') {
                            if (alive_count != 3) td.next_board[i] = ' ';
                            if (alive_count == 3) td.next_board[i] = '#';
                        }
                    }
                }
                char *tmp = td.board;
                td.board = td.next_board;
                td.next_board = tmp;
            }

            for EachIndex(y, td.screen_height) {
                for EachIndex(x, td.screen_width) {
                    td.frame_buffer[FrameBufferPos(x, y,td.screen_width)] = td.board[Pos(x, y,td.screen_width)];
                }
            }
            
            td.frame_buffer[FrameBufferPos(cx,cy,td.screen_width)] = '@';
            if (animate_toggle) {
                td.frame_buffer[FrameBufferPos(cx,cy,td.screen_width)] = '$';
            }
            if (animate_toggle && frame_count % animaion_dur == 0) {
                animate_toggle = 0;
            }

            char *pause_state = is_simulating ? "Play " : "Pause";
            memcpy(td.frame_buffer + FrameBufferPos(0,td.screen_height-1,td.screen_width), "WASD/HJKL: Move | Q: Exit | E: Place | P: ", 42);
            memcpy(td.frame_buffer + FrameBufferPos(42,td.screen_height-1,td.screen_width), pause_state, 6);
            write(STDOUT_FILENO, td.frame_buffer, td.frame_buffer_size);
        }

    
        frame_count += 1;
        nanosleep(&ts, NULL);
        continue;
        exit: break;
    }

}

