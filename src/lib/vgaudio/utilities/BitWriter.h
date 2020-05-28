#pragma once

#include <cstdint>

#include "../../common/utilities/_cxx17.h"
#include "../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace utilities {

        struct BitWriter final {

        private:

            common_lib::utilities::array_ptr<uint8_t> _buffer;
            size_t _position;

        public:

            explicit BitWriter(const common_lib::utilities::array_ptr<uint8_t> &buffer);

            ~BitWriter() = default;

            void SetBuffer(const common_lib::utilities::array_ptr<uint8_t> &buffer, size_t position = 0);

            _CXX17_ATTR_NODISCARD
            common_lib::utilities::array_ptr<uint8_t> getBuffer() const;

            _CXX17_ATTR_NODISCARD
            size_t getLengthBits() const;

            void setPosition(size_t position);

            _CXX17_ATTR_NODISCARD
            size_t getPosition() const;

            _CXX17_ATTR_NODISCARD
            size_t getRemaining() const;

            void alignPosition(int32_t multiple);

            void write(int32_t value, int32_t bitCount);

        private:

            void writeFallback(int32_t value, int32_t bitCount);

        };

    }
}
