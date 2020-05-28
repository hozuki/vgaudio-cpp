#pragma once

#include <cstdint>

#include "../../../common/utilities/_cxx17.h"
#include "../AudioWriter.h"
#include "WaveConfiguration.h"

namespace vgaudio {
    namespace formats {
        namespace pcm8 {
            struct Pcm8FormatBase;
        }
    }
}

namespace common_lib {
    namespace io {
        struct BinaryWriter;
    }
}

namespace vgaudio {
    namespace containers {
        namespace wave {

            struct WaveWriter : AudioWriter<WaveConfiguration> {

                DECLARE_CLASS_TEMPLATE(WaveWriter, AudioWriter, WaveConfiguration);

            private:

                std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> _pcm16;
                std::shared_ptr<vgaudio::formats::pcm8::Pcm8FormatBase> _pcm8;
                std::shared_ptr<vgaudio::formats::IAudioFormat> _audioFormat;

            public:

                WaveWriter() = default;

                ~WaveWriter() override = default;

            private:

                _CXX17_ATTR_NODISCARD
                WaveCodec getCodec() const;

                _CXX17_ATTR_NODISCARD
                int32_t getChannelCount() const;

                _CXX17_ATTR_NODISCARD
                int32_t getSampleCount() const;

                _CXX17_ATTR_NODISCARD
                int32_t getSampleRate() const;

                _CXX17_ATTR_NODISCARD
                bool isLooping() const;

                _CXX17_ATTR_NODISCARD
                int32_t getLoopStart() const;

                _CXX17_ATTR_NODISCARD
                int32_t getLoopEnd() const;

                _CXX17_ATTR_NODISCARD
                int32_t getRiffChunkSize() const;

                _CXX17_ATTR_NODISCARD
                int32_t getFmtChunkSize() const;

                _CXX17_ATTR_NODISCARD
                int32_t getDataChunkSize() const;

                _CXX17_ATTR_NODISCARD
                int32_t getSmplChunkSize() const;

                _CXX17_ATTR_NODISCARD
                int32_t getBitDepth() const;

                _CXX17_ATTR_NODISCARD
                int32_t getBytesPerSample() const;

                _CXX17_ATTR_NODISCARD
                int32_t getBytesPerSecond() const;

                _CXX17_ATTR_NODISCARD
                int32_t getBlockAlign() const;

            protected:

                void setupWriter(const std::shared_ptr<vgaudio::formats::AudioData> &audio) override;

                void writeStream(const std::shared_ptr<common_lib::io::Stream> &stream) override;

                int32_t getFileSize() override;

            private:

                void writeRiffHeader(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeFmtChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeDataChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

                void writeSmplChunk(const std::shared_ptr<common_lib::io::BinaryWriter> &writer);

            };

        }
    }
}
