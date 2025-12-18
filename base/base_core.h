#ifndef ALib_BASE_CORE_H
#define ALib_BASE_CORE_H
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
# define Statement ALib_Statement
# define Stringify ALib_Stringify
# define Glue ALib_Glue
# define ArrayCount ALib_ArrayCount
# define Swap ALib_Swap
# define IntFromPtr ALib_IntFromPtr
# define PtrFromInt ALib_PtrFromInt
# define Compose64Bit ALib_Compose64Bit
# define Compose32Bit ALib_Compose32Bit
# define AlignPow2 ALib_AlignPow2
# define AlignDownPow ALib_AlignDownPow
# define AlignPadPow2 ALib_AlignPadPow2
# define IsPow2 ALib_IsPow2
# define IsPow2OrZero ALib_IsPow2OrZero
# define ExtractBit ALib_ExtractBit
// #define Extract8 ALib_Extract8
// #define Extract16 ALib_Extract16
// #define Extract32 ALib_Extract32
//NOTE: Type -> Alignment
# define AlionOf ALib_AlignOf
//NOTE: Units
# define KB ALib_KB
# define MB ALib_MB
# define GB ALib_GB
# define TB ALib_TB
# define Thousand ALib_Thousand
# define Million ALib_Million
# define Billion ALib_Billion
//NOTE: Assert
# define Trap ALib_Trap
# define SourceCodeLocation ALib_SourceCodeLocation
# define CallerLocation ALib_CallerLocation 
# define AssertAlways ALib_Ensure
# define Assert ALib_Assert
# define InvalidPath ALib_InvalidPath
# define NotImplemented ALib_NotImplemented
# define NoOp ALib_NoOp
# define StaticAssert ALib_StaticAssert
//NOTE: Member Offsets
# define Member ALib_Member
# define OffsetOf ALib_OffsetOf
# define MemberFromOffset ALib_MemberFromOffset
# define CastFromMember ALib_CastFromMember
# define Min ALib_Min
# define Max ALib_Max
# define ClampTop ALib_ClampTop
# define ClampBot ALib_ClampBot
# define Clamp ALib_Clamp
//NOTE: For-Loop Construct Macros
# define DeferLoop ALib_DeferLoop
# define DeferLoopChecked ALib_DeferLoopChecked
# define EachIndex ALib_EachIndex
# define EachElement ALib_EachElement
//NOTE: Memory
# define MemoryCopy ALib_MemoryCopy
# define MemorySet ALib_MemorySet
# define MemoryCompare ALib_MemoryCompare
# define MemoryStrlen ALib_MemoryStrlen
  
# define MemoryCopyStruct ALib_MemoryCopyStruct
# define MemoryCopyArray ALib_MemoryCopyArray
# define MemoryCopyTyped ALib_MemoryCopyTyped
# define MemoryCopyStr8 ALib_MemoryCopyStr8
  
# define MemoryZero ALib_MemoryZero
# define MemoryZeroStruct ALib_MemoryZeroStruct
# define MemoryZeroArray ALib_MemoryZeroArray
# define MemoryZeroTyped ALib_MemoryZeroTyped
  
# define MemoryMatch ALib_MemoryMatch
# define MemoryMatchStruct ALib_MemoryMatchStruct
# define MemoryMatchArray ALib_MemoryMatchArray
  
# define MemoryRead ALib_MemoryRead
# define MemoryConsume ALib_MemoryConsume

// Linked List Building Macros

//NOTE: linked list macro helpers
# define CheckNil ALib_CheckNil
# define SetNil ALib_SetNil
  
//NOTE: doubly-linked-lists
# define DLLInsert_NPZ ALib_DLLInsert_NPZ
# define DLLPushBack_NPZ ALib_DLLPushBack_NPZ
# define DLLPushFront_NPZ ALib_DLLPushFront_NPZ
# define DLLRemove_NPZ ALib_DLLRemove_NPZ
  
//NOTE: singly-linked, doubly-headed lists (queues)
# define SLLQueuePush_NZ ALib_SLLQueuePush_NZ
# define SLLQueuePushFront_NZ ALib_SLLQueuePushFront_NZ
# define SLLQueuePop_NZ ALib_SLLQueuePop_NZ
  
//NOTE: singly-linked, singly-headed lists (stacks)
# define SLLStackPush_N ALib_SLLStackPush_N
# define SLLStackPop_N ALib_SLLStackPop_N
  
