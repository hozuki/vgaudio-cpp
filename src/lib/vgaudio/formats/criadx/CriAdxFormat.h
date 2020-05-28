#pragma once

#include <memory>

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/runtime_array.h"
#include "../AudioFormatBase.h"
#include "../../codecs/criadx/CriAdxType.h"

namespace vgaudio {
    namespace codecs {
        namespace criadx {
            struct CriAdxParameters;
        }
    }
}

namespace vgaudio {
    namespace formats {
        namespace criadx {

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

                _CXX17_ATTR_NODISCARD
                common_lib::utilities::array_ptr<std::shared_ptr<CriAdxChannel>> getChannels() const;

                _CXX17_ATTR_NODISCARD
                int16_t getHighPassFrequency() const;

                _CXX17_ATTR_NODISCARD
                int32_t getFrameSize() const;

                _CXX17_ATTR_NODISCARD
                int32_t getAlignmentSamples() const;

                _CXX17_ATTR_NODISCARD
                int32_t getVersion() const;

                _CXX17_ATTR_NODISCARD
                vgaudio::codecs::criadx::CriAdxType getType() const;

                _CXX17_ATTR_NODISCARD
                int32_t getSampleCount() const override;

                _CXX17_ATTR_NODISCARD
                int32_t getLoopStart() const override;

                _CXX17_ATTR_NODISCARD
                int32_t getLoopEnd() const override;

                std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() override;

                std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

                std::shared_ptr<vgaudio::formats::IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16) override;

                std::shared_ptr<vgaudio::formats::IAudioFormat> encodeFromPcm16WithConfig(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

            private:

                std::shared_ptr<AudioFormatBase> getChannelsInternal(const common_lib::utilities::array_ptr<int32_t> &channelRange) override;

                std::shared_ptr<AudioFormatBase> addInternal(const std::shared_ptr<vgaudio::formats::IAudioFormat> &format) override;

                std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptAudioFormat(const vgaudio::formats::IAudioFormat *format) const override;

                _CXX17_ATTR_NODISCARD
                bool canAcceptConfig(const vgaudio::codecs::CodecParameters *config) const override;

                _CXX17_ATTR_NODISCARD
                std::shared_ptr<vgaudio::codecs::CodecParameters> createConfig() const override;

            };

        }
    }
}
