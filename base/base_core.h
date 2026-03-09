#ifndef ALibBASE_CORE_H
#define ALibBASE_CORE_H
//NOTE: Foreign Includes
#include <stdint.h>
#include <string.h>

#if ALIB_BASE_CORE_STRIP_PREFIX 
//NOTE: Codebase Keywords
# define internal ALibInternal
# define global ALibGlobal
# define local_persist ALibLocal_persist
# define rodata ALibRodata

//NOTE: Utility Marcos
# define Statement ALibStatement
# define Stringify ALibStringify
# define Glue ALibGlue
# define ArrayCount ALibArrayCount
# define Swap ALibSwap
# define IntFromPtr ALibIntFromPtr
# define PtrFromInt ALibPtrFromInt
# define Compose64Bit ALibCompose64Bit
# define Compose32Bit ALibCompose32Bit
# define AlignPow2 ALibAlignPow2
# define AlignDownPow ALibAlignDownPow
# define AlignPadPow2 ALibAlignPadPow2
# define IsPow2 ALibIsPow2
# define IsPow2OrZero ALibIsPow2OrZero
# define ExtractBit ALibExtractBit
// #define Extract8 ALibExtract8
// #define Extract16 ALibExtract16
// #define Extract32 ALibExtract32
//NOTE: Type -> Alignment
# define AlionOf ALibAlignOf
//NOTE: Primitive and core types
# define U8 ALibU8
# define U16 ALibU16
# define U32 ALibU32
# define U64 ALibU64
# define I8 ALibI8
# define I16 ALibI16
# define I32 ALibI32
# define I64 ALibI64
# define B8 ALibB8
# define B16 ALibB16
# define B32 ALibB32
# define B64 ALibB64
# define F32 ALibF32
# define F64 ALibF64
# define VoidProc ALibVoidProc
# define SourceLocation ALibSourceLocation
//NOTE: Units
# define KB ALibKB
# define MB ALibMB
# define GB ALibGB
# define TB ALibTB
# define Thousand ALibThousand
# define Million ALibMillion
# define Billion ALibBillion
//NOTE: Assert
# define Trap ALibTrap
# define SourceCodeLocation ALibSourceCodeLocation
# define CallerLocation ALibCallerLocation 
# define AssertAlways ALibEnsure
# define Assert ALibAssert
# define InvalidPath ALibInvalidPath
# define NotImplemented ALibNotImplemented
# define NoOp ALibNoOp
# define StaticAssert ALibStaticAssert
//NOTE: Member Offsets
# define Member ALibMember
# define OffsetOf ALibOffsetOf
# define MemberFromOffset ALibMemberFromOffset
# define CastFromMember ALibCastFromMember
# define Min ALibMin
# define Max ALibMax
# define ClampTop ALibClampTop
# define ClampBot ALibClampBot
# define Clamp ALibClamp
//NOTE: For-Loop Construct Macros
# define DeferLoop ALibDeferLoop
# define DeferLoopChecked ALibDeferLoopChecked
# define EachIndex ALibEachIndex
# define EachElement ALibEachElement
//NOTE: Memory
# define MemoryCopy ALibMemoryCopy
# define MemorySet ALibMemorySet
# define MemoryCompare ALibMemoryCompare
# define MemoryStrlen ALibMemoryStrlen
  
# define MemoryCopyStruct ALibMemoryCopyStruct
# define MemoryCopyArray ALibMemoryCopyArray
# define MemoryCopyTyped ALibMemoryCopyTyped
# define MemoryCopyStr8 ALibMemoryCopyStr8
  
# define MemoryZero ALibMemoryZero
# define MemoryZeroStruct ALibMemoryZeroStruct
# define MemoryZeroArray ALibMemoryZeroArray
# define MemoryZeroTyped ALibMemoryZeroTyped
  
# define MemoryMatch ALibMemoryMatch
# define MemoryMatchStruct ALibMemoryMatchStruct
# define MemoryMatchArray ALibMemoryMatchArray
  
# define MemoryRead ALibMemoryRead
# define MemoryConsume ALibMemoryConsume

// Linked List Building Macros

//NOTE: linked list macro helpers
# define CheckNil ALibCheckNil
# define SetNil ALibSetNil
  
//NOTE: doubly-linked-lists
# define DLLInsert_NPZ ALibDLLInsert_NPZ
# define DLLPushBack_NPZ ALibDLLPushBack_NPZ
# define DLLPushFront_NPZ ALibDLLPushFront_NPZ
# define DLLRemove_NPZ ALibDLLRemove_NPZ
  
