#pragma once

#include <cstdint>

#include "ChannelType.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "CriHcaConstants.h"

namespace vgaudio {
    namespace utilities {
        struct Mdct;
    }
}

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaChannel final {

            private:

                using C = vgaudio::codecs::crihca::CriHcaConstants;

            public:

                CriHcaChannel();

                ~CriHcaChannel() = default;

                ChannelType type;
                int32_t codedScaleFactorCount;
                common_lib::utilities::jnarray2_ptr<double, C::SubframesPerFrame, C::SamplesPerSubframe> pcmFloat;
                common_lib::utilities::jnarray2_ptr<double, C::SubframesPerFrame, C::SamplesPerSubframe> spectra;
                common_lib::utilities::jnarray2_ptr<double, C::SamplesPerSubframe, C::SubframesPerFrame> scaledSpectra;
                common_lib::utilities::jnarray2_ptr<int32_t, C::SubframesPerFrame, C::SamplesPerSubframe> quantizedSpectra;
                common_lib::utilities::narray_ptr<double, C::SamplesPerSubframe> gain;
                common_lib::utilities::narray_ptr<int32_t, C::SubframesPerFrame> intensity;
                common_lib::utilities::narray_ptr<int32_t, 8> hfrScales;
                common_lib::utilities::narray_ptr<double, 8> hfrGroupAverageSpectra;
                std::shared_ptr<vgaudio::utilities::Mdct> mdct;
                common_lib::utilities::narray_ptr<int32_t, C::SamplesPerSubframe> scaleFactors;
                common_lib::utilities::narray_ptr<int32_t, C::SamplesPerSubframe> resolution;
                int32_t headerLengthBits;
                int32_t scaleFactorDeltaBits;

            };

        }
    }
}
