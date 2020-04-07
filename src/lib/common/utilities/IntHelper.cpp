#include <cmath>
#include <cassert>
#include <vector>

#include "IntHelper.h"
#include "ArrayHelper.h"

using namespace common_lib::utilities;

static int8_t MapUnsignedNibbleToSignedNibble(uint8_t b) {
    assert(b < 16);

    if (b < 8) {
        return static_cast<int8_t>(static_cast<int32_t>(b));
    } else {
        return static_cast<int8_t>(static_cast<int32_t>(b) - 16);
    }
}

int32_t IntHelper::divideByRoundUp(int32_t value, int32_t divisor) {
    return (int32_t) std::ceil(static_cast<double>(value) / divisor);
}

uint8_t IntHelper::combineNibbles(uint8_t high, uint8_t low) {
    uint32_t h = high, l = low;
    return static_cast<uint8_t>((h << 4u) | l);
}

int32_t IntHelper::getNextMultiple(int32_t value, int32_t multiple) {
    if (multiple <= 0) {
        return value;
    }

    if (value % multiple == 0) {
        return value;
    }

    return value + multiple - value % multiple;
}

uint8_t IntHelper::getHighNibble(uint8_t value) {
    return (value >> 4u) & 0x0fu;
}

uint8_t IntHelper::getLowNibble(uint8_t value) {
    return value & 0xfu;
}

int8_t IntHelper::getHighNibbleSigned(uint8_t value) {
    return MapUnsignedNibbleToSignedNibble(getHighNibble(value));
}

int8_t IntHelper::getLowNibbleSigned(int8_t value) {
    return MapUnsignedNibbleToSignedNibble(getLowNibble(value));
}

array_ptr<int32_t> IntHelper::getPrimes(int32_t maxPrime) {
    assert(maxPrime >= 2);

    // TODO: optimize: use bitset
    const auto maxSieveCount = maxPrime / 2;
    const int32_t maxSieve = std::sqrt(maxPrime);
    const auto sieve = make_array_dynamic<uint8_t>(maxSieveCount);

    for (auto i = 3; i <= maxSieve; i += 2) {
        if ((*sieve)[i >> 1] != 0) {
            continue;
        }

        for (auto j = i * i; j < maxPrime; j += i * 2) {
            (*sieve)[j >> 1] = 1;
        }
    }

    std::vector<int32_t> primes;

    primes.push_back(2);

    for (auto i = 1; i < maxSieveCount; i += 1) {
        if ((*sieve)[i] == 0) {
            primes.push_back(i * 2 + 1);
        }
    }

    return ArrayHelper::toArray(primes);
}
