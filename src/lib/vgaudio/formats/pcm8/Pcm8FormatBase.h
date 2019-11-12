#pragma once

#include <cstdint>
#include <functional>

#include "../../../common/utilities/runtime_jagged_array.h"
#include "../AudioFormatBase.h"

namespace vgaudio::formats::pcm8 {

    using namespace common_lib::utilities;

    struct Pcm8FormatBuilder;

    struct Pcm8FormatBase : AudioFormatBase {

        DECLARE_CLASS(Pcm8FormatBase, AudioFormatBase);

    private:

        jarray2_ptr<uint8_t> _channels;

    public:

        Pcm8FormatBase();

        Pcm8FormatBase(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

        explicit Pcm8FormatBase(const shared_ptr<Pcm8FormatBuilder> &builder);

        ~Pcm8FormatBase() override = default;

        shared_ptr<Pcm16Format> toPcm16() override;

        shared_ptr<IAudioFormat> encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) override;

        [[nodiscard]]
        jarray2_ptr<uint8_t> getChannels() const;

        [[nodiscard]]
        virtual bool isSigned() const = 0;

    protected:

        shared_ptr<AudioFormatBase> addInternal(const shared_ptr<IAudioFormat> &format) override;

        shared_ptr<AudioFormatBase> getChannelsInternal(const array_ptr<int32_t> &channelRange) override;

        [[nodiscard]]
        virtual function<array_ptr<uint8_t>(const array_ptr<int16_t> &)> getEncodeFunction() const = 0;

        [[nodiscard]]
        virtual function<array_ptr<int16_t>(const array_ptr<uint8_t> &)> getDecodeFunction() const = 0;

    public:

        static shared_ptr<Pcm8FormatBuilder> getBuilder(const jarray2_ptr<uint8_t> &channels, int32_t sampleRate);

        shared_ptr<AudioFormatBaseBuilder> getCloneBuilder() override;

    };

}
