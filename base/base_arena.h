#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

#if ALIB_BASE_ARENA_STRIP_PREFIX 
#define Arena alib_Arena
#define ArenaFlags alib_ArenaFlags
#define ArenaParams alib_ArenaParams
#define ArenaParams alib_ArenaParams
#define Temp alib_Temp
#define arena_default_reserve_size alib_arena_default_reserve_size
#define arena_default_commit_size alib_arena_default_commit_size
#define arena_default_flags alib_arena_default_flags
#define arena_alloc_ alib_arena_alloc_
#define arena_alloc alib_arena_alloc
#define arena_release alib_arena_release
#define arena_clear alib_arena_clear
#define arena_pop alib_arena_pop
#define arena_pop_to alib_arena_pop_to
#define arena_push alib_arena_push
#define arena_pop alib_arena_pop
#define temp_begin alib_temp_begin
#define temp_end alib_temp_end

#define push_array alib_push_array
#define push_array_aligned alib_push_array_aligned
#define push_array_aligned alib_push_array_no_zero
#define push_array_no_zero_aligned alib_push_array_no_zero_aligned

#endif



typedef alib_u64 alib_ArenaFlags;
enum {
    alib_ArenaFlag_NoChain    = (1<<0),
    // alib_ArenaFlag_LargePages = alib_BitSet(1),
};

#define ALIB_ARENA_HEADER_SIZE 128
typedef struct alib_ArenaParams alib_ArenaParams;
struct alib_ArenaParams {
    alib_ArenaFlags flags;
    alib_u64 reserve_size;
    alib_u8 commit_size;
    void *optional_backing_buffer;
    alib_SourceCodeLocation loc;
};

typedef struct alib_Arena alib_Arena;
struct alib_Arena {
    alib_Arena *prev;    // previous arena in chain
    alib_Arena *current; // current arena in chain
    alib_Arena *free_last;
    alib_u64 free_size;
    alib_ArenaFlags flags;
    alib_u64 cmt_size;
    alib_u64 res_size;
    alib_u64 base_pos;
    alib_u64 pos;
    alib_u64 cmt;
    alib_u64 res;
    alib_SourceCodeLocation loc;
};

alib_StaticAssert(sizeof(Arena) <= ALIB_ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct alib_Temp alib_Temp;
struct alib_Temp {
    alib_Arena *arena;
    alib_u64 pos;
};

// Arena Functions
alib_global alib_u64 alib_arena_default_reserve_size = alib_MB(64);
alib_global alib_u64 alib_arena_default_commit_size  = alib_KB(64);
alib_global alib_ArenaFlags alib_arena_default_flags = 0;

//arena creation/destruction
alib_internal alib_Arena *alib_arena_alloc_(alib_ArenaParams *params);
#define alib_arena_alloc(...) alib_arena_alloc_(&(alib_ArenaParams){       \
        .reserve_size = alib_arena_default_reserve_size,         \
        .commit_size = alib_arena_default_commit_size,           \
        .flags = alib_arena_default_flags,                       \
        .loc = (alib_CallerLocation),                            \
        __VA_ARGS__})

alib_internal void alib_arena_release(alib_Arena *arena);

// arena push/pop/pos core functions
alib_internal void     *alib_arena_push(alib_Arena *arena, alib_u64 size, alib_u64 align);
alib_internal alib_u64  alib_arena_pos(alib_Arena *arena);
alib_internal void      alib_arena_pop_to(alib_Arena *arena, alib_u64 pos);

// arena push/pop helpers
alib_internal void alib_arena_clear(alib_Arena *arena);
alib_internal void alib_arena_pop(alib_Arena *arena, alib_u64 amt);

// temporary arena scopes
alib_internal alib_Temp alib_temp_begin(alib_Arena *arena);
alib_internal void      alib_temp_end(alib_Temp temp);

// push helper macros
#define alib_push_array_no_zero_aligned(a, T, c, align) (T *)alib_arena_push((a), sizeof(T)*(c), (align))
#define alib_push_array_aligned(a, T, c, align) (T *)alib_MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define alib_push_array_no_zero(a, T, c) alib_push_array_no_zero_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))
#define alib_push_array(a, T, c) push_array_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))

#endif
