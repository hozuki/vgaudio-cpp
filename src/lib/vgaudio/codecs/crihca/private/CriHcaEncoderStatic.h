#pragma once

#include <cstdint>
#include <memory>

#include "../../../../common/utilities/runtime_array.h"
#include "../../../../common/utilities/runtime_jagged_array.h"
#include "../CriHcaQuality.h"

namespace vgaudio::codecs::crihca {
    struct HcaInfo;
    struct CriHcaChannel;
    struct CriHcaFrame;
}

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

void CalculateBandCounts(const shared_ptr<HcaInfo> &hca, int32_t bitrate, int32_t cutoffFrequency);

void SetChannelConfiguration(const shared_ptr<HcaInfo> &hca, int32_t channelConfig = -1);

void CalculateLoopInfo(const shared_ptr<HcaInfo> &hca, int32_t loopStart, int32_t loopEnd);

void CalculateHeaderSize(const shared_ptr<HcaInfo> &hca);

void QuantizeSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

void CalculateFrameResolutions(const shared_ptr<CriHcaFrame> &frame);

void CalculateNoiseLevel(const shared_ptr<CriHcaFrame> &frame);

void CalculateEvaluationBoundary(const shared_ptr<CriHcaFrame> &frame);

void CalculateFrameHeaderLength(const shared_ptr<CriHcaFrame> &frame);

void ScaleSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

void CalculateScaleFactors(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

void EncodeIntensityStereo(const shared_ptr<CriHcaFrame> &frame);

void CalculateHfrGroupAverages(const shared_ptr<CriHcaFrame> &frame);

void CalculateHfrScale(const shared_ptr<CriHcaFrame> &frame);

void RunMdct(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

void PcmToFloat(const jarray2_ptr <int16_t> &pcm, const array_ptr<shared_ptr<CriHcaChannel>> &channels);

int32_t CalculateBitrate(const shared_ptr<HcaInfo> &hca, CriHcaQuality quality, int32_t bitrate, bool limitBitrate);
