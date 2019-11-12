#pragma once

#include <cstdint>
#include <memory>

#include "../AudioFormatBase.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
    struct CriHcaParameters;
}

namespace vgaudio::formats::crihca {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::codecs::crihca;

    struct CriHcaFormatBuilder;

    struct CriHcaFormat : public AudioFormatBase {

        DECLARE_CLASS(CriHcaFormat, AudioFormatBase);

    private:

        shared_ptr<HcaInfo> _hca;
        jarray2_ptr<uint8_t> _audioData;

    public:

        CriHcaFormat() = default;

        explicit CriHcaFormat(const shared_ptr<CriHcaFormatBuilder> &builder);

        ~CriHcaFormat() override = default;

        [[nodiscard]]
        shared_ptr<HcaInfo> getHca() const;

        [[nodiscard]]
        jarray2_ptr<uint8_t> getAudioData() const;

        shared_ptr<Pcm16Format> toPcm16() override;

        shared_ptr<Pcm16Format> toPcm16(const shared_ptr<CodecParameters> &config) override;

        shared_ptr<Pcm16Format> toPcm16(const shared_ptr<CriHcaParameters> &config);

        shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) override;

        shared_ptr<IAudioFormat> encodeFromPcm16WithConfig(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) override;

    private:

        shared_ptr<AudioFormatBase> getChannelsInternal(const array_ptr<int32_t> &channelRange) override;

        shared_ptr<AudioFormatBase> addInternal(const shared_ptr<IAudioFormat> &format) override;

        shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

        [[nodiscard]]
        bool canAcceptAudioFormat(const IAudioFormat *format) const override;

        [[nodiscard]]
        bool canAcceptConfig(const CodecParameters *config) const override;

        [[nodiscard]]
        shared_ptr<CodecParameters> createConfig() const override;

    };

}
