#include <cassert>

#include "../../../common/utilities/err.h"
#include "../../../common/utilities/defer.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/BitHelper.h"
#include "../../../common/io/MemoryStream.h"
#include "../../../common/io/BinaryReaderLE.h"
#include "../../../common/io/compression/DeflateStream.h"
#include "ArrayUnpackerStatic.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace common_lib::io::compression;
using namespace vgaudio::utilities;

using TypeIndex = ArrayUnpacker::TypeIndex;
template<typename T = void>
using ArrayWrapper = ArrayUnpacker::ArrayWrapper<T>;
using TArrayPack = std::tuple<TypeIndex, ArrayWrapper<>>;
#define NULL_ARRAY std::make_tuple(ArrayUnpacker::TypeIndex::Unknown, ArrayWrapper<>())

namespace vgaudio::utilities {

    template<typename T>
    void ReleaseWrappedArray(ArrayWrapper<T> &wrapper) {
        if (wrapper.getRank() <= 1) {
            delete[] wrapper.getTypedPointer();
        } else {
            const auto length = wrapper.getLength();
            auto wrappers = reinterpret_cast<ArrayWrapper<T> *>(wrapper.getPointer());

            for (auto i = 0; i < length; i += 1) {
                ReleaseWrappedArray(wrappers[i]);
            }

            delete[] wrappers;
        }
    }

    template<typename T>
    void ReleaseWrappedArray(ArrayWrapper<> &wrapper) {
        ArrayWrapper<T> w(wrapper);
        return ReleaseWrappedArray<T>(w);
    }

    void ArrayUnpacker_ReleaseWrappedArray(TypeIndex typeIndex, ArrayWrapper<> wrapper) {
        switch (typeIndex) {
            case TypeIndex::UInt8:
                ReleaseWrappedArray<uint8_t>(wrapper);
                break;
            case TypeIndex::Int8:
                ReleaseWrappedArray<int8_t>(wrapper);
                break;
            case TypeIndex::Char:
                throw std::runtime_error("not implemented");
            case TypeIndex::Int16:
                ReleaseWrappedArray<int16_t>(wrapper);
                break;
            case TypeIndex::UInt16:
                ReleaseWrappedArray<uint16_t>(wrapper);
                break;
            case TypeIndex::Int32:
                ReleaseWrappedArray<int32_t>(wrapper);
                break;
            case TypeIndex::UInt32:
                ReleaseWrappedArray<uint32_t>(wrapper);
                break;
            case TypeIndex::Int64:
                ReleaseWrappedArray<int64_t>(wrapper);
                break;
            case TypeIndex::UInt64:
                ReleaseWrappedArray<uint64_t>(wrapper);
                break;
            case TypeIndex::Single:
                ReleaseWrappedArray<float>(wrapper);
                break;
            case TypeIndex::Double:
                ReleaseWrappedArray<double>(wrapper);
                break;
            default:
                throw std::out_of_range("invalid type index");
        }
    }

    size_t ArrayUnpacker_SizeOf(TypeIndex type) {
        switch (type) {
            case TypeIndex::UInt8:
            case TypeIndex::Int8:
                return 1;
            case TypeIndex::Char:
            case TypeIndex::Int16:
            case TypeIndex::UInt16:
                return 2;
            case TypeIndex::Int32:
            case TypeIndex::UInt32:
                return 4;
            case TypeIndex::Int64:
            case TypeIndex::UInt64:
                return 8;
            case TypeIndex::Single:
                return 4;
            case TypeIndex::Double:
                return 8;
            default:
                return 0;
        }
    }

#define CAST_ADDR(src_type, dst_type) \
    *static_cast<dst_type *>(p2) = *static_cast<const src_type *>(p1)

#define CAST_BRANCHES(src_type) \
    do { \
        switch (destinationType) { \
            case TypeIndex::Unknown: \
                throw std::out_of_range("not possible" " @" ERROR_SITE_DESCRIPTION); \
            case TypeIndex::UInt8: \
                CAST_ADDR(src_type, uint8_t); \
                break; \
            case TypeIndex::Int8: \
                CAST_ADDR(src_type, int8_t); \
                break; \
            case TypeIndex::Char: \
                throw std::runtime_error("not implemented" " @" ERROR_SITE_DESCRIPTION); \
            case TypeIndex::Int16: \
                CAST_ADDR(src_type, int16_t); \
                break; \
            case TypeIndex::UInt16: \
                CAST_ADDR(src_type, uint16_t); \
                break; \
            case TypeIndex::Int32: \
                CAST_ADDR(src_type, int32_t); \
                break; \
            case TypeIndex::UInt32: \
                CAST_ADDR(src_type, uint32_t); \
                break; \
            case TypeIndex::Int64: \
                CAST_ADDR(src_type, int64_t); \
                break; \
            case TypeIndex::UInt64: \
                CAST_ADDR(src_type, uint64_t); \
                break; \
            case TypeIndex::Single: \
                CAST_ADDR(src_type, float); \
                break; \
            case TypeIndex::Double: \
                CAST_ADDR(src_type, double); \
                break; \
        } \
    } while (false)

