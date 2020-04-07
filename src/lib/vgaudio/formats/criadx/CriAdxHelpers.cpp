#include "CriAdxHelpers.h"
#include "../../../common/utilities/IntHelper.h"

using namespace common_lib::utilities;
using namespace vgaudio::formats::criadx;

int32_t CriAdxHelpers::nibbleCountToSampleCount(int32_t nibbleCount, int32_t frameSize) {
    const int32_t nibblesPerFrame = frameSize * 2;
    const int32_t samplesPerFrame = nibblesPerFrame - 4;

    const int32_t frames = nibbleCount / nibblesPerFrame;
    const int32_t extraNibbles = nibbleCount % nibblesPerFrame;
    const int32_t extraSamples = extraNibbles < 4 ? 0 : (extraNibbles - 4);

    return samplesPerFrame * frames + extraSamples;
}

int32_t CriAdxHelpers::sampleCountToNibbleCount(int32_t sampleCount, int32_t frameSize) {
    const int32_t nibblesPerFrame = frameSize * 2;
    const int32_t samplesPerFrame = nibblesPerFrame - 4;

    const int32_t frames = sampleCount / samplesPerFrame;
    const int32_t extraSamples = sampleCount % samplesPerFrame;
    const int32_t extraNibbles = extraSamples == 0 ? 0 : (extraSamples + 4);

    return nibblesPerFrame * frames + extraNibbles;
}

int32_t CriAdxHelpers::sampleCountToByteCount(int32_t sampleCount, int32_t frameSize) {
    const auto nibbleCount = sampleCountToNibbleCount(sampleCount, frameSize);
    return IntHelper::divideByRoundUp(nibbleCount, 2);
}
