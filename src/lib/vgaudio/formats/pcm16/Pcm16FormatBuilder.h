#pragma once

#include <cstdint>
#include <memory>

#include "../AudioFormatBaseBuilder.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::formats::pcm16 {

    struct Pcm16Format;

    struct Pcm16FormatBuilder : public AudioFormatBaseBuilder {

        DECLARE_CLASS(Pcm16FormatBuilder, AudioFormatBaseBuilder);

    private:

        common_lib::utilities::jarray2_ptr<int16_t> _channels;

    public:

        Pcm16FormatBuilder(const common_lib::utilities::jarray2_ptr<int16_t> &channels, int32_t sampleRate);

        ~Pcm16FormatBuilder() override = default;

        [[nodiscard]]
        common_lib::utilities::jarray2_ptr<int16_t> getChannels() const;

        void setChannels(const common_lib::utilities::jarray2_ptr<int16_t> &channels);

        [[nodiscard]]
        int32_t getChannelCount() const override;

        std::shared_ptr<IAudioFormat> build() override;

    };

}
