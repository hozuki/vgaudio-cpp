#pragma once

#include "../../../common/utilities/_cxx17.h"
#include "Pcm8FormatBase.h"

namespace vgaudio {
    namespace formats {
        namespace pcm8 {

            struct UnsignedPcm8Format : Pcm8FormatBase {

                DECLARE_CLASS(UnsignedPcm8Format, Pcm8FormatBase);

            public:

                UnsignedPcm8Format() = default;

                UnsignedPcm8Format(const common_lib::utilities::jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

                explicit UnsignedPcm8Format(const std::shared_ptr<Pcm8FormatBuilder> &builder);

                ~UnsignedPcm8Format() override = default;

                _CXX17_ATTR_NODISCARD
                bool isSigned() const override;

            protected:

                _CXX17_ATTR_NODISCARD
                std::function<common_lib::utilities::array_ptr<uint8_t>(const common_lib::utilities::array_ptr<int16_t> &)> getEncodeFunction() const override;

                _CXX17_ATTR_NODISCARD
                std::function<common_lib::utilities::array_ptr<int16_t>(const common_lib::utilities::array_ptr<uint8_t> &)> getDecodeFunction() const override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptAudioFormat(const vgaudio::formats::IAudioFormat *format) const override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptConfig(const vgaudio::codecs::CodecParameters *config) const override;

            };

        }
    }
}
