#pragma once

#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace common_lib {
    namespace io {
        struct Stream;
    }
}

namespace vgaudio {
    namespace formats {
        struct AudioData;
        struct IAudioFormat;
    }
}

namespace vgaudio {
    namespace containers {

        struct AudioWithConfig;

        struct IAudioReader {

            virtual ~IAudioReader() = default;

            virtual std::shared_ptr<vgaudio::formats::IAudioFormat> readFormat(const std::shared_ptr<common_lib::io::Stream> &stream) = 0;

            virtual std::shared_ptr<vgaudio::formats::IAudioFormat> readFormat(const common_lib::utilities::array_ptr<uint8_t> &file);

            virtual std::shared_ptr<vgaudio::formats::AudioData> read(const std::shared_ptr<common_lib::io::Stream> &stream) = 0;

            virtual std::shared_ptr<vgaudio::formats::AudioData> read(const common_lib::utilities::array_ptr<uint8_t> &file);

            virtual std::shared_ptr<AudioWithConfig> readWithConfig(const std::shared_ptr<common_lib::io::Stream> &stream) = 0;

            virtual std::shared_ptr<AudioWithConfig> readWithConfig(const common_lib::utilities::array_ptr<uint8_t> &file);

        protected:

            IAudioReader() = default;

        };

    }
}
