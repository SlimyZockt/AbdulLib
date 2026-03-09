#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

#if ALIB_BASE_ARENA_STRIP_PREFIX 
#define arena_alloc alib_arena_alloc
#define arena_release alib_arena_release

#define arena_push alib_arena_push
#define arena_pos alib_arena_pos
#define arena_pop_to alib_arena_pop_to

#define arena_clear alib_arena_clear
#define arena_pop alib_arena_pop

#define temp_begin alib_temp_begin
#define temp_end alib_temp_end

#define push_array alib_push_array
#define push_array_aligned alib_push_array_aligned
#define push_array_no_zero alib_push_array_no_zero
#define push_array_no_zero_aligned alib_push_array_no_zero_aligned

#define ArenaFlags ALibArenaFlags
#define ArenaFlag_NoChain ALibArenaFlag_NoChain
#define ArenaParams ALibArenaParams
#define Arena ALibArena
#define Temp ALibTemp

#define arena_default_reserve_size alib_arena_default_reserve_size
#define arena_default_commit_size alib_arena_default_commit_size
#define arena_default_flags alib_arena_default_flags

// Test

#endif

ALibEnum(ALibArenaFlags, ALibU64) {
    ALibArenaFlag_NoChain    = (1<<0),
    // ALibArenaFlag_LargePages = (2<<0),
};

#define ALIB_ARENA_HEADER_SIZE 128

ALibStruct(ALibArenaParams){
    ALibArenaFlags flags;
    ALibU64 reserve_size;
    ALibU64 commit_size;
    void *optional_backing_buffer;
    ALibSourceLocation loc;
};

ALibStruct(ALibArena){
    ALibArena *prev;
    ALibArena *current;
    ALibArena *free_last;
    ALibU64 free_size;
    ALibArenaFlags flags;
    ALibU64 cmt_size;
    ALibU64 res_size;
    ALibU64 base_pos;
    ALibU64 pos;
    ALibU64 cmt;
    ALibU64 res;
    ALibSourceLocation loc;
};

ALibStaticAssert(sizeof(ALibArena) <= ALIB_ARENA_HEADER_SIZE, arena_header_size_check);

ALibStruct(ALibTemp){
    ALibArena *arena;
    ALibU64 pos;
};

// Arena Functions
ALibGlobal ALibU64 alib_arena_default_reserve_size = ALibMB(64);
ALibGlobal ALibU64 alib_arena_default_commit_size  = ALibKB(64);
ALibGlobal ALibArenaFlags alib_arena_default_flags = 0;

// arena creation/destruction
ALIB_DEF ALibArena *alib_arena_alloc_(ALibArenaParams *params);
#define alib_arena_alloc(...) alib_arena_alloc_(&(ALibArenaParams){   \
        .reserve_size = alib_arena_default_reserve_size,                    \
        .commit_size = alib_arena_default_commit_size,                      \
        .flags = alib_arena_default_flags,                                  \
        .loc = (ALibCallerLocation),                                        \
        __VA_ARGS__})

ALIB_DEF void alib_arena_release(ALibArena *arena);

// arena push/pop/pos core functions
ALIB_DEF void      *alib_arena_push(ALibArena *arena, ALibU64 size, ALibU64 align, ALibB32 zero);
ALIB_DEF ALibU64    alib_arena_pos(ALibArena *arena);
ALIB_DEF void       alib_arena_pop_to(ALibArena *arena, ALibU64 pos);

// arena push/pop helpers
ALIB_DEF void alib_arena_clear(ALibArena *arena);
ALIB_DEF void alib_arena_pop(ALibArena *arena, ALibU64 amt);

// temporary arena scopes
ALIB_DEF ALibTemp alib_temp_begin(ALibArena *arena);
ALIB_DEF void     alib_temp_end(ALibTemp temp);

// push helper macros
#define alib_push_array_no_zero_aligned(a, T, c, align) (T *)alib_arena_push((a), sizeof(T)*(c), (align), (0))
#define alib_push_array_aligned(a, T, c, align) (T *)alib_arena_push((a), sizeof(T)*(c), (align), (1))
#define alib_push_array_no_zero(a, T, c) alib_push_array_no_zero_aligned(a, T, c, ALibMax(8, ALibAlignOf(T)))
#define alib_push_array(a, T, c) alib_push_array_aligned(a, T, c, ALibMax(8, ALibAlignOf(T)))
#endif
