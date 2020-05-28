#pragma once

#include <cstdint>
#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace codecs {
        struct CodecParameters;
    }
}

namespace vgaudio {
    namespace formats {

        namespace pcm16 {
            struct Pcm16Format;
        }

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

            virtual std::shared_ptr<IAudioFormat> withLoop(bool loop, int32_t loopStart, int32_t loopEnd) = 0;

            virtual std::shared_ptr<IAudioFormat> withLoop(bool loop) = 0;

            virtual std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() = 0;

            virtual std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) = 0;

            virtual std::shared_ptr<IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16) = 0;

            virtual std::shared_ptr<IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) = 0;

            virtual std::shared_ptr<IAudioFormat> getChannels(const common_lib::utilities::array_ptr<int32_t> &channelRange) = 0;

            virtual bool tryAdd(const std::shared_ptr<IAudioFormat> &format, std::shared_ptr<IAudioFormat> &result) = 0;

        protected:

            IAudioFormat() = default;

        };

    }
}
