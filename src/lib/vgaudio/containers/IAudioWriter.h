#pragma once

#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace common_lib::io {
    struct Stream;
}

namespace vgaudio::formats {
    struct AudioData;
    struct IAudioFormat;
}

namespace vgaudio::containers {

    struct Configuration;

    struct IAudioWriter {

        virtual ~IAudioWriter() = default;

        virtual void writeToStream(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) = 0;

        virtual common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config);

        virtual void writeToStream(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) = 0;

        virtual common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config);

    protected:

        IAudioWriter() = default;

    };

}
