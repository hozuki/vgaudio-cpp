#include "CriHcaTables.h"
#include "CriHcaChannel.h"
#include "HcaInfo.h"
#include "CriHcaFrame.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

using C = CriHcaConstants;

static narray_ptr<uint8_t, C::SamplesPerSubframe> ScaleAthCurve(int32_t frequency) {
    auto ath = make_array_static<uint8_t, C::SamplesPerSubframe>();

    int32_t acc = 0;
    int32_t i;

    const auto &curve = CriHcaTables::getAthCurve();
    for (i = 0; i < C::SamplesPerSubframe; i += 1) {
        acc += frequency;
        const int32_t index = acc >> 13;

        if (index >= curve->size()) {
            break;
        }

        (*ath)[i] = (*curve)[index];
    }

    for (; i < ath->size(); i += 1) {
        (*ath)[i] = 0xff;
    }

    return ath;
}

static array_ptr<ChannelType> GetChannelTypes(const shared_ptr<HcaInfo> &hca) {
    const int32_t channelsPerTrack = hca->channelCount / hca->trackCount;

    if (hca->stereoBandCount == 0 || channelsPerTrack == 1) {
        return make_array_dynamic<ChannelType>(8);
    }

    switch (channelsPerTrack) {
        case 2:
            return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary);
        case 3:
            return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete);
        case 4: {
            if (hca->channelConfig != 0) {
                return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::Discrete);
            } else {
                return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::StereoPrimary, ChannelType::StereoSecondary);
            }
        }
        case 5: {
            if (hca->channelConfig > 2) {
                return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::Discrete, ChannelType::Discrete);
            } else {
                return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::StereoPrimary,
                                               ChannelType::StereoSecondary);
            }
        }
        case 6:
            return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::Discrete, ChannelType::StereoPrimary,
                                           ChannelType::StereoSecondary);
        case 7:
            return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::Discrete, ChannelType::StereoPrimary,
                                           ChannelType::StereoSecondary, ChannelType::Discrete);
        case 8:
            return make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete, ChannelType::Discrete, ChannelType::StereoPrimary,
                                           ChannelType::StereoSecondary, ChannelType::StereoPrimary, ChannelType::StereoSecondary);
        default:
            return make_array_dynamic<ChannelType>(channelsPerTrack);
    }
}

CriHcaFrame::CriHcaFrame(const shared_ptr<HcaInfo> &hca)
    : _hca(hca), _acceptableNoiseLevel(0), _evaluationBoundary(0) {
    auto channelTypes = GetChannelTypes(hca);
    auto channels = make_array_dynamic<shared_ptr<CriHcaChannel>>(hca->channelCount);
    _channels = channels;

    for (auto i = 0; i < hca->channelCount; i += 1) {
        auto channel = make_shared<CriHcaChannel>();
        channel->type = (*channelTypes)[i];
        channel->codedScaleFactorCount = channel->type == ChannelType::StereoSecondary ? hca->baseBandCount : hca->baseBandCount + hca->stereoBandCount;
        (*channels)[i] = channel;
    }

    _athCurve = hca->useAthCurve ? ScaleAthCurve(hca->sampleRate) : make_array_static<uint8_t, C::SamplesPerSubframe>();
}

shared_ptr<HcaInfo> CriHcaFrame::getHca() const {
    return _hca;
}

array_ptr<shared_ptr<CriHcaChannel>> CriHcaFrame::getChannels() const {
    return _channels;
}

narray_ptr<uint8_t, C::SamplesPerSubframe> CriHcaFrame::getAthCurve() const {
    return _athCurve;
}

int32_t CriHcaFrame::getAcceptableNoiseLevel() const {
    return _acceptableNoiseLevel;
}

void CriHcaFrame::setAcceptableNoiseLevel(int32_t level) {
    _acceptableNoiseLevel = level;
}

int32_t CriHcaFrame::getEvaluationBoundary() const {
    return _evaluationBoundary;
}

void CriHcaFrame::setEvaluationBoundary(int32_t boundary) {
    _evaluationBoundary = boundary;
}
