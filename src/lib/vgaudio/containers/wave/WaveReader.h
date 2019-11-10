#pragma once

#include "../AudioReader.h"
#include "WaveStructure.h"
#include "WaveConfiguration.h"

namespace vgaudio::utilities::riff {
    struct RiffParser;
}

namespace vgaudio::containers::wave {

    using namespace std;
    using namespace vgaudio::containers;
    using namespace vgaudio::utilities::riff;

    struct WaveReader : public AudioReader<WaveStructure, WaveConfiguration> {

        DECLARE_CLASS_TEMPLATE(WaveReader, AudioReader, WaveStructure, WaveConfiguration);

    public:

        WaveReader() = default;

        ~WaveReader() override = default;

    protected:

        shared_ptr<WaveStructure> readFile(const shared_ptr<Stream> &stream, bool readAudioData) override;

        shared_ptr<IAudioFormat> toAudioStream(const shared_ptr<WaveStructure> &structure) override;

    private:

        static void validateWaveFile(const shared_ptr<RiffParser> &parser) noexcept(false);

    };

}
