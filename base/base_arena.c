//TODO(Adnan): replace with os module
#if ALIB_OS_WINDOWS
#include <windows.h>

ALib(u64) _alib_get_pagesize() {
    SYSTEM_INFO info = {0};
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void *_alib_os_reserve(ALib(u64) size) {
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

ALib(b32) _alib_os_commit(void *ptr, ALib(u64) size) {
    ALib(b32) result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

void _alib_os_release(void *ptr, ALib(u64) size) {
    // NOTE: size not used - not necessary on Windows, but necessary for other OSes.
    VirtualFree(ptr, 0, MEM_RELEASE);
}

#elif ALIB_OS_LINUX
#include <unistd.h>
#include <sys/mman.h>

ALib(u64) _alib_get_pagesize() {
    return (ALib(u64))sysconf(_SC_PAGESIZE);
}

void *_alib_os_reserve(ALib(u64) size) {
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

ALib(b32) _alib_os_commit(void *ptr, ALib(u64) size) {
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

void _alib_os_release(void *ptr, ALib(u64) size) {
    munmap(ptr, size);
}
#else
    #error alib arena page size not defined;
#endif // OS 

ALIB_DEF ALib(Arena) *ALibProc(arena_alloc_)(ALib(ArenaParams) *params) {
    //NOTE: round up reserve/commit sizes
    ALib(u64) reserve_size = params->reserve_size;
    ALib(u64) commit_size = params->commit_size;

    reserve_size = ALib_AlignPow2(reserve_size, _alib_get_pagesize());
    commit_size  = ALib_AlignPow2(commit_size,  _alib_get_pagesize());

    //NOTE: reserve/commit initial block
    void *base = params->optional_backing_buffer;
    if (base == 0) {
        base = _alib_os_reserve(reserve_size);
        _alib_os_commit(base, commit_size);
    }

    //NOTE: panic on arena creation failure
    ALib_Ensure(base != 0);

    //NOTE: extract arena header & fill
    ALib(Arena) *arena = (ALib(Arena)*)base;
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

ALIB_DEF void ALibProc(arena_release)(ALib(Arena) *arena) {
    for(ALib(Arena) *n = arena->current, *prev = 0; n != 0; n = prev) {
        prev = n->prev;
        _alib_os_release(n, n->res);
    }
}

//Note: arena push/pop core functions
ALIB_DEF void *ALibProc(arena_push)(ALib(Arena) *arena, ALib(u64) size, ALib(u64) align) {
    ALib(Arena) *current = arena->current;
    ALib(u64) pos_pre = ALib_AlignPow2(current->pos, align);
    ALib(u64) pos_pst = pos_pre + size;

    //NOTE: chain, if needed
    if(current->res < pos_pst && !(arena->flags & ALib(ArenaFlag_NoChain))) {
        ALib(Arena) *new_block = 0;

        ALib(Arena) *prev_block;
        for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev) {
            if(new_block->res >= ALib_AlignPow2(size, align)) {
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
            ALib(u64) res_size = current->res_size;
            ALib(u64) cmt_size = current->cmt_size;
            if(size + ALIB_ARENA_HEADER_SIZE > res_size) {
                res_size = ALib_AlignPow2(size + ALIB_ARENA_HEADER_SIZE, align);
                cmt_size = ALib_AlignPow2(size + ALIB_ARENA_HEADER_SIZE, align);
            }
            new_block = alib_arena_alloc(.reserve_size = res_size,
                    .commit_size  = cmt_size,
                    .flags        = current->flags,
                    .loc = current->loc);
        }

        new_block->base_pos = current->base_pos + current->res;
        ALib_SLLStackPush_N(arena->current, new_block, prev);

        current = new_block;
        pos_pre = ALib_AlignPow2(current->pos, align);
        pos_pst = pos_pre + size;
    }

    //NOTE: commit new pages, if needed
    if(current->cmt < pos_pst) {
        ALib(u64) cmt_pst_aligned = pos_pst + current->cmt_size-1;
        cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
        ALib(u64) cmt_pst_clamped = ALib_ClampTop(cmt_pst_aligned, current->res);
        ALib(u64) cmt_size = cmt_pst_clamped - current->cmt;
        ALib(u8) *cmt_ptr = (ALib(u8) *)current + current->cmt;
        _alib_os_commit(cmt_ptr, cmt_size);
        current->cmt = cmt_pst_clamped;
    }

    //NOTE: push onto current block
    void *result = 0;
    if(current->cmt >= pos_pst) {
        result = (ALib(u8)*)current+pos_pre;
        current->pos = pos_pst;
    }

    ALib_Ensure(result != 0 );

    return result;
}

ALIB_DEF ALib(u64) ALibProc(arena_pos)(ALib(Arena) *arena) {
    ALib(Arena) *current = arena->current;
    ALib(u64) pos = current->base_pos + current->pos;
    return pos;
}

ALIB_DEF void ALibProc(arena_pop_to)(ALib(Arena) *arena, ALib(u64) pos) {
    ALib(u64) big_pos = ALib_ClampBot(ALIB_ARENA_HEADER_SIZE, pos);
    ALib(Arena) *current = arena->current;

    for(ALib(Arena) *prev = 0; current->base_pos >= big_pos; current = prev) {
        prev = current->prev;
        current->pos = ALIB_ARENA_HEADER_SIZE;
        arena->free_size += current->res_size;
        ALib_SLLStackPush_N(arena->free_last, current, prev);
    }
    arena->current = current;
    ALib(u64) new_pos = big_pos - current->base_pos;
    ALib_Ensure(new_pos <= current->pos);
    current->pos = new_pos;
}

//Note: arena push/pop helpers
ALIB_DEF void ALibProc(alib_arena_clear)(ALib(Arena) *arena) {
    arena_pop_to(arena, 0);
}

ALIB_DEF void ALibProc(arena_pop)(ALib(Arena) *arena, ALib(u64) amt) {
    ALib(u64) pos_old = ALibProc(arena_pos)(arena);
    ALib(u64) pos_new = pos_old;
    if(amt < pos_old) {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

//Note: temporary arena scopes
ALIB_DEF ALib(Temp) ALibProc(temp_begin)(ALib(Arena) *arena) {
    ALib(u64) pos = ALibProc(arena_pos)(arena);
    ALib(Temp) temp = {arena, pos};
    return temp;
}

ALIB_DEF void ALibProc(temp_end)(ALib(Temp) temp) {
    arena_pop_to(temp.arena, temp.pos);
}
