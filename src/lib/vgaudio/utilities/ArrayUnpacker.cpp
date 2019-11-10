#include <cassert>

#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/BitHelper.h"
#include "../../common/utilities/defer.h"
#include "../../common/io/MemoryStream.h"
#include "../../common/io/BinaryReaderLE.h"
#include "private/ArrayUnpackerStatic.h"
#include "ArrayUnpacker.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities;

using TypeIndex = ArrayUnpacker::TypeIndex;
template<typename T = void>
using ArrayWrapper = ArrayUnpacker::ArrayWrapper<T>;
using TArrayPack = std::tuple<TypeIndex, ArrayWrapper<>>;

array_ptr<TArrayPack> ArrayUnpacker::unpackArray(const void *packedArrays, size_t size) {
    size_t decompressedPackedArraysSize = 0;
    auto decompressedPackedArrays = ArrayUnpacker_TryDecompress(packedArrays, size, &decompressedPackedArraysSize);

    auto_defer([&]() {
        free(decompressedPackedArrays);
    });

    auto packedArraysBuffer = make_array_dynamic<uint8_t>(static_cast<uint8_t *>(decompressedPackedArrays), decompressedPackedArraysSize, true, true);
    auto stream = make_shared<MemoryStream>(packedArraysBuffer, false);
    auto reader = make_shared<BinaryReaderLE>(stream);

    int32_t isCompressed = reader->readUInt8();
    int32_t version = reader->readUInt8();

    if (isCompressed != 0 || version != 0) {
        throw std::runtime_error("data error");
    }

    int32_t count = reader->readUInt16();

    auto result = make_array_dynamic<TArrayPack>(count);

    for (auto i = 0; i < count; i += 1) {
        uint8_t id = reader->readUInt8();
        uint8_t type = reader->readUInt8();
        auto outType = static_cast<TypeIndex>(BitHelper::getHighNibble(type));
        uint8_t rank = BitHelper::getLowNibble(type);

        auto a = ArrayUnpacker_UnpackArray(reader, outType, rank);

        (*result)[id] = a;
    }

    return result;
}

void ArrayUnpacker::releaseWrappedArray(TypeIndex typeIndex, ArrayWrapper<> wrapper) {
    ArrayUnpacker_ReleaseWrappedArray(typeIndex, wrapper);
}
