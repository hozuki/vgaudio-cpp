#pragma once

#include <cstdint>

#include "../CodecParameters.h"
#include "CriHcaQuality.h"
#include "../../../common/utilities/type_sys.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaParameters : public CodecParameters {

                DECLARE_CLASS(CriHcaParameters, CodecParameters);

            public:

                CriHcaParameters();

                explicit CriHcaParameters(const CodecParameters &source);

                CriHcaParameters(const CriHcaParameters &) = default;

                CriHcaParameters(CriHcaParameters &&) = default;

                ~CriHcaParameters() override = default;

                CriHcaQuality quality;
                int32_t bitrate;
                bool limitBitrate;
                int32_t channelCount;
                int32_t sampleRate;
                bool looping;
                int32_t loopStart;
                int32_t loopEnd;

            };

        }
    }
}
