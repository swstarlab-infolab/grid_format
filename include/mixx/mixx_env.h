#ifndef _MIXX_ENV_H_
#define _MIXX_ENV_H_
#include <mixx/mixx_config.h>
#include <mixx/pphack.h>
#include <stdint.h>

#if defined(_MSC_VER)
#define MIXX_TOOLCHAIN_MSVC
#define MIXX_TOOLCHAIN_VER _MSC_FULL_VER
#define MIXX_TOOLCHAIN_NAME "msvc" MIXX_PP_STRING(MIXX_TOOLCHAIN_VER)

#elif defined(__GNUC__) && !defined(__clang__)
#define MIXX_TOOLCHAIN_GNUC
#define MIXX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define MIXX_TOOLCHAIN_VER MIXX_GCC_VERSION
#define MIXX_TOOLCHAIN_NAME "gcc " MIXX_PP_STRING(MIXX_TOOLCHAIN_VER)
#define fopen_s(fp, fmt, mode)          *(fp)=fopen( (fmt), (mode))
#define strcpy_s(dst, max_len, src)		strncpy((dst), (src), (max_len))

#elif defined(__clang__)
#define MIXX_TOOLCHAIN_CLANG
#define MIXX_CLANG_VERSION (clang_major * 10000 + clang_minor * 100 + __clang_patchlevel__)
#define MIXX_TOOLCHAIN_VER MIXX_CLANG_VERSION
#define MIXX_TOOLCHAIN_NAME "clang " MIXX_PP_STRING(MIXX_TOOLCHAIN_VER)
#define fopen_s(fp, fmt, mode)          *(fp)=fopen( (fmt), (mode))
#define strcpy_s(dst, max_len, src)		strncpy((dst), (src), (max_len))

#else
#error "Unrecognized toolchain"
#endif // !TOOLCHAIN

#if defined(_MSC_VER)

// Detect AMD64 in MSVC
#if defined (_M_X64) 
#define MIXX_ARCH_AMD64
#define MIXX_ARCH_64
// Detect ARM
#elif defined(_M_ARM)
#define MIXX_ARCH_ARM
#define MIXX_ARCH_32
// Detect ARM64
#elif defined(__aarch64__)
#define MIXX_ARCH_ARM64
#define MIXX_ARCH_64
// Detect Intel x86
#elif defined(_M_IX86)
#define MIXX_ARCH_IX86
#define MIXX_ARCH_32
// Detect Intel Itanium (IA-64)
#elif defined(_M_IA64)
#define MIXX_ARCH_IA64
#define MIXX_ARCH_64
// Detect PowerPC
#elif defined(_M_PPC)
#define MIXX_ARCH_PPC
#define MIXX_ARCH_64
#else
#error "Unrecognized architecture"
#endif

#elif defined(__GNUC__) || defined(__GNUG__)
// Detect AMD64 in GNUC
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define MIXX_ARCH_AMD64
#define MIXX_ARCH_64
// Detect ARM
#elif defined(__arm__)
#define MIXX_ARCH_ARM
#define MIXX_ARCH_32
// Detect ARM64
#elif defined(__aarch64__)
#define MIXX_ARCH_ARM64
#define MIXX_ARCH_64
// Detect Intel x86
#elif defined(i386) || defined(__i386) || defined(__i386__)
#define MIXX_ARCH_IX86
#define MIXX_ARCH_32
// Detect Intel Itanium (IA-64)
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__)
#define MIXX_ARCH_IA64
#define MIXX_ARCH_64
// Detect PowerPC
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#define MIXX_ARCH_PPC64
#define MIXX_ARCH_64
#endif
#else
#error "Unrecognized architecture"
#endif // ARCH

#if defined(_WIN32)
#define MIXX_OS_WINDOWS
#elif defined(__linux__)
#define MIXX_OS_LINUX
#elif defined(__APPLE__)
#define MIXX_OS_APPLE
#elif defined(__ANDROID__)
#define MIXX_OS_ANDROID
#else
#error "Unrecognized Operating Systems"
#endif

#if defined(MIXX_TOOLCHAIN_MSVC)
#define MIXX_LIKELY(expression) (expression)
#define MIXX_UNLIKELY(expression) (expression)
#define MIXX_NOINLINE __declspec(noinline)
#define MIXX_FORCEINLINE __forceinline
#define MIXX_ALIGN(n) __declspec(align(n))
#define MIXX_PATH_SEPARATOR '\\'
#define MIXX_PATH_SEPARATOR_STR "\\"
#elif defined(MIXX_TOOLCHAIN_GNUC)
#define MIXX_LIKELY(expression) __builtin_expect(expression, 1)
#define MIXX_UNLIKELY(expression) __builtin_expect(expression, 0)
#define MIXX_NOINLINE __attribute__((noinline))
#define MIXX_FORCEINLINE __inline __attribute__((always_inline))
#define MIXX_ALIGN(n) __attribute__((aligned(n)))
#define MIXX_PATH_SEPARATOR '/'
#define MIXX_PATH_SEPARATOR_STR "/"
#else
#error "Unknown tool chain!"
#endif // !KEYWORDS

namespace mixx {

#if defined(MIXX_ARCH_64)
    using mixx_size_t = uint64_t;
    using mixx_ssize_t = int64_t;
    constexpr mixx_size_t mixx_arch_align = 8;
#elif defined(MIXX_ARCH_32)
    using mixx_size_t = uint32_t;
    using mixx_ssize_t = int32_t;
    constexpr mixx_size_t mixx_arch_align = 4;
#else
#error "Not supported platform! (size type error)"
#endif
    using mxsize_t = mixx_size_t;
    using mxssize_t = mixx_ssize_t;

template <typename T>
T* seek_pointer(T* p, mixx_size_t size) {
    auto const p2 = reinterpret_cast<char*>(p);
    return reinterpret_cast<T*>(p2 + size);
}

MIXX_FORCEINLINE bool is_aligned_address(void* addr, uint64_t alignment) {
    auto const addr2 = reinterpret_cast<uint64_t>(addr);
    return addr2 % alignment == 0;
}

template <typename T1, typename T2>
MIXX_FORCEINLINE bool is_aligned_offset(T1 offset, T2 alignment) {
    return offset % alignment == 0;
}

} // !namespace mixx

#endif // !_MIXX_ENV_H_