//NOTE: singly-linked, doubly-headed lists (queues)
# define SLLQueuePush_NZ ALibSLLQueuePush_NZ
# define SLLQueuePushFront_NZ ALibSLLQueuePushFront_NZ
# define SLLQueuePop_NZ ALibSLLQueuePop_NZ
  
//NOTE: singly-linked, singly-headed lists (stacks)
# define SLLStackPush_N ALibSLLStackPush_N
# define SLLStackPop_N ALibSLLStackPop_N
  
//NOTE: doubly-linked-list helpers
# define DLLInsert_NP ALibDLLInsert_NP
# define DLLPushBack_NP ALibDLLPushBack_NP
# define DLLPushFront_NP ALibDLLPushFront_NP
# define DLLRemove_NP ALibDLLRemove_NP
# define DLLInsert    ALibDLLInsert
# define DLLPushBack  ALibDLLPushBack
# define DLLPushFront ALibDLLPushFront
# define DLLRemove    ALibDLLRemove
  
//NOTE: singly-linked, doubly-headed list helpers
# define SLLQueuePush_N      ALibSLLQueuePush_N
# define SLLQueuePushFront_N ALibSLLQueuePushFront_N
# define SLLQueuePop_N       ALibSLLQueuePop_N
# define SLLQueuePush        ALibSLLQueuePush
# define SLLQueuePushFront   ALibSLLQueuePushFront
# define SLLQueuePop         ALibSLLQueuePop
  
//NOTE: singly-linked, singly-headed list helpers
# define SLLStackPush ALibSLLStackPush
# define SLLStackPop  ALibSLLStackPop

#endif

#ifndef ALIB_DEF
# define ALIB_DEF
#endif

//NOTE: Codebase Keywords
#define ALibInternal static
#define ALibGlobal static
#define ALibLocal_persist static

#define ALibRodata static const

//NOTE: Utility Marcos
#define ALibTypedef(type, name) typedef type name
#define ALibStructForward(name) ALibTypedef(struct name,name)
#define ALibStruct(name) ALibStructForward(name); struct name
#define ALibUnionForward(name) ALibTypedef(union name, name)
#define ALibUnion(name) ALibUnionForward(name); union name
#define ALibEnum(name, size) ALibTypedef(size, name); enum name

#define ALibStatement(S) do {S} while(0)

#define ALibStringify_(S) #S
#define ALibStringify(S) ALibStringify_(S)
#define ALibGlue_(A,B) A##B
#define ALibGlue(A,B) ALibGlue_(A,B)
#define ALibArrayCount(a) (sizeof(a)/sizeof(*(a)))
#define ALibSwap(T,a,b) Statement(t__ = a; a = b; b = t__;)

#if ALIB_ARCH_X64
# define ALibIntFromPtr(ptr) ((ALibU64)(ptr))
#elif ALIB_ARCH_X86
# define ALibIntFromPtr(ptr) ((ALibU32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif

#define ALibPtrFromInt(i) (void*)(i)

#define ALibCompose64Bit(a,b)  ((((ALibU64)a) << 32) | ((ALibU64)b))
#define ALibCompose32Bit(a,b)  ((((ALibU32)a) << 16) | ((ALibU32)b))
#define ALibAlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define ALibAlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define ALibAlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define ALibIsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define ALibIsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ALibExtractBit(word, idx) (((word) >> (idx)) & 1)

// #define ALibExtract8(word, pos)   (((word) >> ((pos)*8))  & max_U8)
// #define ALibExtract16(word, pos)  (((word) >> ((pos)*16)) & max_U16)
// #define ALibExtract32(word, pos)  (((word) >> ((pos)*32)) & max_U32)

//NOTE: Type -> Alignment
#if ALIB_COMPILER_MSVC
# define ALibAlignOf(T) __alignof(T)
#elif ALIB_COMPILER_CLANG
# define ALibAlignOf(T) __alignof(T)
#elif ALIB_COMPILER_GCC
# define ALibAlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

//NOTE: Units
#define ALibKB(n)  (((ALibU64)(n)) << 10)
#define ALibMB(n)  (((ALibU64)(n)) << 20)
#define ALibGB(n)  (((ALibU64)(n)) << 30)
#define ALibTB(n)  (((ALibU64)(n)) << 40)
#define ALibThousand(n)   ((n)*1000)
#define ALibMillion(n)    ((n)*1000000)
#define ALibBillion(n)    ((n)*1000000000)

