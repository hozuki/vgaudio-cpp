#pragma once

#include <cstdint>
#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace vgaudio::codecs {
    struct CodecParameters;
}

namespace vgaudio::formats {

    using namespace std;
    using namespace vgaudio::codecs;
    using namespace common_lib::utilities;

    namespace pcm16 {
        struct Pcm16Format;
    }

    using namespace vgaudio::formats::pcm16;

    struct IAudioFormat {

        virtual ~IAudioFormat() = default;

        [[nodiscard]]
        virtual int32_t getSampleCount() const = 0;

        [[nodiscard]]
        virtual int32_t getSampleRate() const = 0;

        [[nodiscard]]
        virtual int32_t getChannelCount() const = 0;

        [[nodiscard]]
        virtual int32_t getLoopStart() const = 0;

        [[nodiscard]]
        virtual int32_t getLoopEnd() const = 0;

        [[nodiscard]]
        virtual bool isLooping() const = 0;

        virtual shared_ptr<IAudioFormat> withLoop(bool loop, int32_t loopStart, int32_t loopEnd) = 0;

        virtual shared_ptr<IAudioFormat> withLoop(bool loop) = 0;

        virtual shared_ptr<Pcm16Format> toPcm16() = 0;

        virtual shared_ptr<Pcm16Format> toPcm16(const shared_ptr<CodecParameters> &config) = 0;

        virtual shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) = 0;

        virtual shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) = 0;

        virtual shared_ptr<IAudioFormat> getChannels(const array_ptr<int32_t> &channelRange) = 0;

        virtual bool tryAdd(const shared_ptr<IAudioFormat> &format, shared_ptr<IAudioFormat> &result) = 0;

    protected:

        IAudioFormat() = default;

    };

}
