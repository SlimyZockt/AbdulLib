//TODO(Adnan): replace with os module
#if ALIB_OS_WINDOWS
#include <windows.h>

alib_u64 _alib_get_pagesize() {
    SYSTEM_INFO info = {0};
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void *_alib_os_reserve(alib_u64 size) {
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

alib_b32 _alib_os_commit(void *ptr, alib_u64 size) {
    B32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

void _alib_os_release(void *ptr, alib_u64 size) {
    // NOTE: size not used - not necessary on Windows, but necessary for other OSes.
    VirtualFree(ptr, 0, MEM_RELEASE);
}

#elif ALIB_OS_LINUX
#include <unistd.h>

alib_u64 _alib_get_pagesize() {
    return getpagesize();
}

void *_alib_os_reserve(alib_u64 size) {
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

alib_b32 _alib_os_commit(void *ptr, alib_u64 size) {
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

void _alib_os_release(void *ptr, alib_u64 size) {
    munmap(ptr, size);
}
#else
    #error alib arena page size not defined;
#endif // OS 

alib_internal alib_Arena *alib_arena_alloc_(alib_ArenaParams *params) {
    //NOTE: round up reserve/commit sizes
    alib_u64 reserve_size = params->reserve_size;
    alib_u64 commit_size = params->commit_size;

    reserve_size = alib_AlignPow2(reserve_size, _alib_get_pagesize());
    commit_size  = alib_AlignPow2(commit_size,  _alib_get_pagesize());

    //NOTE: reserve/commit initial block
    void *base = params->optional_backing_buffer;
    if (base == 0) {
        base = os_reserve(reserve_size);
        _alib_os_commit(base, commit_size);
    }

    //NOTE: panic on arena creation failure
    alib_Ensure(base == 0);

    //NOTE: extract arena header & fill
    alib_Arena *arena = (alib_Arena *)base;
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

alib_internal void alib_arena_release(alib_Arena *arena) {
    for(alib_Arena *n = arena->current, *prev = 0; n != 0; n = prev) {
        prev = n->prev;
        alib_os_release(n, n->res);
    }
}

//Note: arena push/pop core functions
alib_internal void *alib_arena_push(alib_Arena *arena, alib_u64 size, alib_u64 align) {
    alib_Arena *current = arena->current;
    alib_u64 pos_pre = alib_AlignPow2(current->pos, align);
    alib_u64 pos_pst = pos_pre + size;

    //NOTE: chain, if needed
    if(current->res < pos_pst && !(arena->flags & alib_ArenaFlag_NoChain)) {
        alib_Arena *new_block = 0;

        alib_Arena *prev_block;
        for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev) {
            if(new_block->res >= alib_AlignPow2(size, align)) {
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

        if(new_block == 0) {
            alib_u64 res_size = current->res_size;
            alib_u64 cmt_size = current->cmt_size;
            if(size + ARENA_HEADER_SIZE > res_size) {
                res_size = alib_AlignPow2(size + ARENA_HEADER_SIZE, align);
                cmt_size = alib_AlignPow2(size + ARENA_HEADER_SIZE, align);
            }
            new_block = alib_arena_alloc_(.reserve_size = res_size,
                    .commit_size  = cmt_size,
                    .flags        = current->flags,
                    .loc = current->loc);
        }

        new_block->base_pos = current->base_pos + current->res;
        alib_SLLStackPush_N(arena->current, new_block, prev);

        current = new_block;
        pos_pre = alib_AlignPow2(current->pos, align);
        pos_pst = pos_pre + size;
    }

    //NOTE: commit new pages, if needed
    if(current->cmt < pos_pst) {
        alib_u64 cmt_pst_aligned = pos_pst + current->cmt_size-1;
        cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
        alib_u64 cmt_pst_clamped = ClampTop(cmt_pst_aligned, current->res);
        alib_u64 cmt_size = cmt_pst_clamped - current->cmt;
        alib_u8 *cmt_ptr = (alib_u8 *)current + current->cmt;
        _alib_os_commit(cmt_ptr, cmt_size);
        current->cmt = cmt_pst_clamped;
    }

    //NOTE: push onto current block
    void *result = 0;
    if(current->cmt >= pos_pst) {
        result = (alib_u8 *)current+pos_pre;
        current->pos = pos_pst;
    }

    alib_AssertAlways(result == 0 );

    return result;
}

alib_internal alib_u64 alib_arena_pos(alib_Arena *arena) {
    alib_Arena *current = arena->current;
    alib_u64 pos = current->base_pos + current->pos;
    return pos;
}

alib_internal void alib_arena_pop_to(alib_Arena *arena, alib_u64 pos) {
    alib_u64 big_pos = alib_ClampBottom(ARENA_HEADER_SIZE, pos);
    alib_Arena *current = arena->current;

    for(alib_Arena *prev = 0; current->base_pos >= big_pos; current = prev) {
        prev = current->prev;
        current->pos = ARENA_HEADER_SIZE;
        arena->free_size += current->res_size;
        SLLStackPush_N(arena->free_last, current, prev);
    }
    arena->current = current;
    alib_u64 new_pos = big_pos - current->base_pos;
    alib_AssertAlways(new_pos <= current->pos);
    current->pos = new_pos;
}

//Note: arena push/pop helpers
alib_internal void arena_clear(alib_Arena *arena) {
    arena_pop_to(arena, 0);
}

alib_internal void arena_pop(alib_Arena *arena, alib_u64 amt) {
    alib_u64 pos_old = arena_pos(arena);
    alib_u64 pos_new = pos_old;
    if(amt < pos_old) {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

//Note: temporary arena scopes
alib_internal alib_Temp temp_begin(alib_Arena *arena) {
    alib_u64 pos = arena_pos(arena);
    alib_Temp temp = {arena, pos};
    return temp;
}

alib_alib_internal void temp_end(alib_alib_Temp temp) {
    arena_pop_to(temp.arena, temp.pos);
}
