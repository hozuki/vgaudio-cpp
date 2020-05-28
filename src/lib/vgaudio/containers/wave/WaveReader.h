#pragma once

#include "../AudioReader.h"
#include "WaveStructure.h"
#include "WaveConfiguration.h"

namespace vgaudio {
    namespace utilities {
        namespace riff {
            struct RiffParser;
        }
    }
}

namespace vgaudio {
    namespace containers {
        namespace wave {

            struct WaveReader : public AudioReader<WaveStructure, WaveConfiguration> {

                DECLARE_CLASS_TEMPLATE(WaveReader, AudioReader, WaveStructure, WaveConfiguration);

            public:

                WaveReader() = default;

                ~WaveReader() override = default;

            protected:

                std::shared_ptr<WaveStructure> readFile(const std::shared_ptr<common_lib::io::Stream> &stream, bool readAudioData) override;

                std::shared_ptr<vgaudio::formats::IAudioFormat> toAudioStream(const std::shared_ptr<WaveStructure> &structure) override;

            private:

                static void validateWaveFile(const std::shared_ptr<vgaudio::utilities::riff::RiffParser> &parser) noexcept(false);

            };

        }
    }
}
