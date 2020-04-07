#pragma once

#include <memory>

#include "../../../common/utilities/runtime_array.h"
#include "../AudioFormatBase.h"
#include "../../codecs/criadx/CriAdxType.h"

namespace vgaudio::codecs::criadx {
    struct CriAdxParameters;
}

namespace vgaudio::formats::criadx {

    struct CriAdxFormatBuilder;
    struct CriAdxChannel;

    struct CriAdxFormat : public AudioFormatBase {

        DECLARE_CLASS(CriAdxFormat, AudioFormatBase);

    private:

        common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> _channels;
        int16_t _highPassFrequency;
        int32_t _frameSize;
        int32_t _alignmentSamples;
        int32_t _version;
        vgaudio::codecs::criadx::CriAdxType _type;

    public:

        CriAdxFormat();

        explicit CriAdxFormat(const std::shared_ptr<CriAdxFormatBuilder> &builder);

        ~CriAdxFormat() override = default;

        [[nodiscard]]
        common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> getChannels() const;

        [[nodiscard]]
        int16_t getHighPassFrequency() const;

        [[nodiscard]]
        int32_t getFrameSize() const;

        [[nodiscard]]
        int32_t getAlignmentSamples() const;

        [[nodiscard]]
        int32_t getVersion() const;

        [[nodiscard]]
        vgaudio::codecs::criadx::CriAdxType getType() const;

        [[nodiscard]]
        int32_t getSampleCount() const override;

        [[nodiscard]]
        int32_t getLoopStart() const override;

        [[nodiscard]]
        int32_t getLoopEnd() const override;

        std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() override;

        std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

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
