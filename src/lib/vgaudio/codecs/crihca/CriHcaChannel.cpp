#include <cmath>
#include <memory>

#include "../../utilities/Mdct.h"
#include "CriHcaConstants.h"
#include "CriHcaTables.h"
#include "CriHcaChannel.h"

using namespace vgaudio::codecs::crihca;

using C = CriHcaConstants;

CriHcaChannel::CriHcaChannel() {
    type = static_cast<ChannelType>(0);
    codedScaleFactorCount = 0;
    pcmFloat = make_jagged_array_2_static<double, C::SubframesPerFrame, C::SamplesPerSubframe>();
    spectra = make_jagged_array_2_static<double, C::SubframesPerFrame, C::SamplesPerSubframe>();
    scaledSpectra = make_jagged_array_2_static<double, C::SamplesPerSubframe, C::SubframesPerFrame>();
    quantizedSpectra = make_jagged_array_2_static<int32_t, C::SubframesPerFrame, C::SamplesPerSubframe>();
    gain = make_array_static<double, C::SamplesPerSubframe>();
    intensity = make_array_static<int32_t, C::SubframesPerFrame>();
    hfrScales = make_array_static<int32_t, 8>();
    hfrGroupAverageSpectra = make_array_static<double, 8>();
    mdct = std::make_shared<Mdct>(C::SubframeSampleBits, CriHcaTables::getMdctWindow(), std::sqrt(2.0 / C::SamplesPerSubframe));
    scaleFactors = make_array_static<int32_t, C::SamplesPerSubframe>();
    resolution = make_array_static<int32_t, C::SamplesPerSubframe>();
    headerLengthBits = 0;
    scaleFactorDeltaBits = 0;
}
