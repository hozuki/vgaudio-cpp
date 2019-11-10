#pragma once

#include <cstdint>
#include <list>
#include <type_traits>

#include "IAudioFormat.h"
#include "../../common/utilities/runtime_array.h"

namespace vgaudio::codecs {
    struct CodecParameters;
}

namespace vgaudio::formats {

    using namespace std;
    using namespace vgaudio::codecs;
    using namespace common_lib::utilities;

    struct AudioFormatBaseBuilder;
    struct AudioTrack;

    struct AudioFormatBase : public IAudioFormat {

        DECLARE_ROOT_CLASS(AudioFormatBase);

    private:

        shared_ptr<list<shared_ptr<AudioTrack>>> _tracks;
        int32_t _sampleRate;
        int32_t _channelCount;
        int32_t _unalignedSampleCount;
        int32_t _unalignedLoopStart;
        int32_t _unalignedLoopEnd;
        bool _looping;

    public:

        virtual ~AudioFormatBase() = default;

        [[nodiscard]]
        int32_t getSampleRate() const override;

        [[nodiscard]]
        int32_t getChannelCount() const override;

        [[nodiscard]]
        int32_t getUnalignedSampleCount() const;

        [[nodiscard]]
        int32_t getUnalignedLoopStart() const;

        [[nodiscard]]
        int32_t getUnalignedLoopEnd() const;

        [[nodiscard]]
        int32_t getSampleCount() const override;

        [[nodiscard]]
        int32_t getLoopStart() const override;

        [[nodiscard]]
        int32_t getLoopEnd() const override;

        [[nodiscard]]
        bool isLooping() const override;

        [[nodiscard]]
        shared_ptr<list<shared_ptr<AudioTrack>>> getTracks() const;

        shared_ptr<IAudioFormat> getChannels(const array_ptr<int32_t> &channelRange) final;

        shared_ptr<IAudioFormat> withLoop(bool loop) override;

        shared_ptr<IAudioFormat> withLoop(bool loop, int32_t loopStart, int32_t loopEnd) override;

        shared_ptr<Pcm16Format> toPcm16() override = 0;

        shared_ptr<Pcm16Format> toPcm16(const shared_ptr<CodecParameters> &config) override;

        shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) override = 0;

        virtual shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() = 0;

        bool tryAdd(const shared_ptr<IAudioFormat> &format, shared_ptr<IAudioFormat> &result) final;

        virtual shared_ptr<AudioFormatBase> add(const shared_ptr<AudioFormatBase> &format);

        virtual shared_ptr<IAudioFormat> encodeFromPcm16WithConfig(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config);

    protected:

        AudioFormatBase();

        explicit AudioFormatBase(const shared_ptr<AudioFormatBaseBuilder> &builder);

        virtual shared_ptr<AudioFormatBase> getChannelsInternal(const array_ptr<int32_t> &channelRange) = 0;

        virtual shared_ptr<AudioFormatBase> addInternal(const shared_ptr<IAudioFormat> &format) = 0;

        shared_ptr<AudioFormatBaseBuilder> getCloneBuilderBase(const shared_ptr<AudioFormatBaseBuilder> &builder);

        [[nodiscard]]
        virtual bool canAcceptAudioFormat(const IAudioFormat *format) const = 0;

        [[nodiscard]]
        virtual bool canAcceptConfig(const CodecParameters *config) const = 0;

        [[nodiscard]]
        virtual shared_ptr<CodecParameters> createConfig() const;

    private:

        shared_ptr<CodecParameters> getDerivedParameters(const shared_ptr<CodecParameters> &param);

        shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) final;

    };

}