//NOTE: doubly-linked-list helpers
# define DLLInsert_NP ALib_DLLInsert_NP
# define DLLPushBack_NP ALib_DLLPushBack_NP
# define DLLPushFront_NP ALib_DLLPushFront_NP
# define DLLRemove_NP ALib_DLLRemove_NP
# define DLLInsert    ALib_DLLInsert
# define DLLPushBack  ALib_DLLPushBack
# define DLLPushFront ALib_DLLPushFront
# define DLLRemove    ALib_DLLRemove
  
//NOTE: singly-linked, doubly-headed list helpers
# define SLLQueuePush_N      ALib_SLLQueuePush_N
# define SLLQueuePushFront_N ALib_SLLQueuePushFront_N
# define SLLQueuePop_N       ALib_SLLQueuePop_N
# define SLLQueuePush        ALib_SLLQueuePush
# define SLLQueuePushFront   ALib_SLLQueuePushFront
# define SLLQueuePop         ALib_SLLQueuePop
  
//NOTE: singly-linked, singly-headed list helpers
# define SLLStackPush ALib_SLLStackPush
# define SLLStackPop  ALib_SLLStackPop

# define ALib(name) name
# define ALibProc(name) name
# define ALibConst(name) name

#else

# define ALib(name) ALib##name
# define ALibProc(name) alib_##name
# define ALibConst(name) ALIB##name

#endif

#ifndef ALIB_DEF
# define ALIB_DEF
#endif

//NOTE: Codebase Keywords
#define alib_internal static
#define alib_global static
#define alib_local_persist static

#define alib_rodata static const

//NOTE: Utility Marcos
#define ALibTypedef(type, name) typedef type name
#define ALibStructForward(name) ALibTypedef(struct ALib(name), ALib(name))
#define ALibStruct(name) ALibStructForward(name); struct ALib(name)
#define ALibUnionForward(name) ALibTypedef(union ALib(name), ALib(name))
#define ALibUnion(name) ALibUnionForward(name); union ALib(name)
#define ALibEnum(name, size) ALibTypedef(size, ALib(name)); enum ALib(name)

#define ALib_Statement(S) do {S} while(0)

#define ALib_Stringify_(S) #S
#define ALib_Stringify(S) ALib_Stringify_(S)
#define ALib_Glue_(A,B) A##B
#define ALib_Glue(A,B) ALib_Glue_(A,B)
#define ALib_ArrayCount(a) (sizeof(a)/sizeof(*(a)))
#define ALib_Swap(T,a,b) Statement(t__ = a; a = b; b = t__;)

#if ALIB_ARCH_X64
# define ALib_IntFromPtr(ptr) ((ALib(u64))(ptr))
#elif ALIB_ARCH_X86
# define ALib_IntFromPtr(ptr) ((ALib(u32))(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif

#define ALib_PtrFromInt(i) (void*)(i)

#define ALib_Compose64Bit(a,b)  ((((ALib(u64))a) << 32) | ((ALib(u64))b))
#define ALib_Compose32Bit(a,b)  ((((ALib(u32))a) << 16) | ((ALib(u32))b))
#define ALib_AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define ALib_AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define ALib_AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define ALib_IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define ALib_IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ALib_ExtractBit(word, idx) (((word) >> (idx)) & 1)

// #define ALib_Extract8(word, pos)   (((word) >> ((pos)*8))  & max_U8)
// #define ALib_Extract16(word, pos)  (((word) >> ((pos)*16)) & max_U16)
// #define ALib_Extract32(word, pos)  (((word) >> ((pos)*32)) & max_U32)

//NOTE: Type -> Alignment
#if ALIB_COMPILER_MSVC
# define ALib_AlignOf(T) __alignof(T)
#elif ALIB_COMPILER_CLANG
# define ALib_AlignOf(T) __alignof(T)
#elif ALIB_COMPILER_GCC
# define ALib_AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

//NOTE: Units
#define ALib_KB(n)  (((ALib(u64))(n)) << 10)
#define ALib_MB(n)  (((ALib(u64))(n)) << 20)
#define ALib_GB(n)  (((ALib(u64))(n)) << 30)
#define ALib_TB(n)  (((ALib(u64))(n)) << 40)
#define ALib_Thousand(n)   ((n)*1000)
#define ALib_Million(n)    ((n)*1000000)
#define ALib_Billion(n)    ((n)*1000000000)

