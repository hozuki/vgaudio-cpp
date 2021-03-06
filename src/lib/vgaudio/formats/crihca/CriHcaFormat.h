#pragma once

#include <cstdint>
#include <memory>

#include "../../../common/utilities/_cxx17.h"
#include "../AudioFormatBase.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {
            struct HcaInfo;
            struct CriHcaParameters;
        }
    }
}

namespace vgaudio {
    namespace formats {
        namespace pcm16 {
            struct Pcm16Format;
        }
    }
}

namespace vgaudio {
    namespace formats {
        namespace crihca {

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

                _CXX17_ATTR_NODISCARD
                std::shared_ptr<vgaudio::codecs::crihca::HcaInfo> getHca() const;

                _CXX17_ATTR_NODISCARD
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
