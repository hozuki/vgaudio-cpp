#pragma once

#include <cstdint>
#include <memory>

#include "../AudioFormatBase.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs {
    struct CodecParameters;
}

namespace vgaudio::formats::pcm16 {

    using namespace std;
    using namespace vgaudio::formats;
    using namespace vgaudio::codecs;

    struct Pcm16FormatBuilder;

    struct Pcm16Format : public AudioFormatBase {

        DECLARE_CLASS(Pcm16Format, AudioFormatBase);

    private:

        jarray2_ptr<int16_t> _channels;

    public:

        explicit Pcm16Format(const shared_ptr<Pcm16FormatBuilder> &builder);

        Pcm16Format(const jarray2_ptr<int16_t> &channels, int32_t sampleRate);

        Pcm16Format();

        ~Pcm16Format() override = default;

        [[nodiscard]]
        jarray2_ptr<int16_t> getChannels() const;

        shared_ptr<Pcm16Format> toPcm16() override;

        shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) override;

        shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

        static shared_ptr<Pcm16FormatBuilder> getBuilder(const jarray2_ptr<int16_t> &channels, int32_t sampleRate);

    protected:

        shared_ptr<AudioFormatBase> addInternal(const shared_ptr<IAudioFormat> &pcm16) override;

        shared_ptr<AudioFormatBase> getChannelsInternal(const array_ptr<int32_t> &channelRange) override;

        [[nodiscard]]
        bool canAcceptAudioFormat(const IAudioFormat *format) const override;

        [[nodiscard]]
        bool canAcceptConfig(const CodecParameters *config) const override;

    };

}
