#pragma

#include <cstdint>

#include "../../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace formats {
        namespace criadx {

            struct CriAdxChannel {

                explicit CriAdxChannel(const common_lib::utilities::array_ptr<uint8_t> &audio, int16_t history = 0, int32_t version = 0);

                common_lib::utilities::array_ptr<uint8_t> audio;

                int16_t history;

                int32_t version;

            };

        }
    }
}
