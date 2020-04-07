#pragma once

#include "../ArrayUnpacker.h"

namespace common_lib::io {
    struct BinaryReader;
}

namespace vgaudio::utilities {

    using namespace std;
    using namespace common_lib::io;
    using namespace common_lib::utilities;

    void ArrayUnpacker_ReleaseWrappedArray(ArrayUnpacker::TypeIndex typeIndex, ArrayUnpacker::ArrayWrapper<> wrapper);

    size_t ArrayUnpacker_SizeOf(ArrayUnpacker::TypeIndex type);

    void ArrayUnpacker_CastArray(const void *source, ArrayUnpacker::TypeIndex sourceType, void *destination, ArrayUnpacker::TypeIndex destinationType, size_t length);

    void *ArrayUnpacker_Inflate(const void *compressed, unsigned long compressedSize, int32_t offset, size_t decompressedSize);

    void *ArrayUnpacker_TryDecompress(const void *data, size_t dataSize, size_t *resultSize);

    std::tuple<ArrayUnpacker::TypeIndex, ArrayUnpacker::ArrayWrapper<>>
    ArrayUnpacker_ReadArray(const shared_ptr<BinaryReader> &reader, ArrayUnpacker::TypeIndex storedType, ArrayUnpacker::TypeIndex outType, int32_t length);

    std::tuple<ArrayUnpacker::TypeIndex, ArrayUnpacker::ArrayWrapper<>>
    ArrayUnpacker_UnpackArray(const shared_ptr<BinaryReader> &reader, ArrayUnpacker::TypeIndex outType, int32_t rank);

    std::tuple<ArrayUnpacker::TypeIndex, ArrayUnpacker::ArrayWrapper<>>
    ArrayUnpacker_UnpackInternal(ArrayUnpacker::TypeIndex outType, ArrayUnpacker::TypeIndex storedType, const shared_ptr<BinaryReader> &reader, int32_t depth, const array_ptr<int32_t> &dimensions);

}
