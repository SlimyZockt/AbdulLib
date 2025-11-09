#ifndef ALIB_BASE_CORE_H
#define ALIB_BASE_CORE_H
//NOTE: Foreign Includes
#include <stdint.h>
#include <string.h>

#if ALIB_BASE_CORE_STRIP_PREFIX 
//NOTE: Codebase Keywords
# define internal alib_internal
# define global alib_global
# define local_persist alib_local_persist
# define rodata alib_rodata
//NOTE: Utility Marcos
# define Statement alib_Statement
# define Stringify alib_Stringify
# define Glue alib_Glue
# define ArrayCount alib_ArrayCount
# define Swap alib_Swap
# define IntFromPtr alib_IntFromPtr
# define PtrFromInt alib_PtrFromInt
# define Compose64Bit alib_Compose64Bit
# define Compose32Bit alib_Compose32Bit
# define AlignPow2 alib_AlignPow2
# define AlignDownPow alib_AlignDownPow
# define AlignPadPow2 alib_AlignPadPow2
# define IsPow2 alib_IsPow2
# define IsPow2OrZero alib_IsPow2OrZero
# define ExtractBit alib_ExtractBit
// #define Extract8 alib_Extract8
// #define Extract16 alib_Extract16
// #define Extract32 alib_Extract32
//NOTE: Type -> Alignment
# define AlionOf alib_AlignOf
//NOTE: Units
# define KB alib_KB
# define MB alib_MB
# define GB alib_GB
# define TB alib_TB
# define Thousand alib_Thousand
# define Million alib_Million
# define Billion alib_Billion
//NOTE: Types
# define u8 alib_u8;
# define u16 alib_u16;
# define u32 alib_u32;
# define u64 alib_u64;
# define i8 alib_i8;
# define i16 alib_i16;
# define i32 alib_i32;
# define i64 alib_i64;
# define b8 alib_b8;
# define b16 alib_b16;
# define b32 alib_b32;
# define b64 alib_b64;
# define f32 alib_f32;
# define f64 alib_f64;
# define VoidProc(void) alib_VoidProc(void);
//NOTE: Assert
# define Trap alib_Trap
# define SourceCodeLocation alib_SourceCodeLocation
# define CallerLocation alib_CallerLocation 
# define AssertAlways alib_Ensure
# define Assert alib_Assert
# define InvalidPath alib_InvalidPath
# define NotImplemented alib_NotImplemented
# define NoOp alib_NoOp
# define StaticAssert alib_StaticAssert
//NOTE: Member Offsets
# define alib_Member(T,m)                 (((T*)0)->m)
# define alib_OffsetOf(T,m)               alib_IntFromPtr(&Member(T,m))
# define alib_MemberFromOffset(T,ptr,off) (T)((((U8 *)ptr)+(off)))
# define alib_CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - alib_OffsetOf(T,m))
# define alib_Min(a,b) (((a)<(b)) ? (a):(b))
# define alib_Max(a,b) (((a)>(b)) ? (a):(b))
# define alib_ClampTop(A,X) alib_Min(A,X)
# define alib_ClampBot(X,B) alib_Max(X,B)
# define alib_Clamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))
//NOTE: For-Loop Construct Macros
# define DeferLoop alib_DeferLoop
# define DeferLoopChecked alib_DeferLoopChecked
# define EachIndex alib_EachIndex
# define EachElement alib_EachElement
//NOTE: Memory
# define MemoryCopy alib_MemoryCopy
# define MemorySet alib_MemorySet
# define MemoryCompare alib_MemoryCompare
# define MemoryStrlen alib_MemoryStrlen
  
# define MemoryCopyStruct alib_MemoryCopyStruct
# define MemoryCopyArray alib_MemoryCopyArray
# define MemoryCopyTyped alib_MemoryCopyTyped
# define MemoryCopyStr8 alib_MemoryCopyStr8
  
# define MemoryZero alib_MemoryZero
# define MemoryZeroStruct alib_MemoryZeroStruct
# define MemoryZeroArray alib_MemoryZeroArray
# define MemoryZeroTyped alib_MemoryZeroTyped
  
# define MemoryMatch alib_MemoryMatch
# define MemoryMatchStruct alib_MemoryMatchStruct
# define MemoryMatchArray alib_MemoryMatchArray
  
# define MemoryRead alib_MemoryRead
# define MemoryConsume alib_MemoryConsume
#endif

//NOTE: Codebase Keywords
#define alib_internal static
#define alib_global static
#define alib_local_persist static

#define alib_rodata static const

//NOTE: Utility Marcos
#define alib_Statement(S) do {S} while(0)

#define alib_Stringify_(S) #S
#define alib_Stringify(S) alib_Stringify_(S)
#define alib_Glue_(A,B) A##B
#define alib_Glue(A,B) alib_Glue_(A,B)
#define alib_ArrayCount(a) (sizeof(a)/sizeof(*(a)))
#define alib_Swap(T,a,b) Statement(t__ = a; a = b; b = t__;)

#if ALIB_ARCH_X64
# define alib_IntFromPtr(ptr) ((U64)(ptr))
#elif ALIB_ARCH_X86
# define alib_IntFromPtr(ptr) ((U32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif

