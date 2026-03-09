#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

#if ALIB_BASE_ARENA_STRIP_PREFIX 
#define arena_alloc alib_arena_alloc

#define push_array alib_push_array
#define push_array_aligned alib_push_array_aligned
#define push_array_no_zero alib_push_array_no_zero
#define push_array_no_zero_aligned alib_push_array_no_zero_aligned

#endif

ALibEnum(ALib(ArenaFlags), ALib(u64)) {
    ALib(ArenaFlag_NoChain)    = (1<<0),
    // ALib(ArenaFlag_LargePages)    = (2<<0),
};

#define ALIB_ARENA_HEADER_SIZE 128

ALibStruct(ALib(ArenaParams)){
    ALib(ArenaFlags) flags;
    ALib(u64) reserve_size;
    ALib(u64) commit_size;
    void *optional_backing_buffer;
    ALib(SourceLocation) loc;
};

ALibStruct(ALib(Arena)){
    ALib(Arena) *prev;
    ALib(Arena) *current;
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

ALibStaticAssert(sizeof(ALib(Arena)) <= ALIB_ARENA_HEADER_SIZE, arena_header_size_check);

ALibStruct(ALib(Temp)){
    ALib(Arena) *arena;
    ALib(u64) pos;
};

// Arena Functions
ALibglobal ALib(u64) ALib(arena_default_reserve_size) = ALibMB(64);
ALibglobal ALib(u64) ALib(arena_default_commit_size)  = ALibKB(64);
ALibglobal ALib(ArenaFlags) ALib(arena_default_flags) = 0;

// arena creation/destruction
ALIB_DEF ALib(Arena) *ALibProc(arena_alloc_)(ALib(ArenaParams) *params);
#define alib_arena_alloc(...) ALibProc(arena_alloc_)(&(ALib(ArenaParams)){   \
        .reserve_size = ALib(arena_default_reserve_size),                    \
        .commit_size = ALib(arena_default_commit_size),                      \
        .flags = ALib(arena_default_flags),                                  \
        .loc = (ALibCallerLocation),                                        \
        __VA_ARGS__})

ALIB_DEF void ALibProc(arena_release)(ALib(Arena) *arena);

// arena push/pop/pos core functions
ALIB_DEF void      *ALibProc(arena_push)(ALib(Arena) *arena, ALib(u64) size, ALib(u64) align, ALib(b32) zero);
ALIB_DEF ALib(u64)  ALibProc(arena_pos)(ALib(Arena) *arena);
ALIB_DEF void       ALibProc(arena_pop_to)(ALib(Arena) *arena, ALib(u64) pos);

// arena push/pop helpers
ALIB_DEF void ALibProc(arena_clear)(ALib(Arena) *arena);
ALIB_DEF void ALibProc(arena_pop)(ALib(Arena) *arena, ALib(u64) amt);

// temporary arena scopes
ALIB_DEF ALib(Temp) ALibProc(temp_begin)(ALib(Arena) *arena);
ALIB_DEF void       ALibProc(alib_temp_end)(ALib(Temp) temp);

// push helper macros
#define alib_push_array_no_zero_aligned(a, T, c, align) (T *)ALibProc(arena_push)((a), sizeof(T)*(c), (align), (0))
#define alib_push_array_aligned(a, T, c, align) (T *)ALibProc(arena_push)((a), sizeof(T)*(c), (align), (1))
#define alib_push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, ALibMax(8, ALibAlignOf(T)))
#define alib_push_array(a, T, c) push_array_aligned(a, T, c, ALibMax(8, ALibAlignOf(T)))
#endif
