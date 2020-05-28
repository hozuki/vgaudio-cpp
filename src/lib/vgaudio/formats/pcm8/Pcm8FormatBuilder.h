#pragma once

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioFormatBaseBuilder.h"

namespace vgaudio {
    namespace formats {
        namespace pcm8 {

            struct Pcm8FormatBuilder : public AudioFormatBaseBuilder {

                DECLARE_CLASS(Pcm8FormatBuilder, AudioFormatBaseBuilder);

            private:

                common_lib::utilities::jarray2_ptr<uint8_t> _channels;
                bool _signed;

            public:

                Pcm8FormatBuilder(const common_lib::utilities::jarray2_ptr<uint8_t> &channels, int32_t sampleRate, bool signed_ = false);

                std::shared_ptr<vgaudio::formats::IAudioFormat> build() override;

                _CXX17_ATTR_NODISCARD
                int32_t getChannelCount() const override;

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::jarray2_ptr<uint8_t> getChannels() const;

                void setChannels(const common_lib::utilities::jarray2_ptr<uint8_t> &channels);

                _CXX17_ATTR_NODISCARD
                bool isSigned() const;

                void setSigned(bool signed_);

            };

        }
    }
}
