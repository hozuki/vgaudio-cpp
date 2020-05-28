#pragma once

#include <cstdint>
#include <memory>
#include <queue>

#include "CriHcaQuality.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace utilities {
        struct Crc16;
    }
}

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaParameters;
            struct CriHcaChannel;
            struct CriHcaFrame;
            struct HcaInfo;

            struct CriHcaEncoder final {

            private:

                std::queue<common_lib::utilities::array_ptr<uint8_t>> _hcaOutputBuffer;

                std::shared_ptr<HcaInfo> _hca;
                CriHcaQuality _quality;
                int32_t _bitrate;
                int32_t _cutoffFrequency;

                common_lib::utilities::array_ptr<std::shared_ptr<CriHcaChannel>> _channels;
                std::shared_ptr<CriHcaFrame> _frame;
                std::shared_ptr<vgaudio::utilities::Crc16> _crc;

                common_lib::utilities::jarray2_ptr<int16_t> _pcmBuffer;
                size_t _bufferPosition;
                int32_t _bufferPreSamples;
                int32_t _samplesProcessed;
                int32_t _framesProcessed;
                int32_t _postSamples;
                common_lib::utilities::jarray2_ptr<int16_t> _postAudio;

            public:

                ~CriHcaEncoder() = default;

                static std::shared_ptr<CriHcaEncoder> createNew(const std::shared_ptr<CriHcaParameters> &config);

                void initialize(const std::shared_ptr<CriHcaParameters> &config);

                int32_t encode(const common_lib::utilities::jarray2_ptr<int16_t> &pcm, const common_lib::utilities::array_ptr<uint8_t> &hcaOut);

                common_lib::utilities::array_ptr<uint8_t> getPendingFrame();

                [[nodiscard]]
                std::shared_ptr<HcaInfo> getHca() const;

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

                void setHca(const std::shared_ptr<HcaInfo> &hca);

                void setQuality(CriHcaQuality quality);

                void setBitrate(int32_t bitrate);

                void setCutoffFrequency(int32_t frequency);

                void setFramesProcessed(int32_t n);

            private:

                int32_t encodePreAudio(const common_lib::utilities::jarray2_ptr<int16_t> &pcm, const common_lib::utilities::array_ptr<uint8_t> &hcaOut, int32_t framesOutput);

                int32_t encodeMainAudio(const common_lib::utilities::jarray2_ptr<int16_t> &pcm, const common_lib::utilities::array_ptr<uint8_t> &hcaOut, int32_t framesOutput, int32_t &pcmPosition);

                int32_t encodePostAudio(const common_lib::utilities::jarray2_ptr<int16_t> &pcm, const common_lib::utilities::array_ptr<uint8_t> &hcaOut, int32_t framesOutput);

                void saveLoopAudio(const common_lib::utilities::jarray2_ptr<int16_t> &pcm);

                int32_t outputFrame(int32_t framesOutput, const common_lib::utilities::array_ptr<uint8_t> &hcaOut);

                void encodeFrame(const common_lib::utilities::jarray2_ptr<int16_t> &pcm, const common_lib::utilities::array_ptr<uint8_t> &hcaOut);

            };

        }
    }
}
