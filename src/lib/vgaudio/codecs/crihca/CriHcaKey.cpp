#include <cinttypes>

#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../../../common/utilities/StrHelper.h"
#include "CriHcaKey.h"

using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

static narray_ptr<uint8_t, 16> CreateRandomRow(uint8_t seed) noexcept {
    auto row = make_array_static<uint8_t, 16>();

    int32_t x = static_cast<uint32_t>(seed) >> 4u;
    int32_t mul = ((seed & 1u) << 3u) | 5u;
    int32_t add = (seed & 0xeu) | 1u;

    for (auto i = 0; i < 16; i += 1) {
        x = (x * mul + add) % 16;
        (*row)[i] = static_cast<uint8_t>(x);
    }

    return row;
}

static jnarray2_ptr<uint8_t, 256, 16> GenerateAllRows() noexcept {
    auto rows = make_jagged_array_2_static<uint8_t, 256, 16>();

    for (auto i = 0; i < 256; i += 1) {
        (*rows)[i] = CreateRandomRow(static_cast<uint8_t>(i));
    }

    return rows;
}

static jnarray2_ptr<uint8_t, 256, 16> Rows = GenerateAllRows();

static narray_ptr<uint8_t, 256> CreateDecryptionTableType0() {
    auto table = make_array_static<uint8_t, 256>();

    for (auto i = 0; i < 256; i += 1) {
        (*table)[i] = static_cast<uint8_t>(i);
    }

    return table;
}

static narray_ptr<uint8_t, 256> CreateDecryptionTableType1() {
    auto table = make_array_static<uint8_t, 256>();

    int32_t x = 0;
    constexpr int32_t mul = 13;
    constexpr int32_t add = 11;
    uint32_t outPos = 1;

    for (auto i = 0; i < 256; i += 1) {
        x = (x * mul + add) % 256;

        if (x != 0 && x != 0xff) {
            (*table)[outPos] = static_cast<uint8_t>(x);
            outPos += 1;
        }
    }

    (*table)[0] = 0;
    (*table)[0xff] = 0xff;

    return table;
}

static narray_ptr<uint8_t, 256> ShuffleTable(narray_ptr<uint8_t, 256> tableIn) {
    auto table = make_array_static<uint8_t, 256>();

    uint8_t x = 0;
    uint32_t outPos = 1;

    for (auto i = 0; i < 256; i += 1) {
        x += 17;

        if ((*table)[x] != 0 && (*tableIn)[x] != 0xff) {
            (*table)[outPos] = (*tableIn)[x];
        }
    }

    (*table)[0] = 0;
    (*table)[0xff] = 0xff;

    return table;
}

static narray_ptr<uint8_t, 256> CreateTable(uint8_t rowSeed, narray_ptr<uint8_t, 16> columnSeeds) {
    auto table = make_array_static<uint8_t, 256>();
    auto row = (*Rows)[rowSeed];

    for (auto r = 0; r < 16; r += 1) {
        auto column = (*Rows)[(*columnSeeds)[r]];

        for (auto c = 0; c < 16; c += 1) {
            (*table)[r * 16 + c] = IntHelper::combineNibbles((*row)[r], (*column)[c]);
        }
    }

    auto shuffled = ShuffleTable(table);

    return shuffled;
}

static narray_ptr<uint8_t, 256> InvertTable(narray_ptr<uint8_t, 256> tableIn) {
    constexpr uint32_t length = 256;

    auto table = make_array_static<uint8_t, length>();

    for (auto i = 0; i < length; i += 1) {
        (*table)[(*tableIn)[i]] = static_cast<uint8_t>(i);
    }

    return table;
}

static narray_ptr<uint8_t, 256> CreateDecryptionTable(uint64_t keyCode) {
    const uint64_t code = keyCode - 1;
    const auto *kc = reinterpret_cast<const uint8_t *>(&code);
    auto seed = make_array_static<uint8_t, 16>();

    (*seed)[0] = kc[1];
    (*seed)[1] = kc[6] ^ kc[1];
    (*seed)[2] = kc[2] ^ kc[3];
    (*seed)[3] = kc[2];
    (*seed)[4] = kc[1] ^ kc[2];
    (*seed)[5] = kc[3] ^ kc[4];
    (*seed)[6] = kc[3];
    (*seed)[7] = kc[2] ^ kc[3];
    (*seed)[8] = kc[4] ^ kc[5];
    (*seed)[9] = kc[4];
    (*seed)[10] = kc[3] ^ kc[4];
    (*seed)[11] = kc[5] ^ kc[6];
    (*seed)[12] = kc[5];
    (*seed)[13] = kc[4] ^ kc[5];
    (*seed)[14] = kc[6] ^ kc[1];
    (*seed)[15] = kc[6];

    return CreateTable(kc[0], seed);
}

CriHcaKeyType CriHcaKey::getKeyType() const {
    return _keyType;
}

uint64_t CriHcaKey::getKeyCode(uint64_t *pKey) const {
    if (pKey != nullptr) {
        *pKey = _keyCode;
    }

    return _keyCode;
}

void CriHcaKey::getKeyCode(uint64_t &key) const {
    key = _keyCode;
}

void CriHcaKey::getKeyCode(uint32_t &key1, uint32_t &key2) const {
    key1 = *reinterpret_cast<const uint32_t *>(&_keyCode);
    key2 = *(reinterpret_cast<const uint32_t *>(&_keyCode) + 1);
}

narray_ptr<uint8_t, 256> CriHcaKey::getDecryptionTable() const {
    return _decryptionTable;
}

narray_ptr<uint8_t, 256> CriHcaKey::getEncryptionTable() const {
    return _encryptionTable;
}

CriHcaKey::CriHcaKey(CriHcaKeyType type) {
    switch (type) {
        case CriHcaKeyType::Type0:
            _decryptionTable = CreateDecryptionTableType0();
            break;
        case CriHcaKeyType::Type1:
            _decryptionTable = CreateDecryptionTableType1();
            break;
        default:
            throw invalid_argument(StrHelper::format("unsupported key type for this constructor: %" PRId32, static_cast<int32_t>(type)));
    }

    _keyCode = 0;
    _keyType = type;
    _encryptionTable = InvertTable(getDecryptionTable());
}

CriHcaKey::CriHcaKey(uint64_t keyCode) {
    if (keyCode == 0) {
        _decryptionTable = CreateDecryptionTableType0();
        _keyType = CriHcaKeyType::Type0;
        _keyCode = 0;
    } else {
        _decryptionTable = CreateDecryptionTable(keyCode);
        _keyType = CriHcaKeyType::Type56;
        _keyCode = keyCode;
    }

    _encryptionTable = InvertTable(getDecryptionTable());
}
