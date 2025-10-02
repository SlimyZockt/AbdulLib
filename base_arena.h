#ifndef ALIB_BASE_ARENA_H
#define ALIB_BASE_ARENA_H

typedef ArenaFlags u64;
enum {
    Arena_Growing (1<<0),
};

global u64 arena_default_reserve_size = MB(64);
global u64 arena_default_commit_size  = KB(64);
global ArenaFlags arena_default_flags = 0;

typedef struct Region {
    Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
} Region;

typedef struct Arena {
    ALIB_ArenaFlags falgs;
    u64 commit_size;
    u64 reserve_size;
    Region *begin;
    Region *end;
} Arena;

#define arena_alloc(arena) = alib_alloc_(&(arena), &SourceLocation)

void *alloc_(Arena *arena,  char *location);

#ifdef ALIB_BASE_ARENA_IMPLEMTATION
void *alloc_(Arena *arena, size_t type_size, SourceCodeLocation *location){

}
#endif


#endif
