#pragma once

#include <memory>

#include "../../common/utilities/_cxx17.h"

namespace vgaudio {
    namespace formats {
        struct IAudioFormat;
        struct AudioData;
    }
}

namespace vgaudio {
    namespace containers {

        struct Configuration;

        struct AudioWithConfig final {

        private:

            std::shared_ptr<vgaudio::formats::IAudioFormat> _audioFormat;
            std::shared_ptr<Configuration> _configuration;

        public:

            AudioWithConfig(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audioFormat, const std::shared_ptr<Configuration> &config);

            ~AudioWithConfig() = default;

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<vgaudio::formats::IAudioFormat> getAudioFormat() const;

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<Configuration> getConfiguration() const;

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<vgaudio::formats::AudioData> getAudio() const;

        };

    }
}
