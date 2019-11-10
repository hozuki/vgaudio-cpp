#pragma once

#include <cstdint>
#include <memory>
#include <queue>

#include "CriHcaQuality.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array_2.h"

namespace vgaudio::utilities {
    struct Crc16;
}

namespace vgaudio::codecs::crihca {

    struct CriHcaParameters;
    struct CriHcaChannel;
    struct CriHcaFrame;
    struct HcaInfo;

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::utilities;

    struct CriHcaEncoder final {

    private:

        queue <array_ptr<uint8_t>> _hcaOutputBuffer;

        shared_ptr<HcaInfo> _hca;
        CriHcaQuality _quality;
        int32_t _bitrate;
        int32_t _cutoffFrequency;

        array_ptr<shared_ptr<CriHcaChannel>> _channels;
        shared_ptr<CriHcaFrame> _frame;
        shared_ptr<Crc16> _crc;

        jarray2_ptr<int16_t> _pcmBuffer;
        size_t _bufferPosition;
        int32_t _bufferPreSamples;
        int32_t _samplesProcessed;
        int32_t _framesProcessed;
        int32_t _postSamples;
        jarray2_ptr<int16_t> _postAudio;

    public:

        ~CriHcaEncoder() = default;

        static shared_ptr<CriHcaEncoder> createNew(const shared_ptr<CriHcaParameters> &config);

        void initialize(const shared_ptr<CriHcaParameters> &config);

        int32_t encode(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut);

        array_ptr<uint8_t> getPendingFrame();

        [[nodiscard]]
        shared_ptr<HcaInfo> getHca() const;

        [[nodiscard]]
        CriHcaQuality getQuality() const;

        [[nodiscard]]
        int32_t getBitrate() const;

        [[nodiscard]]
        int32_t getCutoffFrequency() const;

        [[nodiscard]]
        size_t getPendingFrameCount() const;

        [[nodiscard]]
        int32_t getFrameSize() const;

        [[nodiscard]]
        size_t getBufferRemaining() const;

        [[nodiscard]]
        size_t getFramesProcessed() const;

    private:

        CriHcaEncoder();

        void setHca(const shared_ptr<HcaInfo> &hca);

        void setQuality(CriHcaQuality quality);

        void setBitrate(int32_t bitrate);

        void setCutoffFrequency(int32_t frequency);

        void setFramesProcessed(int32_t n);

    private:

        int32_t encodePreAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput);

        int32_t encodeMainAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput, int32_t &pcmPosition);

        int32_t encodePostAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput);

        void saveLoopAudio(const jarray2_ptr<int16_t> &pcm);

        int32_t outputFrame(int32_t framesOutput, const array_ptr<uint8_t> &hcaOut);

        void encodeFrame(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut);

    };

}
