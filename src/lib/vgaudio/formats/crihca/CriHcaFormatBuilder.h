#pragma once

#include <memory>

#include "../../../common/utilities/runtime_jagged_array_2.h"
#include "../AudioFormatBaseBuilder.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
}

namespace vgaudio::formats::crihca {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::codecs::crihca;

    struct CriHcaFormat;

    struct CriHcaFormatBuilder : public AudioFormatBaseBuilder {

        DECLARE_CLASS(CriHcaFormatBuilder, AudioFormatBaseBuilder);

    private:

        jarray2_ptr<uint8_t> _audioData;
        shared_ptr<HcaInfo> _hca;

    public:

        CriHcaFormatBuilder(const jarray2_ptr<uint8_t> &audioData, const shared_ptr<HcaInfo> &hca);

        ~CriHcaFormatBuilder() override = default;

        [[nodiscard]]
        jarray2_ptr<uint8_t> getAudioData() const;

        [[nodiscard]]
        shared_ptr<HcaInfo> getHca() const;

        [[nodiscard]]
        int32_t getChannelCount() const override;

        shared_ptr<IAudioFormat> build() override;

    };

}