    void ArrayUnpacker_CastArray(const void *source, TypeIndex sourceType, void *destination, TypeIndex destinationType, size_t length) {
        const auto srcStep = ArrayUnpacker_SizeOf(sourceType);
        const auto dstStep = ArrayUnpacker_SizeOf(destinationType);

        if (sourceType == TypeIndex::Unknown || destinationType == TypeIndex::Unknown) {
            throw std::out_of_range("invalid array element type conversion" " @" ERROR_SITE_DESCRIPTION);
        }

        for (auto i = 0; i < length; i += 1) {
            const void *p1 = static_cast<const uint8_t *>(source) + srcStep * i;
            void *p2 = static_cast<uint8_t *>(destination) + dstStep * i;

            switch (sourceType) {
                case TypeIndex::Unknown:
                    throw std::out_of_range("not possible" " @" ERROR_SITE_DESCRIPTION);
                case TypeIndex::UInt8:
                    CAST_BRANCHES(uint8_t);
                    break;
                case TypeIndex::Int8:
                    CAST_BRANCHES(int8_t);
                    break;
                case TypeIndex::Char:
                    throw std::runtime_error("not implemented" " @" ERROR_SITE_DESCRIPTION);
                case TypeIndex::Int16:
                    CAST_BRANCHES(int16_t);
                    break;
                case TypeIndex::UInt16:
                    CAST_BRANCHES(uint16_t);
                    break;
                case TypeIndex::Int32:
                    CAST_BRANCHES(int32_t);
                    break;
                case TypeIndex::UInt32:
                    CAST_BRANCHES(uint32_t);
                    break;
                case TypeIndex::Int64:
                    CAST_BRANCHES(int64_t);
                    break;
                case TypeIndex::UInt64:
                    CAST_BRANCHES(uint64_t);
                    break;
                case TypeIndex::Single:
                    CAST_BRANCHES(float);
                    break;
                case TypeIndex::Double:
                    CAST_BRANCHES(double);
                    break;
            }
        }
    }

    void *ArrayUnpacker_Inflate(const void *compressed, unsigned long compressedSize, int32_t offset, size_t decompressedSize) {
        auto result = malloc(decompressedSize);
        memset(result, 0, decompressedSize);

        auto buffer = make_array_dynamic<uint8_t>(static_cast<const uint8_t *>(compressed), compressedSize, false, false);
        auto inMemory = make_shared<MemoryStream>(buffer, false);

        inMemory->setPosition(offset);

        {
            auto deflate = make_shared<DeflateStream>(inMemory, CompressionMode::Decompress);

            const auto read = deflate->read(result, decompressedSize, 0, decompressedSize);

            assert(read == decompressedSize);
        }

        return result;
    }

    void *ArrayUnpacker_TryDecompress(const void *data, size_t dataSize, size_t *resultSize) {
        assert(dataSize > 0);

        bool isCompressed = static_cast<const uint8_t *>(data)[0] == 1;
        void *result;

        if (isCompressed) {
            assert(dataSize > 5);

            // TODO: endian?
            int32_t decompressedSize = *reinterpret_cast<const int32_t *>(reinterpret_cast<const uint8_t *>(data) + 1);

            result = ArrayUnpacker_Inflate(data, dataSize, 5, decompressedSize);

            if (resultSize != nullptr) {
                *resultSize = decompressedSize;
            }
        } else {
            result = malloc(dataSize);
            memcpy(result, data, dataSize);

            if (resultSize != nullptr) {
                *resultSize = dataSize;
            }
        }

        return result;
    }

