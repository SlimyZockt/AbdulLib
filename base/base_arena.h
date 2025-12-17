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


ALibEnum(ArenaFlags, ALib(u64)) {
    ALib(ArenaFlag_NoChain)    = (1<<0),
    // ALib(ArenaFlag_LargePages)    = (2<<0),
};


#define ALIB_ARENA_HEADER_SIZE 128

ALibStruct(ArenaParams){
    ALib(ArenaFlags) flags;
    ALib(u64) reserve_size;
    ALib(u8) commit_size;
    void *optional_backing_buffer;
    ALib(SourceLocation) loc;
}

ALibStruct(Arena){
    ALib(Arena) *prev;    // previous arena in chain
    ALib(Arena) *current; // current arena in chain
    ALib(Arena) *free_last;
    ALib(u64) free_size;
    ALib(ArenaFlags) flags;
    ALib(u64) cmt_size;
    ALib(u64) res_size;
    ALib(u64) base_pos;
    ALib(u64) pos;
    ALib(u64) cmt;
    ALib(u64) res;
    ALib(SourceLocation) loc;
};

ALib_StaticAssert(sizeof(ALib(Arena)) <= ALIB_ARENA_HEADER_SIZE, arena_header_size_check);

ALibStruct(Temp){
    ALib(Arena) *arena;
    ALib(u64) pos;
}

// Arena Functions
alib_global ALib(u64) alib_arena_default_reserve_size = alib_MB(64);
alib_global ALib(u64) alib_arena_default_commit_size  = alib_KB(64);
alib_global ALib(ArenaFlags) alib_arena_default_flags = 0;

// arena creation/destruction
ALIB_DEF ALib(Arena) *alib_arena_alloc_(ALib(ArenaParams) *params);
#define ALib_arena_alloc(...) ALib_arena_alloc_(&(alib_ArenaParams){       \
        .reserve_size = alib_arena_default_reserve_size,         \
        .commit_size = alib_arena_default_commit_size,           \
        .flags = alib_arena_default_flags,                       \
        .loc = (alib_CallerLocation),                            \
        __VA_ARGS__})

ALIB_DEF void alib_arena_release(alib_Arena *arena);

// arena push/pop/pos core functions
ALIB_DEF void     *alib_arena_push(alib_Arena *arena, alib_u64 size, alib_u64 align);
ALIB_DEF alib_u64  alib_arena_pos(alib_Arena *arena);
ALIB_DEF void      alib_arena_pop_to(alib_Arena *arena, alib_u64 pos);

// arena push/pop helpers
ALIB_DEF void alib_arena_clear(alib_Arena *arena);
ALIB_DEF void alib_arena_pop(alib_Arena *arena, alib_u64 amt);

// temporary arena scopes
ALIB_DEF alib_Temp alib_temp_begin(alib_Arena *arena);
ALIB_DEF void      alib_temp_end(alib_Temp temp);

// push helper macros
#define alib_push_array_no_zero_aligned(a, T, c, align) (T *)alib_arena_push((a), sizeof(T)*(c), (align))
#define alib_push_array_aligned(a, T, c, align) (T *)alib_MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define alib_push_array_no_zero(a, T, c) alib_push_array_no_zero_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))
#define alib_push_array(a, T, c) push_array_aligned(a, T, c, alib_Max(8, alib_AlignOf(T)))

#endif
