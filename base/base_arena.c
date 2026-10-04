//TODO(Adnan): replace with os module
#if ALIB_OS_WINDOWS
#include <windows.h>

ALibU64 _alib_get_pagesize() {
    SYSTEM_INFO info = {0};
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void *_alib_os_reserve(ALibU64 size) {
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

ALibB32 _alib_os_commit(void *ptr, ALibU64 size) {
    ALibB32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

void _alib_os_release(void *ptr, ALibU64 size) {
    // NOTE: size not used - not necessary on Windows, but necessary for other OSes.
    VirtualFree(ptr, 0, MEM_RELEASE);
}

#elif ALIB_OS_LINUX
#include <unistd.h>
#include <sys/mman.h>

U64 _get_pagesize() {
    return (U64) sysconf(_SC_PAGESIZE);
}

void *_os_reserve(U64 size) {
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

B32 _os_commit(void *ptr, U64 size) {
    mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return 1;
}

void _os_release(void *ptr, U64 size) {
    munmap(ptr, size);
}
#else
#error alib arena page size not defined;
#endif // OS 

ALIB_DEF Arena *arena_alloc_(ArenaParams *params) {
    //NOTE: round up reserve/commit sizes
    U64 reserve_size = params->reserve_size;
    U64 commit_size = params->commit_size;

    reserve_size = AlignPow2(reserve_size, _get_pagesize());
    commit_size = AlignPow2(commit_size, _get_pagesize());

    //NOTE: reserve/commit initial block
    void *base = params->optional_backing_buffer;
    if (base == 0) {
        base = _os_reserve(reserve_size);
        _os_commit(base, commit_size);
    }

    //NOTE: panic on arena creation failure
    Ensure(base != 0);

    //NOTE: extract arena header & fill
    Arena *arena = (Arena *) base;
    arena->current = arena;
    arena->flags = params->flags;
    arena->cmt_size = params->commit_size;
    arena->res_size = params->reserve_size;
    arena->base_pos = 0;
    arena->pos = ARENA_HEADER_SIZE;
    arena->cmt = commit_size;
    arena->res = reserve_size;
    arena->loc = params->loc;
    arena->free_size = 0;
    arena->free_last = 0;
    return arena;
}

ALIB_DEF void arena_release(Arena *arena) {
    for (Arena *n = arena->current, *prev = 0; n != 0; n = prev) {
        prev = n->prev;
        _os_release(n, n->res);
    }
}

//NOTE: arena push/pop core functions
ALIB_DEF void *arena_push(Arena *arena, U64 size, U64 align, B32 zero) {
    Arena *current = arena->current;
    U64 pos_pre = AlignPow2(current->pos, align);
    U64 pos_pst = pos_pre + size;

    //NOTE: chain, if needed
    if (current->res < pos_pst && !(arena->flags & ArenaFlag_NoChain)) {
        Arena *new_block = 0;

        {
            // use free list
            Arena *prev_block;
            for (new_block = arena->free_last, prev_block = 0; new_block != 0;
                 prev_block = new_block, new_block = new_block->prev) {
                if (new_block->res >= AlignPow2(size, align)) {
                    if (prev_block) {
                        prev_block->prev = new_block->prev;
                    } else {
                        arena->free_last = new_block->prev;
                    }
                    arena->free_size -= new_block->res_size;
                    break;
                }
            }
        }

        if (new_block == 0) {
            U64 res_size = current->res_size;
            U64 cmt_size = current->cmt_size;
            if (size + ARENA_HEADER_SIZE > res_size) {
                res_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
                cmt_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
            }
            new_block = arena_alloc(.reserve_size = res_size,
                                    .commit_size = cmt_size,
                                    .flags = current->flags,
                                    .loc = current->loc);
        }

        new_block->base_pos = current->base_pos + current->res;
        SLLStackPush_N(arena->current, new_block, prev);

        current = new_block;
        pos_pre = AlignPow2(current->pos, align);
        pos_pst = pos_pre + size;
    }

    U64 size_to_zero = 0;
    if (zero) {
        size_to_zero = Min(current->cmt, pos_pst) - pos_pre;
    }

    //NOTE: commit new pages, if needed
    if (current->cmt < pos_pst) {
        U64 cmt_pst_aligned = pos_pst + current->cmt_size - 1;
        cmt_pst_aligned -= cmt_pst_aligned % current->cmt_size;
        U64 cmt_pst_clamped = ClampTop(cmt_pst_aligned, current->res);
        U64 cmt_size = cmt_pst_clamped - current->cmt;
        U8 *cmt_ptr = (U8 *) current + current->cmt;
        _os_commit(cmt_ptr, cmt_size);
        current->cmt = cmt_pst_clamped;
    }

    //NOTE: push onto current block
    void *result = 0;
    if (current->cmt >= pos_pst) {
        result = (U8 *) current + pos_pre;
        current->pos = pos_pst;
        if (size_to_zero != 0) {
            MemoryZero(result, size_to_zero);
        }
    }

    Ensure(result != 0);

    return result;
}

ALIB_DEF U64 arena_pos(Arena *arena) {
    Arena *current = arena->current;
    U64 pos = current->base_pos + current->pos;
    return pos;
}

ALIB_DEF void arena_pop_to(Arena *arena, U64 pos) {
    U64 big_pos = ClampBot(ARENA_HEADER_SIZE, pos);
    Arena *current = arena->current;

    for (Arena *prev = 0; current->base_pos >= big_pos; current = prev) {
        prev = current->prev;
        current->pos = ARENA_HEADER_SIZE;
        arena->free_size += current->res_size;
        SLLStackPush_N(arena->free_last, current, prev);
    }
    arena->current = current;
    U64 new_pos = big_pos - current->base_pos;
    Ensure(new_pos <= current->pos);
    current->pos = new_pos;
}

//Note: arena push/pop helpers
ALIB_DEF void arena_clear(Arena *arena) {
    arena_pop_to(arena, 0);
}

ALIB_DEF void alib_arena_pop(Arena *arena, U64 amt) {
    U64 pos_old = arena_pos(arena);
    U64 pos_new = pos_old;
    if (amt < pos_old) {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

//Note: temporary arena scopes
ALIB_DEF Temp temp_begin(Arena *arena) {
    U64 pos = arena_pos(arena);
    Temp temp = {arena, pos};
    return temp;
}

ALIB_DEF void temp_end(Temp temp) {
    arena_pop_to(temp.arena, temp.pos);
}
