#include "CriAdxKey.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/ArrayHelper.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::criadx;

static array_ptr<int32_t> Primes;

struct _CriAdx_Key_Initializer {

    _CriAdx_Key_Initializer() noexcept {
        const auto primes = IntHelper::getPrimes(0x8000);
        const auto start = ~ArrayHelper::binarySearch(primes, 0x4000);

        const auto trimmedPrimes = make_array_dynamic<int32_t>(0x400);
        ArrayHelper::copyByValue(primes, start, trimmedPrimes, 0, 0x400);

        Primes = trimmedPrimes;
    }

};

[[maybe_unused]]
static _CriAdx_Key_Initializer _init;

CriAdxKey::CriAdxKey(int32_t seed, int32_t mul, int32_t inc)
    : _seed(seed), _mul(mul), _inc(inc) {
}

CriAdxKey::CriAdxKey(uint64_t keyCode) {
    keyCode -= 1;

    _seed = static_cast<int32_t>((keyCode >> 27) & 0x7fff);
    _mul = static_cast<int32_t>(((keyCode >> 12) & 0x7ffc) | 1);
    _inc = static_cast<int32_t>(((keyCode << 1) & 0x7fff) | 1);
}

CriAdxKey::CriAdxKey(const std::string &keyString)
    : _seed(0), _mul(0), _inc(0) {
    if (keyString.empty()) {
        return;
    }

    _keyString = keyString;

    _seed = (*Primes)[0x100];
    _mul = (*Primes)[0x200];
    _inc = (*Primes)[0x300];

    for (const auto ch : keyString) {
        _seed = (*Primes)[_seed * (*Primes)[ch + 0x80] % 0x400];
        _mul = (*Primes)[_mul * (*Primes)[ch + 0x80] % 0x400];
        _inc = (*Primes)[_inc * (*Primes)[ch + 0x80] % 0x400];
    }
}

int32_t CriAdxKey::getSeed() const {
    return _seed;
}

int32_t CriAdxKey::getMul() const {
    return _mul;
}

int32_t CriAdxKey::getInc() const {
    return _inc;
}

uint64_t CriAdxKey::getKeyCode() const {
    const auto seed = static_cast<uint64_t>(_seed) << 27u;
    const auto mul = static_cast<uint64_t>(_mul & 0xfffc) << 12u;
    const auto inc = static_cast<uint64_t>(_inc) >> 1u;

    return (seed | mul | inc) + 1;
}

const std::string &CriAdxKey::getKeyString() const {
    return _keyString;
}
