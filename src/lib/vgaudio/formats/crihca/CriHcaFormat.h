#pragma once

#include <cstdint>
#include <memory>

#include "../AudioFormatBase.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
    struct CriHcaParameters;
}

namespace vgaudio::formats::pcm16 {
    struct Pcm16Format;
}

namespace vgaudio::formats::crihca {

    struct CriHcaFormatBuilder;

    struct CriHcaFormat : public AudioFormatBase {

        DECLARE_CLASS(CriHcaFormat, AudioFormatBase);

    private:

        std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> _hca;
        common_lib::utilities::jarray2_ptr<uint8_t> _audioData;

    public:

        CriHcaFormat() = default;

        explicit CriHcaFormat(const std::shared_ptr<CriHcaFormatBuilder> &builder);

        ~CriHcaFormat() override = default;

        [[nodiscard]]
        std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> getHca() const;

        [[nodiscard]]
        common_lib::utilities::jarray2_ptr<uint8_t> getAudioData() const;

        std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() override;

        std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

        std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::crihca::CriHcaParameters> &config);

        std::shared_ptr<vgaudio::formats::IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16) override;

        std::shared_ptr<vgaudio::formats::IAudioFormat> encodeFromPcm16WithConfig(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

    private:

        std::shared_ptr<AudioFormatBase> getChannelsInternal(const common_lib::utilities::array_ptr<int32_t> &channelRange) override;

        std::shared_ptr<AudioFormatBase> addInternal(const std::shared_ptr<vgaudio::formats::IAudioFormat> &format) override;

        std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

        [[nodiscard]]
        bool canAcceptAudioFormat(const vgaudio::formats::IAudioFormat *format) const override;

        [[nodiscard]]
        bool canAcceptConfig(const vgaudio::codecs::CodecParameters *config) const override;

        [[nodiscard]]
        std::shared_ptr<vgaudio::codecs::CodecParameters> createConfig() const override;

    };

}