//NOTE: Types
ALibTypedef(uint8_t, ALibU8);
ALibTypedef(uint16_t, ALibU16);
ALibTypedef(uint32_t, ALibU32);
ALibTypedef(uint64_t, ALibU64);
ALibTypedef(int8_t, ALibI8);
ALibTypedef(int16_t, ALibI16);
ALibTypedef(int32_t, ALibI32);
ALibTypedef(int64_t, ALibI64);
ALibTypedef(ALibI8, ALibB8);
ALibTypedef(ALibI16, ALibB16);
ALibTypedef(ALibI32, ALibB32);
ALibTypedef(ALibI64, ALibB64);
ALibTypedef(float, ALibF32);
ALibTypedef(double, ALibF64);
ALibTypedef(void, ALibVoidProc(void));

//NOTE: Asserts
#if ALIB_COMPILER_MSVC 
# define ALibTrap() __debugbreak()
#elif ALIB_COMPILER_CLANG || ALIB_COMPILER_GCC
# define ALibTrap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

ALibStruct(ALibSourceLocation) {
    char* file;
    int line;
};

#define ALibCallerLocation ((ALibSourceLocation){__FILE__, __LINE__})

#define ALibEnsure(x) ALibStatement(if(!(x)) {ALibTrap();})

#if ALIB_DEBUG_BUILD
# define ALibAssert(x) ALibEnsure(x)
#else
# define ALibAssert(x) (void)(x)
#endif

#define ALibInvalidPath        ALibAssert(!"Invalid Path!")
#define ALibNotImplemented     ALibAssert(!"Not Implemented!")
#define ALibNoOp               ((void)0)
#define ALibStaticAssert(C, ID) ALibGlobal ALibU8 ALibGlue(ID, __LINE__)[(C)?1:-1]

//NOTE: Member Offsets
#define ALibMember(T,m)                 (((T*)0)->m)
#define ALibOffsetOf(T,m)               ALibIntFromPtr(&Member(T,m))
#define ALibMemberFromOffset(T,ptr,off) (T)((((ALibU8*)ptr)+(off)))
#define ALibCastFromMember(T,m,ptr)     (T*)(((ALibU8*)ptr) - ALibOffsetOf(T,m))

#define ALibMin(a,b) (((a)<(b)) ? (a):(b))
#define ALibMax(a,b) (((a)>(b)) ? (a):(b))
#define ALibClamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))
#define ALibClampTop(a,b) ALibMin(a, b)
#define ALibClampBot(a,b) ALibMax(a,b)

//NOTE: For-Loop Construct Macros
#define ALibDeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define ALibDeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))
#define ALibEachIndex(it, count) (ALibU64 it = 0; it < (count); it += 1)
#define ALibEachElement(it, array) (ALibU64 it = 0; it < ALibArrayCount(array); it += 1)

//NOTE: Memory
#define ALibMemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define ALibMemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define ALibMemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define ALibMemoryStrlen(ptr)             strlen(ptr)

#define ALibMemoryCopyStruct(d,s)  ALibMemoryCopy((d),(s),sizeof(*(d)))
#define ALibMemoryCopyArray(d,s)   ALibMemoryCopy((d),(s),sizeof(d))
#define ALibMemoryCopyTyped(d,s,c) ALibMemoryCopy((d),(s),sizeof(*(d))*(c))
#define ALibMemoryCopyStr8(dst, s) ALibMemoryCopy(dst, (s).str, (s).size)

#ifdef ALIB_BUILD_DEBUG 
# define ALibMemoryZero(s,z)       memset((s),0xCB,(z))
#else
# define ALibMemoryZero(s,z)       memset((s),0,(z))
#endif

#define ALibMemoryZeroStruct(s)   ALibMemoryZero((s),sizeof(*(s)))
#define ALibMemoryZeroArray(a)    ALibMemoryZero((a),sizeof(a))
#define ALibMemoryZeroTyped(m,c)  ALibMemoryZero((m),sizeof(*(m))*(c))

#define ALibMemoryMatch(a,b,z)     (ALibMemoryCompare((a),(b),(z)) == 0)
#define ALibMemoryMatchStruct(a,b)  ALibMemoryMatch((a),(b),sizeof(*(a)))
#define ALibMemoryMatchArray(a,b)   ALibMemoryMatch((a),(b),sizeof(a))

#define ALibMemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define ALibMemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

// Linked List Building Macros

//NOTE: linked list macro helpers
#define ALibCheckNil(nil,p) ((p) == 0 || (p) == nil)
#define ALibSetNil(nil,p) ((p) = nil)

