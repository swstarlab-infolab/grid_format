#ifndef _MIXX_SIZE_H_
#define _MIXX_SIZE_H_
#include <mixx/mixx_env.h>
#include <type_traits>

namespace mixx {

template <typename T = mixx_size_t>
MIXX_FORCEINLINE constexpr T KiB(const T i) {
    return i << 10;
}

template <typename T = mixx_size_t>
MIXX_FORCEINLINE constexpr T MiB(const T i) {
    return i << 20;
}

template <typename T = mixx_size_t>
MIXX_FORCEINLINE constexpr T GiB(const T i) {
    return i << 30;
}

template <typename T = mixx_size_t>
MIXX_FORCEINLINE constexpr T TiB(const T i) {
    return i << 40;
}

template <typename T1, typename T2>
MIXX_FORCEINLINE constexpr T1 aligned_size(const T1 size, const T2 align) {
    return align * ((size + align - 1) / align);
}

template <typename T1, typename T2>
MIXX_FORCEINLINE constexpr T1 padding_size(T1 const size, T2 const align) {
    return aligned_size(size, align) - size;
}

template <typename T>
MIXX_FORCEINLINE constexpr bool is_power_of_two(T value) {
    static_assert(std::is_integral<T>::value, "invalid argument! type must be integral type");
    return value && ((value & (value - 1)) == 0);
}

template <mixx_size_t S>
struct naturally_aliged_size {
    constexpr static mixx_size_t value = aligned_size(S, mixx_arch_align);
};

#define MIXX_NATURALLY_ALIGNED_SIZE(S, Value) \
template <>\
struct naturally_aliged_size<S> {\
    constexpr static mixx_size_t value = Value;\
};

MIXX_NATURALLY_ALIGNED_SIZE(0, 0);
MIXX_NATURALLY_ALIGNED_SIZE(1, 1);
MIXX_NATURALLY_ALIGNED_SIZE(2, 2);
MIXX_NATURALLY_ALIGNED_SIZE(3, 4);
MIXX_NATURALLY_ALIGNED_SIZE(4, 4);
MIXX_NATURALLY_ALIGNED_SIZE(5, 8);
MIXX_NATURALLY_ALIGNED_SIZE(6, 8);
MIXX_NATURALLY_ALIGNED_SIZE(7, 8);
MIXX_NATURALLY_ALIGNED_SIZE(8, 8);

#undef MIXX_NATURALLY_ALIGNED_SIZE

template <typename T>
struct mixx_sizeof {
    constexpr static mixx_size_t value = sizeof(T);
};

template <>
struct mixx_sizeof<void> {
    constexpr static mixx_size_t value = 0;
};

//TODO: static version of roundup and roundup2

template <typename T>
MIXX_FORCEINLINE typename std::enable_if<std::is_signed<T>::value, T>::type roundup(const T n, const T m) {
    return ((n + ((n >= 0) ? 1 : 0) * (m - 1)) / m) * m;
}

template <typename T>
MIXX_FORCEINLINE typename std::enable_if<std::is_unsigned<T>::value, T>::type roundup(const T n, const T m) {
    return ((n + m - 1) / m) * m;
}

MIXX_FORCEINLINE uint32_t roundup2_nonzero(uint32_t n) {
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    ++n;
    return n;
}

MIXX_FORCEINLINE uint32_t roundup2(uint32_t n) {
    n += (n == 0);
    return roundup2_nonzero(n);
}

MIXX_FORCEINLINE uint64_t roundup2_nonzero(uint64_t n) {
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n |= (n >> 32);
    ++n;
    return n;
}

MIXX_FORCEINLINE uint64_t roundup2(uint64_t n) {
    n += (n == 0);
    return roundup2_nonzero(n);
}

template <uint32_t Lv, uint32_t V, uint32_t R, uint32_t ...Rest>
struct _static_log2_impl;

template <uint32_t Lv, uint32_t V, uint32_t R, uint32_t C, uint32_t ...Rest>
struct _static_log2_impl<Lv, V, R, C, Rest...> {
private:
    static constexpr uint32_t Shift = (V > C) << Lv;
public:
    static constexpr uint32_t value = _static_log2_impl<Lv - 1, (V >> Shift), (R | Shift), Rest ... > ::value;
};

template <uint32_t V, uint32_t R, uint32_t C>
struct _static_log2_impl<1, V, R, C> {
private:
    static constexpr uint32_t Shift = (V > C) << 1;
public:
    static constexpr uint32_t value = _static_log2_impl<0, (V >> Shift), (R | Shift)>::value;
};

template <uint32_t V, uint32_t R>
struct _static_log2_impl<0, V, R> {
    static constexpr uint32_t value = R | (V >> 1);
};

template <uint32_t V>
struct static_log2 {
    static_assert(V != 0, "The logarithm of zero is undefined");
private:
    static constexpr uint32_t R = (V > 0xFFFF);
public:
    static constexpr uint32_t value = 
        _static_log2_impl<3, (V >> R), R, 0xFF, 0xF, 0x3>::value;
};

enum class size_prefix: int {
    EiB = 0,
    PiB,
    TiB,
    GiB,
    MiB,
    KiB,
    B
};

char* size2str_decimal(uint64_t size, char* out, mixx_size_t bufsize);
char* size2str_binary(uint64_t size, char* out, mixx_size_t bufsize);
char* size2str_binary(uint64_t size, size_prefix base, char* out, mixx_size_t bufsize);

} // !namespace mixx

#endif // !_MIXX_SIZE_H_
