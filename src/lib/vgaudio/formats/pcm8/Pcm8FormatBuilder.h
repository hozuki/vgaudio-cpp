#pragma once

#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioFormatBaseBuilder.h"

namespace vgaudio::formats::pcm8 {

    using namespace common_lib::utilities;

    struct Pcm8FormatBuilder : public AudioFormatBaseBuilder {

        DECLARE_CLASS(Pcm8FormatBuilder, AudioFormatBaseBuilder);

    private:

        jarray2_ptr<uint8_t> _channels;
        bool _signed;

    public:

        Pcm8FormatBuilder(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate, bool signed_ = false);

        shared_ptr<IAudioFormat> build() override;

        [[nodiscard]]
        int32_t getChannelCount() const override;

        [[nodiscard]]
        jarray2_ptr<uint8_t> getChannels() const;

        void setChannels(const jarray2_ptr<uint8_t> &channels);

        [[nodiscard]]
        bool isSigned() const;

        void setSigned(bool signed_);

    };

}
