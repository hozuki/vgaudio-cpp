#include "AdxStructure.h"
#include "../../formats/criadx/CriAdxHelpers.h"
#include "../../../common/utilities/IntHelper.h"

using namespace common_lib::utilities;
using namespace vgaudio::containers::adx;
using namespace vgaudio::codecs::criadx;
using namespace vgaudio::formats::criadx;

AdxStructure::AdxStructure()
    : headerSize(0), encodingType(CriAdxType::Linear), frameSize(0), bitDepth(0),
      channelCount(0), sampleRate(0), sampleCount(0), highPassFrequency(0),
      version(0), revision(0), insertedSamples(0), loopCount(0), looping(false), loopType(0),
      loopStartSample(0), loopStartByte(0), loopEndSample(0), loopEndByte(0) {
}

int32_t AdxStructure::getSamplesPerFrame() const {
    return CriAdxHelpers::nibbleCountToSampleCount(frameSize * 2, frameSize);
}

int32_t AdxStructure::getAudioDataLength() const {
    return IntHelper::divideByRoundUp(sampleCount, getSamplesPerFrame()) * frameSize * channelCount;
}
