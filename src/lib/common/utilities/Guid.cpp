#include <cstring>
#include <cinttypes>

#include "Guid.h"

using namespace std;
using namespace common_lib::utilities;

static inline size_t HashCombine(size_t seed, size_t h) {
    return seed ^ (h + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

Guid::Guid() noexcept {
    memset(this, 0, sizeof(*this));
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint64_t d4) noexcept
    : data1(d1), data2(d2), data3(d3) {
    memcpy(data4, &d4, 8);
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, const uint8_t d5[6])
    : data1(d1), data2(d2), data3(d3) {
    *const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(data4)) = d4;
    memcpy(data4 + 2, d5, 6);
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, const uint8_t d4[8])
    : data1(d1), data2(d2), data3(d3) {
    memcpy(data4, d4, 8);
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint16_t d5l, uint32_t d5h) noexcept
    : data1(d1), data2(d2), data3(d3) {
    *const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(data4)) = d4;
    uint64_t d5 = static_cast<uint64_t>(d5l) | (static_cast<uint64_t>(d5h) << 16u);
    memcpy(data4 + 2, &d5, 6);
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint32_t d5l, uint16_t d5h) noexcept
    : data1(d1), data2(d2), data3(d3) {
    *const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(data4)) = d4;
    uint64_t d5 = static_cast<uint64_t>(d5l) | (static_cast<uint64_t>(d5h) << 32u);
    memcpy(data4 + 2, &d5, 6);
}

Guid::Guid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint64_t d5) noexcept
    : data1(d1), data2(d2), data3(d3) {
    *const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(data4)) = d4;
    memcpy(data4 + 2, &d5, 6);
}

Guid::Guid(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4) noexcept {
    auto *p = const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(this));
    p[0] = d1;
    p[1] = d2;
    p[2] = d3;
    p[3] = d4;
}

Guid::Guid(const uint8_t data[16]) {
    memcpy(this, data, 16);
}

bool Guid::operator==(const Guid &other) {
    return data1 == other.data1 && data2 == other.data2 && data3 == other.data3 && memcmp(data4, other.data4, 4) == 0;
}

bool Guid::operator!=(const Guid &other) {
    return !(*this == other);
}

std::size_t Guid::getHashCode() const {
    size_t hash = 0;
    hash = HashCombine(hash, data1);
    hash = HashCombine(hash, static_cast<size_t>(data2 << 16u) | data3);
    hash = HashCombine(hash, *reinterpret_cast<const uint32_t *>(data4));
    hash = HashCombine(hash, *reinterpret_cast<const uint32_t *>(data4 + 4));
    return hash;
}

std::string Guid::toString() const {
    char buf[37] = {0};

    const uint32_t part1 = data1;
    const uint16_t part2 = data2;
    const uint16_t part3 = data3;
    const uint16_t part4 = *reinterpret_cast<const uint16_t *>(data4);

    const uint64_t part5_lo = *reinterpret_cast<const uint16_t *>(data4 + 2);
    const uint64_t part5_hi = *reinterpret_cast<const uint32_t *>(data4 + 4);
    const uint64_t part5 = part5_lo | (part5_hi << 16u);

    sprintf(buf, "%08" PRIx32 "-%04" PRIx16 "-%04" PRIx16 "-%04" PRIx16 "-%012" PRIx64, part1, part2, part3, part4, part5);

    return std::string(buf);
}

namespace common_lib::utilities {

    std::ostream &operator<<(std::ostream &stream, const Guid &guid) {
        return stream << guid.toString();
    }

}