//NOTE: doubly-linked-lists
#define ALibDLLInsert_NPZ(nil,f,l,p,n,next,prev) (ALibCheckNil(nil,f) ?                                     \
        ((f) = (l) = (n), ALibSetNil(nil,(n)->next), ALibSetNil(nil,(n)->prev)) :                           \
        ALibCheckNil(nil,p) ?                                                                           \
        ((n)->next = (f), (f)->prev = (n), (f) = (n), ALibSetNil(nil,(n)->prev)) :                      \
        ((p)==(l)) ?                                                                                \
        ((l)->next = (n), (n)->prev = (l), (l) = (n), ALibSetNil(nil, (n)->next)) :                     \
        (((!ALibCheckNil(nil,p) && ALibCheckNil(nil,(p)->next)) ? (0) :                                     \
        ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define ALibDLLPushBack_NPZ(nil,f,l,n,next,prev) ALibDLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define ALibDLLPushFront_NPZ(nil,f,l,n,next,prev) ALibDLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define ALibDLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),   \
        ((n) == (l) ? (l) = (l)->prev : (0)),                                       \
        (ALibCheckNil(nil,(n)->prev) ? (0) :                                            \
        ((n)->prev->next = (n)->next)),                                             \
        (ALibCheckNil(nil,(n)->next) ? (0) :                                            \
        ((n)->next->prev = (n)->prev)))

//NOTE: singly-linked, doubly-headed lists (queues)
#define ALibSLLQueuePush_NZ(nil,f,l,n,next) (ALibCheckNil(nil,f)?   \
        ((f)=(l)=(n),ALibSetNil(nil,(n)->next)):                \
        ((l)->next=(n),(l)=(n),ALibSetNil(nil,(n)->next)))
#define ALibSLLQueuePushFront_NZ(nil,f,l,n,next) (ALibCheckNil(nil,f)?  \
        ((f)=(l)=(n),ALibSetNil(nil,(n)->next)):                    \
        ((n)->next=(f),(f)=(n)))
#define ALibSLLQueuePop_NZ(nil,f,l,next) ((f)==(l)? \
        (ALibSetNil(nil,f),ALibSetNil(nil,l)):          \
        ((f)=(f)->next))

//NOTE: singly-linked, singly-headed lists (stacks)
#define ALibSLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define ALibSLLStackPop_N(f,next) ((f)=(f)->next)

//NOTE: doubly-linked-list helpers
#define ALibDLLInsert_NP(f,l,p,n,next,prev) ALibDLLInsert_NPZ(0,f,l,p,n,next,prev)
#define ALibDLLPushBack_NP(f,l,n,next,prev) ALibDLLPushBack_NPZ(0,f,l,n,next,prev)
#define ALibDLLPushFront_NP(f,l,n,next,prev) ALibDLLPushFront_NPZ(0,f,l,n,next,prev)
#define ALibDLLRemove_NP(f,l,n,next,prev) ALibDLLRemove_NPZ(0,f,l,n,next,prev)
#define ALibDLLInsert(f,l,p,n) ALibDLLInsert_NPZ(0,f,l,p,n,next,prev)
#define ALibDLLPushBack(f,l,n) ALibDLLPushBack_NPZ(0,f,l,n,next,prev)
#define ALibDLLPushFront(f,l,n) ALibDLLPushFront_NPZ(0,f,l,n,next,prev)
#define ALibDLLRemove(f,l,n) ALibDLLRemove_NPZ(0,f,l,n,next,prev)

//NOTE: singly-linked, doubly-headed list helpers
#define ALibSLLQueuePush_N(f,l,n,next) ALibSLLQueuePush_NZ(0,f,l,n,next)
#define ALibSLLQueuePushFront_N(f,l,n,next) ALibSLLQueuePushFront_NZ(0,f,l,n,next)
#define ALibSLLQueuePop_N(f,l,next) ALibSLLQueuePop_NZ(0,f,l,next)
#define ALibSLLQueuePush(f,l,n) ALibSLLQueuePush_NZ(0,f,l,n,next)
#define ALibSLLQueuePushFront(f,l,n) ALibSLLQueuePushFront_NZ(0,f,l,n,next)
#define ALibSLLQueuePop(f,l) ALibSLLQueuePop_NZ(0,f,l,next)

//NOTE: singly-linked, singly-headed list helpers
#define ALibSLLStackPush(f,n) ALibSLLStackPush_N(f,n,next)
#define ALibSLLStackPop(f) ALibSLLStackPop_N(f,next)

#endif
