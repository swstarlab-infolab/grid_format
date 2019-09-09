#ifndef _MIXX_BITSET_H_
#define _MIXX_BITSET_H_
#include <mixx/mixx_env.h>

namespace mixx {

template <typename RequestType, typename OffsetType>
MIXX_FORCEINLINE RequestType get_bit(OffsetType off) {
    RequestType c = 1;
    return c << off;
}

template <typename RequestType, typename OffsetType>
MIXX_FORCEINLINE void set_bit(RequestType* bitmap, OffsetType off) {
    bitmap[off / (sizeof(RequestType) * 8)] |= get_bit<RequestType>(off % (sizeof(RequestType) * 8));
}

template <typename RequestType, typename OffsetType>
MIXX_FORCEINLINE void clear_bit(RequestType* bitmap, OffsetType off) {
    bitmap[off / (sizeof(RequestType) * 8)] &= ~get_bit<RequestType>(off % (sizeof(RequestType) * 8));
}

template <typename RequestType, typename OffsetType>
MIXX_FORCEINLINE bool test_bit(RequestType* bitmap, OffsetType off) {
    return  bitmap[off / (sizeof(RequestType) * 8)] & get_bit<RequestType>(off % (sizeof(RequestType) * 8));
}

static char const __num_to_bits[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

inline uint8_t count_set_bits(uint8_t const n) {
    if (n == 0)
        return 0;
    uint8_t const nibble = n & 0xf;
    return __num_to_bits[nibble] + __num_to_bits[n >> 4];
}

inline uint16_t count_set_bits(uint16_t const n) {
    if (n == 0)
        return 0;
    uint16_t const c = count_set_bits(static_cast<uint8_t>(n));
    return c + count_set_bits(static_cast<uint8_t>(n >> 8));
}

inline uint32_t count_set_bits(uint32_t const n) {
    if (n == 0)
        return 0;
    uint32_t const c = count_set_bits(static_cast<uint16_t>(n));
    return c + count_set_bits(static_cast<uint16_t>(n >> 16));
}

inline uint64_t count_set_bits(uint64_t const n) {
    if (n == 0)
        return 0;
    uint64_t const c = count_set_bits(static_cast<uint32_t>(n));
    return c + count_set_bits(static_cast<uint32_t>(n >> 32));
}

template <mixx_size_t Length>
class bitset {
public:
    constexpr static mixx_size_t length = Length;
    constexpr static mixx_size_t byte_length = length / 8;
    static_assert(length % 8 == 0, "a template argument \'Length\' must be multiple of 8");

    bitset() {
        memset(data, 0, byte_length);
    }

    bitset(bitset const& other) {
        memmove(data, other.data, byte_length);
    }

    ~bitset() noexcept {
        /* nothing to do. */
    }

    MIXX_FORCEINLINE bool test(mixx_size_t const index) const {
        const char byte = data[index / 8];
        return byte & get_bit<char>(index % 8);
    }

    MIXX_FORCEINLINE void set(mixx_size_t const index) {
        data[index / 8] |= get_bit<char>(index % 8);
    }

    MIXX_FORCEINLINE void clear(mixx_size_t const index) {
        data[index / 8] &= ~get_bit<char>(index % 8);
    }

    MIXX_FORCEINLINE void set_all() {
        memset(data, ~0, byte_length);
    }

    MIXX_FORCEINLINE void clear_all() {
        memset(data, 0, byte_length);
    }

    MIXX_FORCEINLINE mixx_size_t count() const {
        mixx_size_t l = byte_length / 8;
        mixx_size_t c = 0;
        uint8_t const* p = reinterpret_cast<uint8_t const*>(data);
        for (mixx_size_t i = 0; i < l; ++i) {
            c += ::mixx::count_set_bits(*reinterpret_cast<uint64_t const*>(p));
            p += 8;
        }
        l = byte_length - (l * 8);
        for (mixx_size_t i = 0; i < l; ++i) {
            c += ::mixx::count_set_bits(*p);
            p += 1;
        }
        return c;
    }

    MIXX_FORCEINLINE bool operator==(bitset const& other) {
        return memcmp(data, other.data, byte_length) == 0;
    }

    bitset& operator|=(bitset const& other) {
        mixx_size_t l = byte_length / 8;
        char* p1 = data;
        char const* p2 = other.data;
        for (mixx_size_t i = 0; i < l; ++i) {
            *reinterpret_cast<int64_t*>(p1) |= *reinterpret_cast<int64_t const*>(p2);
            p1 += 8;
            p2 += 8;
        }
        l = byte_length - (l * 8);
        for (mixx_size_t i = 0; i < l; ++i) {
            *p1 |= *p2;
            p1 += 1;
            p2 += 1;
        }
        return *this;
    }

    bitset& operator&=(bitset const& other) {
        mixx_size_t l = byte_length / 8;
        char* p1 = data;
        char const* p2 = other.data;
        for (mixx_size_t i = 0; i < l; ++i) {
            *reinterpret_cast<int64_t*>(p1) &= *reinterpret_cast<int64_t const*>(p2);
            p1 += 8;
            p2 += 8;
        }
        l = byte_length - (l * 8);
        for (mixx_size_t i = 0; i < l; ++i) {
            *p1 &= *p2;
            p1 += 1;
            p2 += 1;
        }
        return *this;
    }

    bitset& operator^=(bitset const& other) {
        mixx_size_t l = byte_length / 8;
        char* p1 = data;
        char const* p2 = other.data;
        for (mixx_size_t i = 0; i < l; ++i) {
            *reinterpret_cast<int64_t*>(p1) ^= *reinterpret_cast<int64_t const*>(p2);
            p1 += 8;
            p2 += 8;
        }
        l = byte_length - (l * 8);
        for (mixx_size_t i = 0; i < l; ++i) {
            *p1 ^= *p2;
            p1 += 1;
            p2 += 1;
        }
        return *this;
    }

    bitset operator|(bitset const& rhs) const {
        bitset r{ *this };
        return r |= rhs;
    }

    bitset operator&(bitset const& rhs) const {
        bitset r{ *this };
        return r &= rhs;
    }

    bitset operator^(bitset const& rhs) const {
        bitset r{ *this };
        return r ^= rhs;
    }

    char data[byte_length];
};

//! Byte swap unsigned short
inline uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

//! Byte swap short
inline int16_t swap_int16(int16_t val) {
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap unsigned int
inline uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

//! Byte swap int
inline int32_t swap_int32(int32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

//! Byte swap 64-bit int
inline int64_t swap_int64(int64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

//! Byte swap 64-bit unsigned int
inline uint64_t swap_uint64(uint64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}

} // !namespace mixx

#endif // !_MIXX_BITSET_H_
