#include "../../../common/utilities/IntHelper.h"

#include "HcaInfo.h"

using namespace vgaudio::codecs::crihca;
using namespace common_lib::utilities;

HcaInfo::HcaInfo() {
    channelCount = 0;
    sampleRate = 0;
    sampleCount = 0;
    frameCount = 0;
    insertedSamples = 0;
    appendedSamples = 0;
    headerSize = 0;

    frameSize = 0;
    minResolution = 0;
    maxResolution = 0;
    trackCount = 0;
    channelConfig = 0;
    totalBandCount = 0;
    baseBandCount = 0;
    stereoBandCount = 0;
    hfrBandCount = 0;
    bandsPerHfrGroup = 0;
    hfrGroupCount = 0;

    decStereoType = 0;

    looping = false;
    loopStartFrame = 0;
    loopEndFrame = 0;
    preLoopSamples = 0;
    postLoopSamples = 0;

    useAthCurve = false;

    vbrMaxFrameSize = 0;
    vbrNoiseLevel = 0;

    encryptionType = CriHcaKeyType::Type0;

    volume = 1;
}

void HcaInfo::calculateHfrValues() {
    if (bandsPerHfrGroup <= 0) {
        return;
    }

    hfrBandCount = totalBandCount - baseBandCount - stereoBandCount;
    hfrGroupCount = IntHelper::divideByRoundUp(hfrBandCount, bandsPerHfrGroup);
}

int32_t HcaInfo::getLoopStartSample() const {
    return loopStartFrame * 1024 + preLoopSamples - insertedSamples;
}

int32_t HcaInfo::getLoopEndSample() const {
    return (loopEndFrame + 1) * 1024 - postLoopSamples - insertedSamples;
}

size_t HcaInfo::getCommentLength() const {
    return comment.length();
}
