#pragma once

#include <cstdint>

#include "../../common/utilities/runtime_array.h"
#include "OffsetBias.h"

namespace vgaudio::utilities {

    using namespace common_lib::utilities;

    struct BitReader final {

    private:

        array_ptr<uint8_t> _buffer;
        size_t _position;

    public:

        explicit BitReader(const array_ptr<uint8_t> &buffer);

        ~BitReader() = default;

        void SetBuffer(const array_ptr<uint8_t> &buffer);

        [[nodiscard]]
        array_ptr<uint8_t> getBuffer() const;

        [[nodiscard]]
        size_t getLengthBits() const;

        void setPosition(size_t position);

        [[nodiscard]]
        size_t getPosition() const;

        [[nodiscard]]
        size_t getRemaining() const;

        int32_t readInt(int32_t bitCount);

        int32_t readSignedInt(int32_t bitCount);

        bool readBool();

        int32_t readOffsetBinary(int32_t bitCount, OffsetBias bias);

        void alignPosition(int32_t multiple);

        int32_t PeekInt(int32_t bitCount);

    private:

        int32_t PeekIntFallback(int32_t bitCount);

    };

}
