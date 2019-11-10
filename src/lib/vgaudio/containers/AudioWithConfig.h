#pragma once

#include <memory>

namespace vgaudio::formats {
    struct IAudioFormat;
    struct AudioData;
}

namespace vgaudio::containers {

    struct Configuration;

    using namespace std;
    using namespace vgaudio::formats;

    struct AudioWithConfig final {

    private:

        shared_ptr<IAudioFormat> _audioFormat;
        shared_ptr<Configuration> _configuration;

    public:

        AudioWithConfig(const shared_ptr<IAudioFormat> &audioFormat, const shared_ptr<Configuration> &config);

        ~AudioWithConfig() = default;

        [[nodiscard]]
        shared_ptr<IAudioFormat> getAudioFormat() const;

        [[nodiscard]]
        shared_ptr<Configuration> getConfiguration() const;

        [[nodiscard]]
        shared_ptr<AudioData> getAudio() const;

    };

}
