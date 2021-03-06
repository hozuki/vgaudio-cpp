#pragma once

#include <cstdint>

#include "../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace utilities {

        struct Crc16 final {

        private:

            common_lib::utilities::narray_ptr<uint16_t, 256> _table;

        public:

            explicit Crc16(uint16_t polynomial);

            ~Crc16() = default;

            uint16_t compute(const common_lib::utilities::array_ptr<uint8_t> &data, size_t size);

            template<size_t N>
            uint16_t compute(const common_lib::utilities::narray_ptr<uint8_t, N> &data, size_t size);

            uint16_t compute(const uint8_t *data, size_t size);

        };

    }
}