    TArrayPack ArrayUnpacker_ReadArray(const shared_ptr<BinaryReader> &reader, TypeIndex storedType, TypeIndex outType, int32_t length) {
        if (length >= UINT16_MAX) {
            return NULL_ARRAY;
        }

        int32_t sizeInBytes = length * ArrayUnpacker_SizeOf(storedType);

        auto bytes = reader->readBytes(sizeInBytes);

        assert(bytes->size() == sizeInBytes);

        void *data;

        if (storedType == outType) {
            data = malloc(sizeInBytes);
            memcpy(data, bytes->data(), sizeInBytes);
        } else {
            auto tmp = malloc(sizeInBytes);
            memcpy(tmp, bytes->data(), sizeInBytes);

            auto_defer([&]() {
                free(tmp);
            });

            int32_t convertedSizeInBytes = length * ArrayUnpacker_SizeOf(outType);
            data = malloc(convertedSizeInBytes);

            ArrayUnpacker_CastArray(tmp, storedType, data, outType, length);
        }

        ArrayWrapper<> w(data, length, 1);

        return std::make_tuple(outType, w);
    }

    TArrayPack ArrayUnpacker_UnpackArray(const shared_ptr<BinaryReader> &reader, TypeIndex outType, int32_t rank) {
        uint8_t modeType = reader->readUInt8();

        if (modeType == 0xff) {
            return NULL_ARRAY;
        }

        uint8_t mode = BitHelper::getHighNibble(modeType);
        auto storedType = static_cast<TypeIndex >(BitHelper::getLowNibble(modeType));

        switch (mode) {
            case 0: {
                int32_t length = reader->readUInt16();

                if (rank == 1) {
                    return ArrayUnpacker_ReadArray(reader, storedType, outType, length);
                }

                auto array = new ArrayWrapper<>[length];

                for (auto i = 0; i < length; i += 1) {
                    auto[_, w] = ArrayUnpacker_UnpackArray(reader, outType, rank - 1);
                    array[i] = w;
                }

                ArrayWrapper<> wrapper(array, length, rank);

                return std::make_tuple(outType, wrapper);
            }
            case 1: {
                auto dimensions = make_array_dynamic<int32_t>(rank);

                for (auto d = 0; d < rank; d += 1) {
                    (*dimensions)[d] = reader->readUInt16();
                }

                return ArrayUnpacker_UnpackInternal(outType, storedType, reader, 0, dimensions);
            }
            case 2: {
                int32_t length = reader->readUInt16();
                auto lengths = new int32_t[length];

                auto_defer([&]() {
                    delete[] lengths;
                });

                for (auto i = 0; i < length; i += 1) {
                    lengths[i] = reader->readUInt16();
                }

                auto array = new ArrayWrapper<>[length];

                for (auto i = 0; i < length; i += 1) {
                    auto[_, w] = ArrayUnpacker_ReadArray(reader, storedType, outType, lengths[i]);
                    array[i] = w;
                }

                ArrayWrapper<> wrapper(array, length, 2);

                return std::make_tuple(outType, wrapper);
            }
            default:
                throw std::out_of_range("invalid unpack mode");
        }
    }

    TArrayPack ArrayUnpacker_UnpackInternal(TypeIndex outType, TypeIndex storedType, const shared_ptr<BinaryReader> &reader, int32_t depth, const array_ptr<int32_t> &dimensions) {
        const auto dimensionCount = dimensions->size();

        if (depth >= dimensionCount) {
            return NULL_ARRAY;
        }

        if (depth == dimensionCount - 1) {
            return ArrayUnpacker_ReadArray(reader, storedType, outType, (*dimensions)[depth]);
        }

        const auto length = (*dimensions)[depth];
        auto array = new ArrayWrapper<>[length];

        for (auto i = 0; i < length; i += 1) {
            auto[_, w] = ArrayUnpacker_UnpackInternal(outType, storedType, reader, depth + 1, dimensions);
            array[i] = w;
        }

        ArrayWrapper<> wrapper(array, length, dimensionCount - depth);

        return std::make_tuple(outType, wrapper);
    }

}
