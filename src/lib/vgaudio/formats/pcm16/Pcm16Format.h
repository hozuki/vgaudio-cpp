#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/_cxx17.h"
#include "../AudioFormatBase.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace codecs {
        struct CodecParameters;
    }
}

namespace vgaudio {
    namespace formats {
        namespace pcm16 {

            struct Pcm16FormatBuilder;

            struct Pcm16Format : public AudioFormatBase {

                DECLARE_CLASS(Pcm16Format, AudioFormatBase);

            private:

                common_lib::utilities::jarray2_ptr<int16_t> _channels;

            public:

                explicit Pcm16Format(const std::shared_ptr<Pcm16FormatBuilder> &builder);

                Pcm16Format(const common_lib::utilities::jarray2_ptr<int16_t> &channels, int32_t sampleRate);

                Pcm16Format();

                ~Pcm16Format() override = default;

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::jarray2_ptr<int16_t> getChannels() const;

                std::shared_ptr<Pcm16Format> toPcm16() override;

                std::shared_ptr<vgaudio::formats::IAudioFormat> encodeFromPcm16(const std::shared_ptr<Pcm16Format> &pcm16) override;

                std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

                static std::shared_ptr<Pcm16FormatBuilder> getBuilder(const common_lib::utilities::jarray2_ptr<int16_t> &channels, int32_t sampleRate);

            protected:

                std::shared_ptr<AudioFormatBase> addInternal(const std::shared_ptr<vgaudio::formats::IAudioFormat> &pcm16) override;

                std::shared_ptr<AudioFormatBase> getChannelsInternal(const common_lib::utilities::array_ptr<int32_t> &channelRange) override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptAudioFormat(const vgaudio::formats::IAudioFormat *format) const override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptConfig(const vgaudio::codecs::CodecParameters *config) const override;

            };

        }
    }
}
