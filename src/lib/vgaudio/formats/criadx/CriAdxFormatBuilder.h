#pragma once

#include <memory>

#include "../AudioFormatBaseBuilder.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../codecs/criadx/CriAdxType.h"

namespace vgaudio::formats::criadx {

    struct CriAdxFormat;
    struct CriAdxChannel;

    struct CriAdxFormatBuilder : public AudioFormatBaseBuilder {

        DECLARE_CLASS(CriAdxFormatBuilder, AudioFormatBaseBuilder);

    private:

        common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> _channels;
        int16_t _highPassFrequency;
        int32_t _frameSize;
        int32_t _alignmentSamples;
        vgaudio::codecs::criadx::CriAdxType _type;

    public:

        CriAdxFormatBuilder(const common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> &channels, int32_t sampleCount, int32_t sampleRate, int32_t frameSize, int16_t highPassFrequency);

        ~CriAdxFormatBuilder() override = default;

        [[nodiscard]]
        int32_t getChannelCount() const override;

        std::shared_ptr<IAudioFormat> build() override;

    public:

        [[nodiscard]]
        common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> getChannels() const;

        void setChannels(const common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> &channels);

        [[nodiscard]]
        int16_t getHighPassFrequency() const;

        void setHighPassFrequency(int16_t frequency);

        [[nodiscard]]
        int32_t getFrameSize() const;

        void setFrameSize(int32_t frameSize);

        [[nodiscard]]
        int32_t getAlignmentSamples() const;

        void setAlignmentSamples(int32_t samples);

        [[nodiscard]]
        vgaudio::codecs::criadx::CriAdxType getType() const;

        void setType(vgaudio::codecs::criadx::CriAdxType type);

        [[nodiscard]]
        int32_t getVersion() const;

    };

}
