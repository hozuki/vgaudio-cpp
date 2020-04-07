#pragma once

#include <cstdint>

#include "BinaryReader.h"

namespace common_lib::io {

    struct BinaryReaderLE : public BinaryReader {

        DECLARE_CLASS(BinaryReaderLE, BinaryReader);

    public:

        explicit BinaryReaderLE(const std::shared_ptr<Stream> &stream);

        ~BinaryReaderLE() override = default;

        uint16_t readUInt16() override;

        int16_t readInt16() override;

        uint32_t readUInt32() override;

        int32_t readInt32() override;

        uint64_t readUInt64() override;

        int64_t readInt64() override;

        float readSingle() override;

        double readDouble() override;

    };

}
