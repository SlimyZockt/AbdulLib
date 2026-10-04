#ifndef ALibBASE_CORE_H
#define ALibBASE_CORE_H
//NOTE: Foreign Includes
#include <stdint.h>
#include <string.h>
#ifndef ALIB_DEF
# define ALIB_DEF  // comment for clion formarter
#endif

//NOTE: Codebase Keywords
#define internal static
#define global static
#define local_persist static

#define rodata static const

//NOTE: Utility Marcos
#define ALibTypedef(type, name) typedef type name
#define ALibStructForward(name) ALibTypedef(struct name,name)
#define ALibStruct(name) ALibStructForward(name); struct name
#define ALibUnionForward(name) ALibTypedef(union name, name)
#define ALibUnion(name) ALibUnionForward(name); union name
#define ALibEnum(name, size) ALibTypedef(size, name); enum name

#define Statement(S) do {S} while(0)

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)
#define ArrayCount(a) (sizeof(a)/sizeof(*(a)))
#define Swap(T,a,b) Statement(t__ = a; a = b; b = t__;)

#if ALIB_ARCH_X64
# define IntFromPtr(ptr) ((U64)(ptr))
#elif ALIB_ARCH_X86
# define IntFromPtr(ptr) ((U32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif

#define PtrFromInt(i) (void*)(i)

#define Compose64Bit(a,b)  ((((U64)a) << 32) | ((U64)b))
#define Compose32Bit(a,b)  ((((U32)a) << 16) | ((U32)b))
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ExtractBit(word, idx) (((word) >> (idx)) & 1)

// #define Extract8(word, pos)   (((word) >> ((pos)*8))  & max_U8)
// #define Extract16(word, pos)  (((word) >> ((pos)*16)) & max_U16)
// #define Extract32(word, pos)  (((word) >> ((pos)*32)) & max_U32)

//NOTE: Type -> Alignment
#if ALIB_COMPILER_MSVC
# define AlignOf(T) __alignof(T)
#elif ALIB_COMPILER_CLANG
# define AlignOf(T) __alignof(T)
#elif ALIB_COMPILER_GCC
# define AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

//NOTE: Units
#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

//NOTE: Types
ALibTypedef(uint8_t, U8);
ALibTypedef(uint16_t, U16);
ALibTypedef(uint32_t, U32);
ALibTypedef(uint64_t, U64);
ALibTypedef(int8_t, I8);
ALibTypedef(int16_t, I16);
ALibTypedef(int32_t, I32);
ALibTypedef(int64_t, I64);
ALibTypedef(I8, B8);
ALibTypedef(I16, B16);
ALibTypedef(I32, B32);
ALibTypedef(I64, B64);
ALibTypedef(float, F32);
ALibTypedef(double, F64);
ALibTypedef(void, VoidProc(void));

//NOTE: Asserts
#if ALIB_COMPILER_MSVC
# define Trap() __debugbreak()
#elif ALIB_COMPILER_CLANG || ALIB_COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

ALibStruct(SourceLocation) {
    const char *file;
    int line;
};

#define CallerLocation ((SourceLocation){__FILE__, __LINE__})

#define Ensure(x) Statement(if(!(x)) {Trap();})

#if ALIB_DEBUG_BUILD
# define Assert(x) Ensure(x)
#else
# define Assert(x) (void)(x)
#endif

#define InvalidPath        Assert(!"Invalid Path!")
#define NotImplemented     Assert(!"Not Implemented!")
#define NoOp               ((void)0)
#define StaticAssert(C, ID) global U8 Glue(ID, __LINE__)[(C)?1:-1]

//NOTE: Member Offsets
#define Member(T,m)                 (((T*)0)->m)
#define OffsetOf(T,m)               IntFromPtr(&Member(T,m))
#define MemberFromOffset(T,ptr,off) (T)((((U8*)ptr)+(off)))
#define CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - OffsetOf(T,m))

#define Min(a,b) (((a)<(b)) ? (a):(b))
#define Max(a,b) (((a)>(b)) ? (a):(b))
#define Clamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))
#define ClampTop(a,b) Min(a, b)
#define ClampBot(a,b) Max(a,b)

//NOTE: For-Loop Construct Macros
#define DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))
#define EachIndex(it, count) (U64 it = 0; it < (count); it += 1)
#define EachElement(it, array) (U64 it = 0; it < ArrayCount(array); it += 1)

//NOTE: Memory
#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define MemoryStrlen(ptr)             strlen(ptr)

#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   MemoryCopy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),sizeof(*(d))*(c))
#define MemoryCopyStr8(dst, s) MemoryCopy(dst, (s).str, (s).size)

#ifdef ALIB_BUILD_DEBUG
# define MemoryZero(s,z)       memset((s),0xCB,(z))
#else
# define MemoryZero(s,z)       memset((s),0,(z))
#endif

#define MemoryZeroStruct(s)   MemoryZero((s),sizeof(*(s)))
#define MemoryZeroArray(a)    MemoryZero((a),sizeof(a))
#define MemoryZeroTyped(m,c)  MemoryZero((m),sizeof(*(m))*(c))

#define MemoryMatch(a,b,z)     (MemoryCompare((a),(b),(z)) == 0)
#define MemoryMatchStruct(a,b)  MemoryMatch((a),(b),sizeof(*(a)))
#define MemoryMatchArray(a,b)   MemoryMatch((a),(b),sizeof(a))

#define MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

// Linked List Building Macros

//NOTE: linked list macro helpers
#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

//NOTE: doubly-linked-lists
#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ?                                     \
        ((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :                           \
        CheckNil(nil,p) ?                                                                           \
        ((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :                      \
        ((p)==(l)) ?                                                                                \
        ((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :                     \
        (((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) :                                     \
        ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushFront_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),   \
        ((n) == (l) ? (l) = (l)->prev : (0)),                                       \
        (CheckNil(nil,(n)->prev) ? (0) :                                            \
        ((n)->prev->next = (n)->next)),                                             \
        (CheckNil(nil,(n)->next) ? (0) :                                            \
        ((n)->next->prev = (n)->prev)))

//NOTE: singly-linked, doubly-headed lists (queues)
#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?   \
        ((f)=(l)=(n),SetNil(nil,(n)->next)):                \
        ((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))
#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?  \
        ((f)=(l)=(n),SetNil(nil,(n)->next)):                    \
        ((n)->next=(f),(f)=(n)))
#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)? \
        (SetNil(nil,f),SetNil(nil,l)):          \
        ((f)=(f)->next))

//NOTE: singly-linked, singly-headed lists (stacks)
#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)

//NOTE: doubly-linked-list helpers
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(0,f,l,n,next,prev)
#define DLLInsert(f,l,p,n) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove(f,l,n) DLLRemove_NPZ(0,f,l,n,next,prev)

//NOTE: singly-linked, doubly-headed list helpers
#define SLLQueuePush_N(f,l,n,next) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront_N(f,l,n,next) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop_N(f,l,next) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop(f,l) SLLQueuePop_NZ(0,f,l,next)

//NOTE: singly-linked, singly-headed list helpers
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)

#endif
