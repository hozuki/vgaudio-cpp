#pragma once

#include <memory>

namespace vgaudio::formats {
    struct IAudioFormat;
    struct AudioData;
}

namespace vgaudio::containers {

    struct Configuration;

    struct AudioWithConfig final {

    private:

        std::shared_ptr<vgaudio::formats::IAudioFormat> _audioFormat;
        std::shared_ptr<Configuration> _configuration;

    public:

        AudioWithConfig(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audioFormat, const std::shared_ptr<Configuration> &config);

        ~AudioWithConfig() = default;

        [[nodiscard]]
        std::shared_ptr<vgaudio::formats::IAudioFormat> getAudioFormat() const;

        [[nodiscard]]
        std::shared_ptr<Configuration> getConfiguration() const;

        [[nodiscard]]
        std::shared_ptr<vgaudio::formats::AudioData> getAudio() const;

    };

}
