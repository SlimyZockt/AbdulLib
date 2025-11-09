#ifndef ALIB_BASE_CONTEXT_H
#define ALIB_BASE_CONTEXT_H

//NOTE: Clang OS/Arch Cracking
#if defined(__clang__)
# define ALIB_COMPILER_CLANG 1
#if defined(_WIN32)
# define ALIB_OS_WINDOWS 1
#elif defined(__gnu_linux__) || defined(__linux__)
# define ALIB_OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
# define ALIB_OS_MAC 1
#else
# error This compiler/OS combo is not supported.
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
# define ALIB_ARCH_X64 1
#elif defined(i386) || defined(__i386) || defined(__i386__)
# define ALIB_ARCH_X86 1
#elif defined(__aarch64__)
# define ALIB_ARCH_ARM64 1
#elif defined(__arm__)
# define ALIB_ARCH_ARM32 1
#else
# error Architecture not supported.
#endif

//NOTE: MSVC OS/Arch Cracking
#elif defined(_MSC_VER)
# define ALIB_COMPILER_MSVC 1

#if _MSC_VER >= 1920
#define ALIB_COMPILER_MSVC_YEAR 2019
#elif _MSC_VER >= 1910
#define ALIB_COMPILER_MSVC_YEAR 2017
#elif _MSC_VER >= 1900
#define ALIB_COMPILER_MSVC_YEAR 2015
#elif _MSC_VER >= 1800
#define ALIB_COMPILER_MSVC_YEAR 2013
#elif _MSC_VER >= 1700
#define ALIB_COMPILER_MSVC_YEAR 2012
#elif _MSC_VER >= 1600
#define ALIB_COMPILER_MSVC_YEAR 2010
#elif _MSC_VER >= 1500
#define ALIB_COMPILER_MSVC_YEAR 2008
#elif _MSC_VER >= 1400
#define ALIB_COMPILER_MSVC_YEAR 2005
#else
#define ALIB_COMPILER_MSVC_YEAR 0
#endif

#if defined(_WIN32)
#define ALIB_OS_WINDOWS 1
#else
#error This compiler/OS combo is not supported.
#endif

#if defined(_M_AMD64)
#define ALIB_ARCH_X64 1
#elif defined(_M_IX86)
#define ALIB_ARCH_X86 1
#elif defined(_M_ARM64)
#define ALIB_ARCH_ARM64 1
#elif defined(_M_ARM)
#define ALIB_ARCH_ARM32 1
#else
#error Architecture not supported.
#endif

//NOTE: GCC OS/Arch Cracking
#elif defined(__GNUC__) || defined(__GNUG__)

#define ALIB_COMPILER_GCC 1

#if defined(__gnu_linux__) || defined(__linux__)
#define ALIB_OS_LINUX 1
#else
#error This compiler/OS combo is not supported.
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define ALIB_ARCH_X64 1
#elif defined(i386) || defined(__i386) || defined(__i386__)
#define ALIB_ARCH_X86 1
#elif defined(__aarch64__)
#define ALIB_ARCH_ARM64 1
#elif defined(__arm__)
#define ALIB_ARCH_ARM32 1
#else
#error Architecture not supported.
#endif
#else
#error Compiler not supported.
#endif

//NOTE: Arch Cracking
#if defined(ALIB_ARCH_X64)
#define ALIB_ARCH_64BIT 1
#elif defined(ALIB_ARCH_X86)
#define ALIB_ARCH_32BIT 1
#endif

#if ALIB_ARCH_ARM32 || ALIB_ARCH_ARM64 || ALIB_ARCH_X64 || ALIB_ARCH_X86
#define ARCH_LITTLE_ENDIAN 1
#else
#error Endianness of this architecture not understood by context cracker.
#endif

//NOTE: Language Cracking
#if defined(__cplusplus)
#define ALIB_LANG_CPP 1
#else
#define ALIB_LANG_C 1
#endif

//NOTE: Zero All Undefined Options
#if !defined(ALIB_ARCH_32BIT)
#define ALIB_ARCH_32BIT 0
#endif
#if !defined(ALIB_ARCH_64BIT)
#define ALIB_ARCH_64BIT 0
#endif
#if !defined(ALIB_ARCH_X64)
#define ALIB_ARCH_X64 0
#endif
#if !defined(ALIB_ARCH_X86)
#define ALIB_ARCH_X86 0
#endif
#if !defined(ALIB_ARCH_ARM64)
#define ALIB_ARCH_ARM64 0
#endif
#if !defined(ALIB_ARCH_ARM32)
#define ALIB_ARCH_ARM32 0
#endif
#if !defined(ALIB_COMPILER_MSVC)
#define ALIB_COMPILER_MSVC 0
#endif
#if !defined(ALIB_COMPILER_GCC)
#define ALIB_COMPILER_GCC 0
#endif
#if !defined(ALIB_COMPILER_CLANG)
#define ALIB_COMPILER_CLANG 0
#endif
#if !defined(ALIB_OS_WINDOWS)
#define ALIB_OS_WINDOWS 0
#endif
#if !defined(ALIB_OS_LINUX)
#define ALIB_OS_LINUX 0
#endif
#if !defined(ALIB_OS_MAC)
#define ALIB_OS_MAC 0
#endif
#if !defined(ALIB_LANG_CPP)
#define ALIB_LANG_CPP 0
#endif
#if !defined(ALIB_LANG_C)
#define ALIB_LANG_C 0
#endif

//NOTE: Unsupported Errors
#if ALIB_ARCH_X86
#error You tried to build in x86 (32 bit) mode, but currently, only building in x64 (64 bit) mode is supported.
#endif

#if !ALIB_ARCH_X64
#error You tried to build with an unsupported architecture. Currently, only building in x64 mode is supported.
#endif

#endif

