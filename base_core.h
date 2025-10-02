#ifndef ALIB_BASE_CORE_H
#define ALIB_BASE_CORE_H
//NOTE: Foreign Includes
#include <stdint.h>
#include <string.h>

#ifdef ALIB_BASE_CORE_STIP_PREFIX 
//NOTE: Codebase Keywords
#define internal alib_internal
#define global alib_global
#define local_persist alib_local_persist
#define rodata alib_rodata
//NOTE: Utility Marcos
#define Statement alib_Statement
#define Stringify alib_Stringify
#define Glue alib_Glue
#define ArrayCount alib_ArrayCount
#define Swap alib_Swap
#define IntFromPtr alib_IntFromPtr
#define PtrFromInt alib_PtrFromInt
#define Compose64Bit alib_Compose64Bit
#define Compose32Bit alib_Compose32Bit
#define AlignPow2 alib_AlignPow2
#define AlignDownPow alib_AlignDownPow
#define AlignPadPow2 alib_AlignPadPow2
#define IsPow2 alib_IsPow2
#define IsPow2OrZero alib_IsPow2OrZero
#define ExtractBit alib_ExtractBit
// #define Extract8 alib_Extract8
// #define Extract16 alib_Extract16
// #define Extract32 alib_Extract32
//NOTE: Type -> Alignment
#define AlionOf alib_AlignOf
//NOTE: Units
#define KB alib_KB
#define MB alib_MB
#define GB alib_GB
#define TB alib_TB
#define Thousand alib_Thousand
#define Million alib_Million
#define Billion alib_Billion
//NOTE: Types
#define u8;
#define u16;
#define u32;
#define u64;
#define i8;
#define i16;
#define i32;
#define i64;
#define b8;
#define b16;
#define b32;
#define b64;
#define f32;
#define f64;
#define void alib_VoidProc(void);
//NOTE: Assert
#define Trap alib_Trap
#define SourceCodeLocation alib_SourceCodeLocation
#define SourceLocation alib_SourceLocation 
#define AssertAlways alib_AssertAlways
#define Assert alib_Assert
#define InvalidPath alib_InvalidPath
#define NotImplemented alib_NotImplemented
#define NoOp alib_NoOp
#define StaticAssert alib_StaticAssert
//NOTE: Member Offsets
#define alib_Member(T,m)                 (((T*)0)->m)
#define alib_OffsetOf(T,m)               alib_IntFromPtr(&Member(T,m))
#define alib_MemberFromOffset(T,ptr,off) (T)((((U8 *)ptr)+(off)))
#define alib_CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - alib_OffsetOf(T,m))
#define alib_Min(a,b) (((a)<(b)) ? (a):(b))
#define alib_Max(a,b) (((a)>(b)) ? (a):(b))
#define alib_Clamp(a,x,b) (((x)<(a)) ? (a):((b)<(x)) ? (b):(x))
//NOTE: For-Loop Construct Macros
#define DeferLoop alib_DeferLoop
#define DeferLoopChecked alib_DeferLoopChecked
#define EachIndex alib_EachIndex
#define EachElement alib_EachElement
//NOTE: Memory
#define MemoryCopy alib_MemoryCopy
#define MemorySet alib_MemorySet
#define MemoryCompare alib_MemoryCompare
#define MemoryStrlen alib_MemoryStrlen

#define MemoryCopyStruct alib_MemoryCopyStruct
#define MemoryCopyArray alib_MemoryCopyArray
#define MemoryCopyTyped alib_MemoryCopyTyped
#define MemoryCopyStr8 alib_MemoryCopyStr8

#define MemoryZero alib_MemoryZero
#define MemoryZeroStruct alib_MemoryZeroStruct
#define MemoryZeroArray alib_MemoryZeroArray
#define MemoryZeroTyped alib_MemoryZeroTyped

#define MemoryMatch alib_MemoryMatch
#define MemoryMatchStruct alib_MemoryMatchStruct
#define MemoryMatchArray alib_MemoryMatchArray

#define MemoryRead alib_MemoryRead
#define MemoryConsume alib_MemoryConsume
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

#if ARCH_64BIT
    #define alib_IntFromPtr(ptr) ((U64)(ptr))
#elif ARCH_32BIT
    #define alib_IntFromPtr(ptr) ((U32)(ptr))
#else
    #error Missing pointer-to-integer cast for this architecture.
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
#if COMPILER_MSVC
# define alib_AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
# define alib_AlignOf(T) __alignof(T)
#elif COMPILER_GCC
# define alib_AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

//NOTE: Units
#define alib_KB(n)  (((U64)(n)) << 10)
#define alib_MB(n)  (((U64)(n)) << 20)
#define alib_GB(n)  (((U64)(n)) << 30)
#define alib_TB(n)  (((U64)(n)) << 40)
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
typedef i8 alib_b8;
typedef i16 alib_b16;
typedef i32 alib_b32;
typedef i64 alib_b64;
typedef float alib_f32;
typedef double alib_f64;
typedef void alib_VoidProc(void);

//NOTE: Asserts
#if COMPILER_MSVC
    # define alib_Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
    # define alib_Trap() __builtin_trap()
#else
    # error Unknown trap intrinsic for this compiler.
    // #define Trap() (*(int*)( 0 ) = 0)
#endif

typedef struct alib_SourceCodeLocation {
    char* file;
    char* line;
} alib_SourceCodeLocation;

#define alib_SourceLocation ((alib_SourceCodeLocation){__FILE__, __LINE__})

#define alib_AssertAlways(x) alib_Statement(if(!(x)) {alib_Trap();})

#if BUILD_DEBUG
    #define alib_Assert(x) alib_AssertAlways(x)
#else
    #define alib_Assert(x) (void)(x) #endif
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

#define alib_MemoryZero(s,z)       memset((s),0,(z))
#define alib_MemoryZeroStruct(s)   alib_MemoryZero((s),sizeof(*(s)))
#define alib_MemoryZeroArray(a)    alib_MemoryZero((a),sizeof(a))
#define alib_MemoryZeroTyped(m,c)  alib_MemoryZero((m),sizeof(*(m))*(c))

#define alib_MemoryMatch(a,b,z)     (alib_MemoryCompare((a),(b),(z)) == 0)
#define alib_MemoryMatchStruct(a,b)  alib_MemoryMatch((a),(b),sizeof(*(a)))
#define alib_MemoryMatchArray(a,b)   alib_MemoryMatch((a),(b),sizeof(a))

#define alib_MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define alib_MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

#endif