#define alib_PtrFromInt(i) (void*)(i)

#define alib_Compose64Bit(a,b)  ((((U64)a) << 32) | ((U64)b))
#define alib_Compose32Bit(a,b)  ((((U32)a) << 16) | ((U32)b))
#define alib_AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define alib_AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define alib_AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define alib_IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define alib_IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define alib_ExtractBit(word, idx) (((word) >> (idx)) & 1)

// #define alib_Extract8(word, pos)   (((word) >> ((pos)*8))  & max_U8)
// #define alib_Extract16(word, pos)  (((word) >> ((pos)*16)) & max_U16)
// #define alib_Extract32(word, pos)  (((word) >> ((pos)*32)) & max_U32)

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
#define alib_KB(n)  (((alib_u64)(n)) << 10)
#define alib_MB(n)  (((alib_u64)(n)) << 20)
#define alib_GB(n)  (((alib_u64)(n)) << 30)
#define alib_TB(n)  (((alib_u64)(n)) << 40)
#define alib_Thousand(n)   ((n)*1000)
#define alib_Million(n)    ((n)*1000000)
#define alib_Billion(n)    ((n)*1000000000)

//NOTE: Types
typedef uint8_t alib_u8;
typedef uint16_t alib_u16;
typedef uint32_t alib_u32;
typedef uint64_t alib_u64;
typedef int8_t alib_i8;
typedef int16_t alib_i16;
typedef int32_t alib_i32;
typedef int64_t alib_i64;
typedef alib_i8 alib_b8;
typedef alib_i16 alib_b16;
typedef alib_i32 alib_b32;
typedef alib_i64 alib_b64;
typedef float alib_f32;
typedef double alib_f64;
typedef void alib_VoidProc(void);

//NOTE: Asserts
#if ALIB_COMPILER_MSVC 
# define Trap() __debugbreak()
#elif ALIB_COMPILER_CLANG || ALIB_COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

typedef struct alib_SourceCodeLocation alib_SourceCodeLocation;
struct alib_SourceCodeLocation {
    char* file;
    int line;
};

#define alib_CallerLocation ((alib_SourceCodeLocation){__FILE__, __LINE__})

#define alib_Ensure(x) alib_Statement(if(!(x)) {alib_Trap();})

#if ALIB_DEBUG_BUILD
# define alib_Assert(x) alib_Ensure(x)
#else
# define alib_Assert(x) (void)(x)
#endif

#define alib_InvalidPath        alib_Assert(!"Invalid Path!")
#define alib_NotImplemented     alib_Assert(!"Not Implemented!")
#define alib_NoOp               ((void)0)
#define alib_StaticAssert(C, ID) global u8 alib_Glue(ID, __LINE__)[(C)?1:-1]

//NOTE: Member Offsets
#define alib_Member(T,m)                 (((T*)0)->m)
#define alib_OffsetOf(T,m)               alib_IntFromPtr(&Member(T,m))
#define alib_MemberFromOffset(T,ptr,off) (T)((((U8 *)ptr)+(off)))
#define alib_CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - alib_OffsetOf(T,m))

#define alib_Min(a,b) (((a)<(b)) ? (a):(b))
#define alib_Max(a,b) (((a)>(b)) ? (a):(b))
#define alib_Clamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))

//NOTE: For-Loop Construct Macros
#define alib_DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define alib_DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))
#define alib_EachIndex(it, count) (U64 it = 0; it < (count); it += 1)
#define alib_EachElement(it, array) (U64 it = 0; it < alib_ArrayCount(array); it += 1)

//NOTE: Memory
#define alib_MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define alib_MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define alib_MemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define alib_MemoryStrlen(ptr)             strlen(ptr)

#define alib_MemoryCopyStruct(d,s)  alib_MemoryCopy((d),(s),sizeof(*(d)))
#define alib_MemoryCopyArray(d,s)   alib_MemoryCopy((d),(s),sizeof(d))
#define alib_MemoryCopyTyped(d,s,c) alib_MemoryCopy((d),(s),sizeof(*(d))*(c))
#define alib_MemoryCopyStr8(dst, s) alib_MemoryCopy(dst, (s).str, (s).size)

#ifdef ALIB_BUILD_DEBUG 
# define alib_MemoryZero(s,z)       memset((s),0xCB,(z))
#else
# define alib_MemoryZero(s,z)       memset((s),0,(z))
#endif

#define alib_MemoryZeroStruct(s)   alib_MemoryZero((s),sizeof(*(s)))
#define alib_MemoryZeroArray(a)    alib_MemoryZero((a),sizeof(a))
#define alib_MemoryZeroTyped(m,c)  alib_MemoryZero((m),sizeof(*(m))*(c))

#define alib_MemoryMatch(a,b,z)     (alib_MemoryCompare((a),(b),(z)) == 0)
#define alib_MemoryMatchStruct(a,b)  alib_MemoryMatch((a),(b),sizeof(*(a)))
#define alib_MemoryMatchArray(a,b)   alib_MemoryMatch((a),(b),sizeof(a))

#define alib_MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define alib_MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

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
