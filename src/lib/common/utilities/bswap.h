#pragma once

#include <cstdint>

#if defined(_MSC_VER)

#include <cstdlib>

#endif

namespace common_lib {
    namespace utilities {

#if defined(_MSC_VER)
#define _BSWAP_CONSTEXPR
#else
#define _BSWAP_CONSTEXPR constexpr
#endif

#if defined(__GNUC__)

        static _BSWAP_CONSTEXPR inline uint16_t bswap(uint16_t value) {
            return __builtin_bswap16(value);
        }

        static _BSWAP_CONSTEXPR inline uint32_t bswap(uint32_t value) {
            return __builtin_bswap32(value);
        }

        static _BSWAP_CONSTEXPR inline uint64_t bswap(uint64_t value) {
            return __builtin_bswap64(value);
        }

#elif defined(_MSC_VER)

        static _BSWAP_CONSTEXPR inline uint16_t bswap(uint16_t value) {
            return _byteswap_ushort(value);
        }

        static _BSWAP_CONSTEXPR inline uint32_t bswap(uint32_t value) {
            return _byteswap_ulong(value);
        }

        static _BSWAP_CONSTEXPR inline uint64_t bswap(uint64_t value) {
            return _byteswap_uint64(value);
        }

#else

        // A neat one
        // https://mklimenko.github.io/english/2018/08/22/robust-endian-swap/

        static _BSWAP_CONSTEXPR inline uint16_t bswap(uint16_t value) {
            return (value >> 8) | (value << 8);
        }

        static _BSWAP_CONSTEXPR inline uint32_t bswap(uint32_t value) {
            uint32_t tmp = ((value << 8) & 0xff00ff00) | ((value >> 8) & 0x00ff00ff);
            return (tmp << 16) | (tmp >> 16);
        }

        static _BSWAP_CONSTEXPR inline uint64_t bswap(uint64_t value) {
            uint64_t tmp = ((value & 0x00000000ffffffffull) << 32) | ((value & 0xffffffff00000000ull) >> 32);
            tmp = ((tmp & 0x0000ffff0000ffffull) << 16) | ((tmp & 0xffff0000ffff0000ull) >> 16);
            tmp = ((tmp & 0x00ff00ff00ff00ffull) << 8) | ((tmp & 0xff00ff00ff00ff00ull) >> 8);
            return tmp;
        }

#endif

// reinterpret_cast<> cannot be used because two's complement is not guaranteed.
// Why can static_cast<> be used here: https://stackoverflow.com/a/14623406 (tested on Clang, GCC, MSVC)
#define _DEFINE_EXTRA_BSWAP_INT(from_type, to_type) \
    static _BSWAP_CONSTEXPR inline to_type bswap(to_type value) { \
        auto v = static_cast<from_type>(value); \
        v = bswap(v); \
        return static_cast<to_type>(v); \
    }

#define _DEFINE_EXTRA_BSWAP_FLOAT(from_type, to_type) \
    static _BSWAP_CONSTEXPR inline to_type bswap(to_type value) { \
        auto v = *reinterpret_cast<const from_type *>(&value); \
        v = bswap(v); \
        return *reinterpret_cast<const to_type *>(&v); \
    }

        _DEFINE_EXTRA_BSWAP_INT(uint16_t, int16_t)

        _DEFINE_EXTRA_BSWAP_INT(uint32_t, int32_t)

        _DEFINE_EXTRA_BSWAP_INT(uint64_t, int64_t)

        _DEFINE_EXTRA_BSWAP_FLOAT(uint32_t, float)

        _DEFINE_EXTRA_BSWAP_FLOAT(uint64_t, double)

#undef _BSWAP_CONSTEXPR
#undef _DEFINE_EXTRA_BSWAP_INT
#undef _DEFINE_EXTRA_BSWAP_FLOAT

    }
}
