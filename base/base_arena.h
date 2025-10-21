#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

typedef alib_u64 Alib_ArenaFlags;
enum {
    Alib_ArenaFlag_NoChain    = alib_BitField(0),
    Alib_ArenaFlag_LargePages = alib_BitField(1),
};

typedef struct Alib_ArenaParams Alib_ArenaParams;
struct Alib_ArenaParams {
    Alib_ArenaFlags flags;
    alib_u64 reserve_size;
    alib_u8 commit_size;
    void *optional_backing_buffer;
    alib_SourceCodeLocation loc;
};

typedef struct Alib_Arena Alib_Arena;
struct Alib_Arena {
    Alib_Arena *prev;    // previous arena in chain
    Alib_Arena *current; // current arena in chain
    Alib_Arena *free_last;
    alib_u64 free_size;
    Alib_ArenaFlags flags;
    alib_u64 cmt_size;
    alib_u64 res_size;
    alib_u64 base_pos;
    alib_u64 pos;
    alib_u64 cmt;
    alib_u64 res;
    alib_SourceCodeLocation loc;
};

typedef struct Alib_Temp Alib_Temp;
struct Alib_Temp {
    Alib_Arena *arena;
    alib_u64 pos;
};

// Arena Functions
alib_global alib_u64 alib_arena_default_reserve_size = alib_MB(64);
alib_global alib_u64 alib_arena_default_commit_size  = alib_KB(64);
alib_global Alib_ArenaFlags alib_arena_default_flags = 0;

//arena creation/destruction
alib_internal Alib_Arena *arena_alloc_(Alib_ArenaParams *params);
#define arena_alloc(...) arena_alloc_(&(Alib_ArenaParams){       \
        .reserve_size = alib_arena_default_reserve_size,         \
        .commit_size = alib_arena_default_commit_size,           \
        .flags = alib_arena_default_flags,                       \
        .loc = (alib_CallerLocation),                            \
        __VA_ARGS__})

alib_internal void alib_arena_release(Alib_Arena *arena);

// arena push/pop/pos core functions
alib_internal void     *alib_arena_push(Alib_Arena *arena, alib_u64 size, alib_u64 align);
alib_internal alib_u64  alib_arena_pos(Alib_Arena *arena);
alib_internal void      alib_arena_pop_to(Alib_Arena *arena, alib_u64 pos);

// arena push/pop helpers
alib_internal void alib_arena_clear(Alib_Arena *arena);
alib_internal void alib_arena_pop(Alib_Arena *arena, alib_u64 amt);

// temporary arena scopes
alib_internal Alib_Temp alib_temp_begin(Alib_Arena *arena);
alib_internal void      alib_temp_end(Alib_Temp temp);

// push helper macros
#define alib_push_array_no_zero_aligned(a, T, c, align) (T *)alib_arena_push((a), sizeof(T)*(c), (align))
#define alib_push_array_aligned(a, T, c, align) (T *)alib_MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define alib_push_array_no_zero(a, T, c) alib_push_array_no_zero_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))
#define alib_push_array(a, T, c) push_array_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))

#endif
