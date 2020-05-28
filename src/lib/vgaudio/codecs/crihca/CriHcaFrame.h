#pragma once

#include <memory>

#include "CriHcaConstants.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio {
    namespace codecs {
        namespace crihca {

            struct HcaInfo;
            struct CriHcaChannel;

            struct CriHcaFrame final {

            private:

                using C = vgaudio::codecs::crihca::CriHcaConstants;

            private:

                std::shared_ptr<HcaInfo> _hca;
                common_lib::utilities::array_ptr<std::shared_ptr<CriHcaChannel>> _channels;
                common_lib::utilities::narray_ptr<uint8_t, C::SamplesPerSubframe> _athCurve;
                int32_t _acceptableNoiseLevel;
                int32_t _evaluationBoundary;

            public:

                explicit CriHcaFrame(const std::shared_ptr<HcaInfo> &hca);

                ~CriHcaFrame() = default;

                [[nodiscard]]
                std::shared_ptr<HcaInfo> getHca() const;

                [[nodiscard]]
                common_lib::utilities::array_ptr<std::shared_ptr<CriHcaChannel>> getChannels() const;

                [[nodiscard]]
                common_lib::utilities::narray_ptr<uint8_t, C::SamplesPerSubframe> getAthCurve() const;

                [[nodiscard]]
                int32_t getAcceptableNoiseLevel() const;

                void setAcceptableNoiseLevel(int32_t level);

                [[nodiscard]]
                int32_t getEvaluationBoundary() const;

                void setEvaluationBoundary(int32_t boundary);

            };

        }
    }
}
