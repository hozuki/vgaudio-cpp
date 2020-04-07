#pragma once

#include <cstdint>

#include "../AudioWriter.h"
#include "WaveConfiguration.h"

namespace vgaudio::formats::pcm8 {
    struct Pcm8FormatBase;
}

namespace common_lib::io {
    struct BinaryWriter;
}

namespace vgaudio::containers::wave {

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

        [[nodiscard]]
        WaveCodec getCodec() const;

        [[nodiscard]]
        int32_t getChannelCount() const;

        [[nodiscard]]
        int32_t getSampleCount() const;

        [[nodiscard]]
        int32_t getSampleRate() const;

        [[nodiscard]]
        bool isLooping() const;

        [[nodiscard]]
        int32_t getLoopStart() const;

        [[nodiscard]]
        int32_t getLoopEnd() const;

        [[nodiscard]]
        int32_t getRiffChunkSize() const;

        [[nodiscard]]
        int32_t getFmtChunkSize() const;

        [[nodiscard]]
        int32_t getDataChunkSize() const;

        [[nodiscard]]
        int32_t getSmplChunkSize() const;

        [[nodiscard]]
        int32_t getBitDepth() const;

        [[nodiscard]]
        int32_t getBytesPerSample() const;

        [[nodiscard]]
        int32_t getBytesPerSecond() const;

        [[nodiscard]]
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
