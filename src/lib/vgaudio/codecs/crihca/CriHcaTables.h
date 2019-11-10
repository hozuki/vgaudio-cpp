#pragma once

#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array_2.h"

namespace vgaudio::codecs::crihca {

    using namespace std;
    using namespace common_lib::utilities;

    struct CriHcaTables {

        IMPLEMENT_STATIC_CLASS(CriHcaTables);

        /* Initialized by generation */

        static narray_ptr<double, 64> getDequantizerScalingTable();

        static narray_ptr<double, 16> getQuantizerStepSize();

        static narray_ptr<double, 16> getQuantizerDeadZone();

        static narray_ptr<double, 64> getQuantizerScalingTable();

        static narray_ptr<double, 16> getQuantizerInverseStepSize();

        static narray_ptr<int32_t, 16> getResolutionMaxValues();

        static narray_ptr<double, 15> getIntensityRatioTable();

        static narray_ptr<double, 14> getIntensityRatioBoundsTable();

        static narray_ptr<double, 128> getScaleConversionTable();

        /* Initialized by unpacking stored array */

        static array_ptr<uint8_t> getDefaultChannelMapping();

        static jarray2_ptr<uint8_t> getValidChannelMappings();

        static array_ptr<uint8_t> getScaleToResolutionCurve();

        static array_ptr<uint8_t> getQuantizedSpectrumMaxBits();

        static jarray2_ptr<uint8_t> getQuantizedSpectrumBits();

        static jarray2_ptr<int8_t> getQuantizedSpectrumValue();

        static jarray2_ptr<uint8_t> getQuantizeSpectrumBits();

        static jarray2_ptr<uint8_t> getQuantizeSpectrumValue();

        static array_ptr<double> getMdctWindow();

        static array_ptr<uint8_t> getAthCurve();

    };

}
