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

ALibU64 _alib_get_pagesize() {
    return (ALibU64)sysconf(_SC_PAGESIZE);
}

void *_alib_os_reserve(ALibU64 size) {
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

ALibB32 _alib_os_commit(void *ptr, ALibU64 size) {
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

void _alib_os_release(void *ptr, ALibU64 size) {
    munmap(ptr, size);
}
#else
    #error alib arena page size not defined;
#endif // OS 

ALIB_DEF ALibArena *alib_arena_alloc_(ALibArenaParams *params) {
    //NOTE: round up reserve/commit sizes
    ALibU64 reserve_size = params->reserve_size;
    ALibU64 commit_size = params->commit_size;

    reserve_size = ALibAlignPow2(reserve_size, _alib_get_pagesize());
    commit_size  = ALibAlignPow2(commit_size,  _alib_get_pagesize());

    //NOTE: reserve/commit initial block
    void *base = params->optional_backing_buffer;
    if (base == 0) {
        base = _alib_os_reserve(reserve_size);
        _alib_os_commit(base, commit_size);
    }

    //NOTE: panic on arena creation failure
    ALibEnsure(base != 0);

    //NOTE: extract arena header & fill
    ALibArena *arena = (ALibArena*)base;
    arena->current = arena;
    arena->flags = params->flags;
    arena->cmt_size = params->commit_size;
    arena->res_size = params->reserve_size;
    arena->base_pos = 0;
    arena->pos = ALIB_ARENA_HEADER_SIZE;
    arena->cmt = commit_size;
    arena->res = reserve_size;
    arena->loc = params->loc;
    arena->free_size = 0;
    arena->free_last = 0;
    return arena;
}

ALIB_DEF void alib_arena_release(ALibArena *arena) {
    for(ALibArena *n = arena->current, *prev = 0; n != 0; n = prev) {
        prev = n->prev;
        _alib_os_release(n, n->res);
    }
}

//NOTE: arena push/pop core functions
ALIB_DEF void *alib_arena_push(ALibArena *arena, ALibU64 size, ALibU64 align, ALibB32 zero) {
    ALibArena *current = arena->current;
    ALibU64 pos_pre = ALibAlignPow2(current->pos, align);
    ALibU64 pos_pst = pos_pre + size;

    //NOTE: chain, if needed
    if(current->res < pos_pst && !(arena->flags & ALibArenaFlag_NoChain)) {
        ALibArena *new_block = 0;

        {// use free list
            ALibArena *prev_block;
            for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev) {
                if(new_block->res >= ALibAlignPow2(size, align)) {
                    if(prev_block) {
                        prev_block->prev = new_block->prev;
                    }
                    else {
                        arena->free_last = new_block->prev;
                    }
                    arena->free_size -= new_block->res_size;
                    break;
                }
            }
        }

        if(new_block == 0) {
            ALibU64 res_size = current->res_size;
            ALibU64 cmt_size = current->cmt_size;
            if(size + ALIB_ARENA_HEADER_SIZE > res_size) {
                res_size = ALibAlignPow2(size + ALIB_ARENA_HEADER_SIZE, align);
                cmt_size = ALibAlignPow2(size + ALIB_ARENA_HEADER_SIZE, align);
            }
            new_block = alib_arena_alloc(.reserve_size = res_size,
                                         .commit_size  = cmt_size,
                                         .flags        = current->flags,
                                         .loc = current->loc);
        }

        new_block->base_pos = current->base_pos + current->res;
        ALibSLLStackPush_N(arena->current, new_block, prev);

        current = new_block;
        pos_pre = ALibAlignPow2(current->pos, align);
        pos_pst = pos_pre + size;
    }

    ALibU64 size_to_zero = 0;
    if(zero) {
        size_to_zero = Min(current->cmt, pos_pst) - pos_pre;
    }

    //NOTE: commit new pages, if needed
    if(current->cmt < pos_pst) {
        ALibU64 cmt_pst_aligned = pos_pst + current->cmt_size-1;
        cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
        ALibU64 cmt_pst_clamped = ALibClampTop(cmt_pst_aligned, current->res);
        ALibU64 cmt_size = cmt_pst_clamped - current->cmt;
        ALibU8 *cmt_ptr = (ALibU8 *)current + current->cmt;
        _alib_os_commit(cmt_ptr, cmt_size);
        current->cmt = cmt_pst_clamped;
    }

    //NOTE: push onto current block
    void *result = 0;
    if(current->cmt >= pos_pst) {
        result = (ALibU8*)current+pos_pre;
        current->pos = pos_pst;
        if (size_to_zero != 0) {
            ALibMemoryZero(result, size_to_zero);
        }
    }

    ALibEnsure(result != 0 );

    return result;
}

ALIB_DEF ALibU64 alib_arena_pos(ALibArena *arena) {
    ALibArena *current = arena->current;
    ALibU64 pos = current->base_pos + current->pos;
    return pos;
}

ALIB_DEF void alib_arena_pop_to(ALibArena *arena, ALibU64 pos) {
    ALibU64 big_pos = ALibClampBot(ALIB_ARENA_HEADER_SIZE, pos);
    ALibArena *current = arena->current;

    for(ALibArena *prev = 0; current->base_pos >= big_pos; current = prev) {
        prev = current->prev;
        current->pos = ALIB_ARENA_HEADER_SIZE;
        arena->free_size += current->res_size;
        ALibSLLStackPush_N(arena->free_last, current, prev);
    }
    arena->current = current;
    ALibU64 new_pos = big_pos - current->base_pos;
    ALibEnsure(new_pos <= current->pos);
    current->pos = new_pos;
}

//Note: arena push/pop helpers
ALIB_DEF void alib_arena_clear(ALibArena *arena) {
    alib_arena_pop_to(arena, 0);
}

ALIB_DEF void alib_arena_pop(ALibArena *arena, ALibU64 amt) {
    ALibU64 pos_old = alib_arena_pos(arena);
    ALibU64 pos_new = pos_old;
    if(amt < pos_old) {
        pos_new = pos_old - amt;
    }
    alib_arena_pop_to(arena, pos_new);
}

//Note: temporary arena scopes
ALIB_DEF ALibTemp alib_temp_begin(ALibArena *arena) {
    ALibU64 pos = alib_arena_pos(arena);
    ALibTemp temp = {arena, pos};
    return temp;
}

ALIB_DEF void alib_temp_end(ALibTemp temp) {
    alib_arena_pop_to(temp.arena, temp.pos);
}
