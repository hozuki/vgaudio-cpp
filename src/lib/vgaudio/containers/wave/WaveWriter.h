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

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;
    using namespace vgaudio::formats::pcm8;

    struct WaveWriter : AudioWriter<WaveConfiguration> {

        DECLARE_CLASS_TEMPLATE(WaveWriter, AudioWriter, WaveConfiguration);

    private:

        shared_ptr<Pcm16Format> _pcm16;
        shared_ptr<Pcm8FormatBase> _pcm8;
        shared_ptr<IAudioFormat> _audioFormat;

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

        void setupWriter(const shared_ptr<AudioData> &audio) override;

        void writeStream(const shared_ptr<Stream> &stream) override;

        int32_t getFileSize() override;

    private:

        void writeRiffHeader(const shared_ptr<BinaryWriter> &writer);

        void writeFmtChunk(const shared_ptr<BinaryWriter> &writer);

        void writeDataChunk(const shared_ptr<BinaryWriter> &writer);

        void writeSmplChunk(const shared_ptr<BinaryWriter> &writer);

    };

}
