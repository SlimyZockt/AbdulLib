#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

ALibEnum(ArenaFlags, U64) {
    ArenaFlag_NoChain = (1 << 0),
    // ArenaFlag_LargePages = (2<<0),
};

#define ARENA_HEADER_SIZE 128

ALibStruct(ArenaParams) {
    ArenaFlags flags;
    U64 reserve_size;
    U64 commit_size;
    void *optional_backing_buffer;
    SourceLocation loc;
};

ALibStruct(Arena) {
    Arena *prev;
    Arena *current;
    Arena *free_last;
    U64 free_size;
    ArenaFlags flags;
    U64 cmt_size;
    U64 res_size;
    U64 base_pos;
    U64 pos;
    U64 cmt;
    U64 res;
    SourceLocation loc;
};

StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

ALibStruct(Temp) {
    Arena *arena;
    U64 pos;
};

// Arena Functions
global U64 arena_default_reserve_size = MB(64);
global U64 arena_default_commit_size = KB(64);
global ArenaFlags arena_default_flags = 0;

// arena creation/destruction
ALIB_DEF Arena *arena_alloc_(ArenaParams *params);

#define arena_alloc(...) arena_alloc_(&(ArenaParams){                  \
        .reserve_size = arena_default_reserve_size,                    \
        .commit_size = arena_default_commit_size,                      \
        .flags = arena_default_flags,                                  \
        .loc = (CallerLocation),                                       \
        __VA_ARGS__})

ALIB_DEF void arena_release(Arena *arena);

// arena push/pop/pos core functions
ALIB_DEF void *arena_push(Arena *arena, U64 size, U64 align, B32 zero);

ALIB_DEF U64 arena_pos(Arena *arena);

ALIB_DEF void arena_pop_to(Arena *arena, U64 pos);

// arena push/pop helpers
ALIB_DEF void arena_clear(Arena *arena);

ALIB_DEF void arena_pop(Arena *arena, U64 amt);

// temporary arena scopes
ALIB_DEF Temp temp_begin(Arena *arena);

ALIB_DEF void temp_end(Temp temp);

// push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align), (0))
#define push_array_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align), (1))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))
#endif
