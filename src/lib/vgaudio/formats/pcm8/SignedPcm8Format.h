#pragma once

#include "Pcm8FormatBase.h"

namespace vgaudio::formats::pcm8 {

    using namespace std;
    using namespace common_lib::utilities;

    struct SignedPcm8Format : Pcm8FormatBase {

        DECLARE_CLASS(SignedPcm8Format, Pcm8FormatBase);

    public:

        SignedPcm8Format() = default;

        SignedPcm8Format(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

        explicit SignedPcm8Format(const shared_ptr<Pcm8FormatBuilder> &builder);

        ~SignedPcm8Format() override = default;

        [[nodiscard]]
        bool isSigned() const override;

    protected:

        [[nodiscard]]
        function<array_ptr<uint8_t>(const array_ptr<int16_t> &)> getEncodeFunction() const override;

        [[nodiscard]]
        function<array_ptr<int16_t>(const array_ptr<uint8_t> &)> getDecodeFunction() const override;

        [[nodiscard]]
        bool canAcceptAudioFormat(const IAudioFormat *format) const override;

        [[nodiscard]]
        bool canAcceptConfig(const CodecParameters *config) const override;

    };

}
