#pragma once

#include <cstdint>

#include "../../common/utilities/_cxx17.h"
#include "../../common/utilities/runtime_array.h"
#include "OffsetBias.h"

namespace vgaudio {
    namespace utilities {

        struct BitReader final {

        private:

            common_lib::utilities::array_ptr<uint8_t> _buffer;
            size_t _position;

        public:

            explicit BitReader(const common_lib::utilities::array_ptr<uint8_t> &buffer);

            ~BitReader() = default;

            void SetBuffer(const common_lib::utilities::array_ptr<uint8_t> &buffer);

            _CXX17_ATTR_NODISCARD
            common_lib::utilities::array_ptr<uint8_t> getBuffer() const;

            _CXX17_ATTR_NODISCARD
            size_t getLengthBits() const;

            void setPosition(size_t position);

            _CXX17_ATTR_NODISCARD
            size_t getPosition() const;

            _CXX17_ATTR_NODISCARD
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
}
