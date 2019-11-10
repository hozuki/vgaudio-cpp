#pragma once

#include <cstdint>

#include "ChannelType.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array_2.h"
#include "CriHcaConstants.h"

namespace vgaudio::utilities {
    struct Mdct;
}

namespace vgaudio::codecs::crihca {

    using namespace vgaudio::utilities;
    using namespace common_lib::utilities;

    struct CriHcaChannel final {

    private:

        using C = vgaudio::codecs::crihca::CriHcaConstants;

    public:

        CriHcaChannel();

        ~CriHcaChannel() = default;

        ChannelType type;
        int32_t codedScaleFactorCount;
        jnarray2_ptr<double, C::SubframesPerFrame, C::SamplesPerSubframe> pcmFloat;
        jnarray2_ptr<double, C::SubframesPerFrame, C::SamplesPerSubframe> spectra;
        jnarray2_ptr<double, C::SamplesPerSubframe, C::SubframesPerFrame> scaledSpectra;
        jnarray2_ptr<int32_t, C::SubframesPerFrame, C::SamplesPerSubframe> quantizedSpectra;
        narray_ptr<double, C::SamplesPerSubframe> gain;
        narray_ptr<int32_t, C::SubframesPerFrame> intensity;
        narray_ptr<int32_t, 8> hfrScales;
        narray_ptr<double, 8> hfrGroupAverageSpectra;
        shared_ptr<Mdct> mdct;
        narray_ptr<int32_t, C::SamplesPerSubframe> scaleFactors;
        narray_ptr<int32_t, C::SamplesPerSubframe> resolution;
        int32_t headerLengthBits;
        int32_t scaleFactorDeltaBits;

    };

}
