#pragma once

#include <memory>

#include "CriHcaConstants.h"
#include "../../../common/utilities/runtime_array.h"

namespace vgaudio::codecs::crihca {

    struct HcaInfo;
    struct CriHcaChannel;

    using namespace std;
    using namespace common_lib::utilities;

    struct CriHcaFrame final {

    private:

        using C = vgaudio::codecs::crihca::CriHcaConstants;

    private:

        shared_ptr<HcaInfo> _hca;
        array_ptr<shared_ptr<CriHcaChannel>> _channels;
        narray_ptr<uint8_t, C::SamplesPerSubframe> _athCurve;
        int32_t _acceptableNoiseLevel;
        int32_t _evaluationBoundary;

    public:

        explicit CriHcaFrame(const shared_ptr<HcaInfo> &hca);

        ~CriHcaFrame() = default;

        [[nodiscard]]
        shared_ptr<HcaInfo> getHca() const;

        [[nodiscard]]
        array_ptr<shared_ptr<CriHcaChannel>> getChannels() const;

        [[nodiscard]]
        narray_ptr<uint8_t, C::SamplesPerSubframe> getAthCurve() const;

        [[nodiscard]]
        int32_t getAcceptableNoiseLevel() const;

        void setAcceptableNoiseLevel(int32_t level);

        [[nodiscard]]
        int32_t getEvaluationBoundary() const;

        void setEvaluationBoundary(int32_t boundary);

    };

}
