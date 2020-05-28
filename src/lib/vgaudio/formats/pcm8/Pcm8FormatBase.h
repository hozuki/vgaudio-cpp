#pragma once

#include <cstdint>
#include <functional>

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioFormatBase.h"

namespace vgaudio {
    namespace formats {
        namespace pcm16 {
            struct Pcm16Format;
        }
    }
}

namespace vgaudio {
    namespace formats {
        namespace pcm8 {

            struct Pcm8FormatBuilder;

            struct Pcm8FormatBase : AudioFormatBase {

                DECLARE_CLASS(Pcm8FormatBase, AudioFormatBase);

            private:

                common_lib::utilities::jarray2_ptr<uint8_t> _channels;

            public:

                Pcm8FormatBase();

                Pcm8FormatBase(const common_lib::utilities::jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

                explicit Pcm8FormatBase(const std::shared_ptr<Pcm8FormatBuilder> &builder);

                ~Pcm8FormatBase() override = default;

                std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() override;

                std::shared_ptr<IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16) override;

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::jarray2_ptr<uint8_t> getChannels() const;

                _CXX17_ATTR_NODISCARD
                virtual bool isSigned() const = 0;

            protected:

                std::shared_ptr<AudioFormatBase> addInternal(const std::shared_ptr<IAudioFormat> &format) override;

                std::shared_ptr<AudioFormatBase> getChannelsInternal(const common_lib::utilities::array_ptr<int32_t> &channelRange) override;

                _CXX17_ATTR_NODISCARD
                virtual std::function<common_lib::utilities::array_ptr<uint8_t>(const common_lib::utilities::array_ptr<int16_t> &)> getEncodeFunction() const = 0;

                _CXX17_ATTR_NODISCARD
                virtual std::function<common_lib::utilities::array_ptr<int16_t>(const common_lib::utilities::array_ptr<uint8_t> &)> getDecodeFunction() const = 0;

            public:

                static std::shared_ptr<Pcm8FormatBuilder> getBuilder(const common_lib::utilities::jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

                std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

            };

        }
    }
}
