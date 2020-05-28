#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <type_traits>

#include "../../common/utilities/_cxx17.h"
#include "IAudioFormat.h"
#include "../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace codecs {
        struct CodecParameters;
    }
}

namespace vgaudio {
    namespace formats {

        struct AudioFormatBaseBuilder;
        struct AudioTrack;

        struct AudioFormatBase : public IAudioFormat {

            DECLARE_ROOT_CLASS(AudioFormatBase);

        private:

            std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> _tracks;
            int32_t _sampleRate;
            int32_t _channelCount;
            int32_t _unalignedSampleCount;
            int32_t _unalignedLoopStart;
            int32_t _unalignedLoopEnd;
            bool _looping;

        public:

            virtual ~AudioFormatBase() = default;

            _CXX17_ATTR_NODISCARD
            int32_t getSampleRate() const override;

            _CXX17_ATTR_NODISCARD
            int32_t getChannelCount() const override;

            _CXX17_ATTR_NODISCARD
            int32_t getUnalignedSampleCount() const;

            _CXX17_ATTR_NODISCARD
            int32_t getUnalignedLoopStart() const;

            _CXX17_ATTR_NODISCARD
            int32_t getUnalignedLoopEnd() const;

            _CXX17_ATTR_NODISCARD
            int32_t getSampleCount() const override;

            _CXX17_ATTR_NODISCARD
            int32_t getLoopStart() const override;

            _CXX17_ATTR_NODISCARD
            int32_t getLoopEnd() const override;

            _CXX17_ATTR_NODISCARD
            bool isLooping() const override;

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> getTracks() const;

            std::shared_ptr<IAudioFormat> getChannels(const common_lib::utilities::array_ptr<int32_t> &channelRange) final;

            std::shared_ptr<IAudioFormat> withLoop(bool loop) override;

            std::shared_ptr<IAudioFormat> withLoop(bool loop, int32_t loopStart, int32_t loopEnd) override;

            std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16() override = 0;

            std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> toPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) override;

            std::shared_ptr<IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16) override = 0;

            virtual std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() = 0;

            bool tryAdd(const std::shared_ptr<IAudioFormat> &format, std::shared_ptr<IAudioFormat> &result) final;

            virtual std::shared_ptr<AudioFormatBase> add(const std::shared_ptr<AudioFormatBase> &format);

            virtual std::shared_ptr<IAudioFormat> encodeFromPcm16WithConfig(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config);

        protected:

            AudioFormatBase();

            explicit AudioFormatBase(const std::shared_ptr<AudioFormatBaseBuilder> &builder);

            virtual std::shared_ptr<AudioFormatBase> getChannelsInternal(const common_lib::utilities::array_ptr<int32_t> &channelRange) = 0;

            virtual std::shared_ptr<AudioFormatBase> addInternal(const std::shared_ptr<IAudioFormat> &format) = 0;

            std::shared_ptr<AudioFormatBaseBuilder> getCloneBuilderBase(const std::shared_ptr<AudioFormatBaseBuilder> &builder);

            _CXX17_ATTR_NODISCARD
            virtual bool canAcceptAudioFormat(const IAudioFormat *format) const = 0;

            _CXX17_ATTR_NODISCARD
            virtual bool canAcceptConfig(const vgaudio::codecs::CodecParameters *config) const = 0;

            _CXX17_ATTR_NODISCARD
            virtual std::shared_ptr<vgaudio::codecs::CodecParameters> createConfig() const;

        private:

            std::shared_ptr<vgaudio::codecs::CodecParameters> getDerivedParameters(const std::shared_ptr<vgaudio::codecs::CodecParameters> &param);

            std::shared_ptr<IAudioFormat> encodeFromPcm16(const std::shared_ptr<vgaudio::formats::pcm16::Pcm16Format> &pcm16, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config) final;

        };

    }
}
