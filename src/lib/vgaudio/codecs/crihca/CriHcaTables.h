#pragma once

#include <memory>

#include "../../../common/utilities/static_class.h"
#include "../../../common/utilities/runtime_array.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct CriHcaTables {

                IMPLEMENT_STATIC_CLASS(CriHcaTables);

                /* Initialized by generation */

                static common_lib::utilities::narray_ptr<double, 64> getDequantizerScalingTable();

                static common_lib::utilities::narray_ptr<double, 16> getQuantizerStepSize();

                static common_lib::utilities::narray_ptr<double, 16> getQuantizerDeadZone();

                static common_lib::utilities::narray_ptr<double, 64> getQuantizerScalingTable();

                static common_lib::utilities::narray_ptr<double, 16> getQuantizerInverseStepSize();

                static common_lib::utilities::narray_ptr<int32_t, 16> getResolutionMaxValues();

                static common_lib::utilities::narray_ptr<double, 15> getIntensityRatioTable();

                static common_lib::utilities::narray_ptr<double, 14> getIntensityRatioBoundsTable();

                static common_lib::utilities::narray_ptr<double, 128> getScaleConversionTable();

                /* Initialized by unpacking stored array */

                static common_lib::utilities::array_ptr<uint8_t> getDefaultChannelMapping();

                static common_lib::utilities::jarray2_ptr<uint8_t> getValidChannelMappings();

                static common_lib::utilities::array_ptr<uint8_t> getScaleToResolutionCurve();

                static common_lib::utilities::array_ptr<uint8_t> getQuantizedSpectrumMaxBits();

                static common_lib::utilities::jarray2_ptr<uint8_t> getQuantizedSpectrumBits();

                static common_lib::utilities::jarray2_ptr<int8_t> getQuantizedSpectrumValue();

                static common_lib::utilities::jarray2_ptr<uint8_t> getQuantizeSpectrumBits();

                static common_lib::utilities::jarray2_ptr<uint8_t> getQuantizeSpectrumValue();

                static common_lib::utilities::array_ptr<double> getMdctWindow();

                static common_lib::utilities::array_ptr<uint8_t> getAthCurve();

            };

        }
    }
}