//NOTE: Types
ALibTypedef(uint8_t, ALib(u8));
ALibTypedef(uint16_t, ALib(u16));
ALibTypedef(uint32_t, ALib(u32));
ALibTypedef(uint64_t, ALib(u64));
ALibTypedef(int8_t, ALib(i8));
ALibTypedef(int16_t, ALib(i16));
ALibTypedef(int32_t, ALib(i32));
ALibTypedef(int64_t, ALib(i64));
ALibTypedef(ALib(i8), ALib(b8));
ALibTypedef(ALib(i16), ALib(b16));
ALibTypedef(ALib(i32), ALib(b32));
ALibTypedef(ALib(i64), ALib(b64));
ALibTypedef(float, ALib(f32));
ALibTypedef(double, ALib(f64));
ALibTypedef(void, ALib(VoidProc(void)));

//NOTE: Asserts
#if ALIB_COMPILER_MSVC 
# define ALib_Trap() __debugbreak()
#elif ALIB_COMPILER_CLANG || ALIB_COMPILER_GCC
# define ALib_Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

ALibStruct(SourceLocation) {
    char* file;
    int line;
};

#define ALib_CallerLocation ((ALib(SourceLocation)){__FILE__, __LINE__})

#define ALib_Ensure(x) ALib_Statement(if(!(x)) {ALib_Trap();})

#if ALIB_DEBUG_BUILD
# define ALib_Assert(x) ALib_Ensure(x)
#else
# define ALib_Assert(x) (void)(x)
#endif

#define ALib_InvalidPath        ALib_Assert(!"Invalid Path!")
#define ALib_NotImplemented     ALib_Assert(!"Not Implemented!")
#define ALib_NoOp               ((void)0)
#define ALib_StaticAssert(C, ID) alib_global ALib(u8) ALib_Glue(ID, __LINE__)[(C)?1:-1]

//NOTE: Member Offsets
#define ALib_Member(T,m)                 (((T*)0)->m)
#define ALib_OffsetOf(T,m)               ALib_IntFromPtr(&Member(T,m))
#define ALib_MemberFromOffset(T,ptr,off) (T)((((ALib(u8)*)ptr)+(off)))
#define ALib_CastFromMember(T,m,ptr)     (T*)(((ALib(u8)*)ptr) - ALib_OffsetOf(T,m))

#define ALib_Min(a,b) (((a)<(b)) ? (a):(b))
#define ALib_Max(a,b) (((a)>(b)) ? (a):(b))
#define ALib_Clamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))
#define ALib_ClampTop(a,b) ALib_Min(a, b)
#define ALib_ClampBot(a,b) ALib_Max(a,b)

//NOTE: For-Loop Construct Macros
#define ALib_DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define ALib_DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))
#define ALib_EachIndex(it, count) (ALib(u64) it = 0; it < (count); it += 1)
#define ALib_EachElement(it, array) (ALib(u64) it = 0; it < ALib_ArrayCount(array); it += 1)

//NOTE: Memory
#define ALib_MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define ALib_MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define ALib_MemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define ALib_MemoryStrlen(ptr)             strlen(ptr)

#define ALib_MemoryCopyStruct(d,s)  ALib_MemoryCopy((d),(s),sizeof(*(d)))
#define ALib_MemoryCopyArray(d,s)   ALib_MemoryCopy((d),(s),sizeof(d))
#define ALib_MemoryCopyTyped(d,s,c) ALib_MemoryCopy((d),(s),sizeof(*(d))*(c))
#define ALib_MemoryCopyStr8(dst, s) ALib_MemoryCopy(dst, (s).str, (s).size)

#ifdef ALIB_BUILD_DEBUG 
# define ALib_MemoryZero(s,z)       memset((s),0xCB,(z))
#else
# define ALib_MemoryZero(s,z)       memset((s),0,(z))
#endif

#define ALib_MemoryZeroStruct(s)   ALib_MemoryZero((s),sizeof(*(s)))
#define ALib_MemoryZeroArray(a)    ALib_MemoryZero((a),sizeof(a))
#define ALib_MemoryZeroTyped(m,c)  ALib_MemoryZero((m),sizeof(*(m))*(c))

#define ALib_MemoryMatch(a,b,z)     (ALib_MemoryCompare((a),(b),(z)) == 0)
#define ALib_MemoryMatchStruct(a,b)  ALib_MemoryMatch((a),(b),sizeof(*(a)))
#define ALib_MemoryMatchArray(a,b)   ALib_MemoryMatch((a),(b),sizeof(a))

#define ALib_MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define ALib_MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

// Linked List Building Macros

//NOTE: linked list macro helpers
#define ALib_CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define ALib_SetNil(nil,p) ((p) = nil)

