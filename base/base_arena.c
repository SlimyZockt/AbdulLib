//TODO(Adnan): replace with os module
#if ALIB_WIN
#include <windows.h>

alib_u64 _alib_get_pagesize() {
    SYSTEM_INFO info = {0};
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void *_alib_os_reserve(size_t size) {

}

void alib_os_commit(void *dest, size_t size) {

}


#elif ALIB_LINUX
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

alib_b32 alib_os_commit(void *dest, size_t size) {
  mprotect(ptr, size, PROT_READ|PROT_WRITE);
  return 1;
}

#else
alib_StaticAssert(0, alib_arena_page_size_not_found);
#endif

alib_internal Alib_Arena *arena_alloc_(Alib_ArenaParams *params) {
    // Note: round up reserve/commit sizes
    alib_u64 reserve_size = params->reserve_size;
    alib_u64 commit_size = params->commit_size;

    reserve_size = alib_AlignPow2(reserve_size, _alib_get_pagesize());
    commit_size  = alib_AlignPow2(commit_size,  _alib_get_pagesize());

    // Note: reserve/commit initial block
    void *base = params->optional_backing_buffer;
    if(base == 0){
        base = os_reserve(reserve_size);
        os_commit(base, commit_size);
        // raddbg_annotate_vaddr_range(base, reserve_size, "arena %s:%i", params->allocation_site_file, params->allocation_site_line);
    }

    // Note: panic on arena creation failure
    alib_AssertAlways(base == 0);

    // Note: extract arena header & fill
    Alib_Arena *arena = (Alib_Arena *)base;
    arena->current = arena;
    arena->flags = params->flags;
    arena->cmt_size = params->commit_size;
    arena->res_size = params->reserve_size;
    arena->base_pos = 0;
    arena->pos = ARENA_HEADER_SIZE;
    arena->cmt = commit_size;
    arena->res = reserve_size;
    arena->allocation_site_file = params->allocation_site_file;
    arena->allocation_site_line = params->allocation_site_line;
    arena->free_size = 0;
    arena->free_last = 0;
    // AsanPoisonMemoryRegion(base, commit_size);
    // AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
    return arena;
}

alib_internal void arena_release(Alib_Arena *arena) {
    for(Alib_Arena *n = arena->current, *prev = 0; n != 0; n = prev) {
        prev = n->prev;
        os_release(n, n->res);
    }
}

//Note: arena push/pop core functions
alib_internal void *arena_push(Alib_Arena *arena, alib_u64 size, alib_u64 align) {
    Alib_Arena *current = arena->current;
    alib_u64 pos_pre = alib_AlignPow2(current->pos, align);
    alib_u64 pos_pst = pos_pre + size;

    // Note: chain, if needed
    if(current->res < pos_pst && !(arena->flags & Alib_ArenaFlag_NoChain)) {
        Alib_Arena *new_block = 0;

        Alib_Arena *prev_block;
        for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev) {
            if(new_block->res >= alib_AlignPow2(size, align)) {
                if(prev_block) {
                    prev_block->prev = new_block->prev;
                }
                else {
                    arena->free_last = new_block->prev;
                }
                arena->free_size -= new_block->res_size;
                AsanUnpoisonMemoryRegion((alib_u8*)new_block + ARENA_HEADER_SIZE, new_block->res_size - ARENA_HEADER_SIZE);
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
            new_block = arena_alloc(.reserve_size = res_size,
                    .commit_size  = cmt_size,
                    .flags        = current->flags,
                    .allocation_site_file = current->allocation_site_file,
                    .allocation_site_line = current->allocation_site_line);
        }

        new_block->base_pos = current->base_pos + current->res;
        //TODO(adnan): What is this?
        // SLLStackPush_N(arena->current, new_block, prev);


        current = new_block;
        pos_pre = alib_AlignPow2(current->pos, align);
        pos_pst = pos_pre + size;
    }

    // Note: commit new pages, if needed
    if(current->cmt < pos_pst) {
        alib_u64 cmt_pst_aligned = pos_pst + current->cmt_size-1;
        cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
        alib_u64 cmt_pst_clamped = ClampTop(cmt_pst_aligned, current->res);
        alib_u64 cmt_size = cmt_pst_clamped - current->cmt;
        alib_u8 *cmt_ptr = (alib_u8 *)current + current->cmt;
        os_commit(cmt_ptr, cmt_size);
        current->cmt = cmt_pst_clamped;
    }

    // Note: push onto current block
    void *result = 0;
    if(current->cmt >= pos_pst) {
        result = (alib_u8 *)current+pos_pre;
        current->pos = pos_pst;
        // AsanUnpoisonMemoryRegion(result, size);
    }

    alib_AssertAlways(result == 0 );

    return result;
}

alib_internal alib_u64 arena_pos(Alib_Arena *arena) {
    Alib_Arena *current = arena->current;
    alib_u64 pos = current->base_pos + current->pos;
    return pos;
}

alib_internal void arena_pop_to(Alib_Arena *arena, alib_u64 pos) {
    alib_u64 big_pos = ClampBot(ARENA_HEADER_SIZE, pos);
    Alib_Arena *current = arena->current;

    for(Alib_Arena *prev = 0; current->base_pos >= big_pos; current = prev) {
        prev = current->prev;
        current->pos = ARENA_HEADER_SIZE;
        arena->free_size += current->res_size;
        // SLLStackPush_N(arena->free_last, current, prev);
        // #define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))

        current->prev=arena->free_last;
        arena->free_last=current
        // AsanPoisonMemoryRegion((alib_u8*)current + ARENA_HEADER_SIZE, current->res_size - ARENA_HEADER_SIZE);
    }
    arena->current = current;
    alib_u64 new_pos = big_pos - current->base_pos;
    alib_AssertAlways(new_pos <= current->pos);
    // AsanPoisonMemoryRegion((alib_u8*)current + new_pos, (current->pos - new_pos));
    current->pos = new_pos;
}

//Note: arena push/pop helpers
alib_internal void arena_clear(Alib_Arena *arena) {
    arena_pop_to(arena, 0);
}

alib_internal void arena_pop(Alib_Arena *arena, alib_u64 amt) {
    alib_u64 pos_old = arena_pos(arena);
    alib_u64 pos_new = pos_old;
    if(amt < pos_old) {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

//Note: temporary arena scopes
alib_internal Alib_Temp temp_begin(Alib_Arena *arena) {
    alib_u64 pos = arena_pos(arena);
    Alib_Temp temp = {arena, pos};
    return temp;
}

alib_alib_internal void temp_end(Alib_Alib_Temp temp) {
    arena_pop_to(temp.arena, temp.pos);
}
