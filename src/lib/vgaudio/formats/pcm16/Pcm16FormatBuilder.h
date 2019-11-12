#pragma once

#include <cstdint>
#include <memory>

#include "../AudioFormatBaseBuilder.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::formats::pcm16 {

    using namespace vgaudio::formats;
    using namespace common_lib::utilities;

    struct Pcm16Format;

    struct Pcm16FormatBuilder : public AudioFormatBaseBuilder {

        DECLARE_CLASS(Pcm16FormatBuilder, AudioFormatBaseBuilder);

    private:

        jarray2_ptr<int16_t> _channels;

    public:

        Pcm16FormatBuilder(const jarray2_ptr<int16_t> &channels, int32_t sampleRate);

        ~Pcm16FormatBuilder() override = default;

        [[nodiscard]]
        jarray2_ptr<int16_t> getChannels() const;

        void setChannels(const jarray2_ptr<int16_t> &channels);

        [[nodiscard]]
        int32_t getChannelCount() const override;

        shared_ptr<IAudioFormat> build() override;

    };

}