//NOTE: doubly-linked-lists
#define ALib_DLLInsert_NPZ(nil,f,l,p,n,next,prev) (ALib_CheckNil(nil,f) ?                                     \
        ((f) = (l) = (n), ALib_SetNil(nil,(n)->next), ALib_SetNil(nil,(n)->prev)) :                           \
        ALib_CheckNil(nil,p) ?                                                                           \
        ((n)->next = (f), (f)->prev = (n), (f) = (n), ALib_SetNil(nil,(n)->prev)) :                      \
        ((p)==(l)) ?                                                                                \
        ((l)->next = (n), (n)->prev = (l), (l) = (n), ALib_SetNil(nil, (n)->next)) :                     \
        (((!ALib_CheckNil(nil,p) && ALib_CheckNil(nil,(p)->next)) ? (0) :                                     \
        ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define ALib_DLLPushBack_NPZ(nil,f,l,n,next,prev) ALib_DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define ALib_DLLPushFront_NPZ(nil,f,l,n,next,prev) ALib_DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define ALib_DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),   \
        ((n) == (l) ? (l) = (l)->prev : (0)),                                       \
        (ALib_CheckNil(nil,(n)->prev) ? (0) :                                            \
        ((n)->prev->next = (n)->next)),                                             \
        (ALib_CheckNil(nil,(n)->next) ? (0) :                                            \
        ((n)->next->prev = (n)->prev)))

//NOTE: singly-linked, doubly-headed lists (queues)
#define ALib_SLLQueuePush_NZ(nil,f,l,n,next) (ALib_CheckNil(nil,f)?   \
        ((f)=(l)=(n),ALib_SetNil(nil,(n)->next)):                \
        ((l)->next=(n),(l)=(n),ALib_SetNil(nil,(n)->next)))
#define ALib_SLLQueuePushFront_NZ(nil,f,l,n,next) (ALib_CheckNil(nil,f)?  \
        ((f)=(l)=(n),ALib_SetNil(nil,(n)->next)):                    \
        ((n)->next=(f),(f)=(n)))
#define ALib_SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)? \
        (ALib_SetNil(nil,f),ALib_SetNil(nil,l)):          \
        ((f)=(f)->next))

//NOTE: singly-linked, singly-headed lists (stacks)
#define ALib_SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define ALib_SLLStackPop_N(f,next) ((f)=(f)->next)

//NOTE: doubly-linked-list helpers
#define ALib_DLLInsert_NP(f,l,p,n,next,prev) ALib_DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define ALib_DLLPushBack_NP(f,l,n,next,prev) ALib_DLLPushBack_NPZ(0,f,l,n,next,prev)
#define ALib_DLLPushFront_NP(f,l,n,next,prev) ALib_DLLPushFront_NPZ(0,f,l,n,next,prev)
#define ALib_DLLRemove_NP(f,l,n,next,prev) ALib_DLLRemove_NPZ(0,f,l,n,next,prev)
#define ALib_DLLInsert(f,l,p,n) ALib_DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define ALib_DLLPushBack(f,l,n) ALib_DLLPushBack_NPZ(0,f,l,n,next,prev)
#define ALib_DLLPushFront(f,l,n) ALib_DLLPushFront_NPZ(0,f,l,n,next,prev)
#define ALib_DLLRemove(f,l,n) ALib_DLLRemove_NPZ(0,f,l,n,next,prev)

//NOTE: singly-linked, doubly-headed list helpers
#define ALib_SLLQueuePush_N(f,l,n,next) ALib_SLLQueuePush_NZ(0,f,l,n,next)
#define ALib_SLLQueuePushFront_N(f,l,n,next) ALib_SLLQueuePushFront_NZ(0,f,l,n,next)
#define ALib_SLLQueuePop_N(f,l,next) ALib_SLLQueuePop_NZ(0,f,l,next)
#define ALib_SLLQueuePush(f,l,n) ALib_SLLQueuePush_NZ(0,f,l,n,next)
#define ALib_SLLQueuePushFront(f,l,n) ALib_SLLQueuePushFront_NZ(0,f,l,n,next)
#define ALib_SLLQueuePop(f,l) ALib_SLLQueuePop_NZ(0,f,l,next)

//NOTE: singly-linked, singly-headed list helpers
#define ALib_SLLStackPush(f,n) ALib_SLLStackPush_N(f,n,next)
#define ALib_SLLStackPop(f) ALib_SLLStackPop_N(f,next)

#endif
