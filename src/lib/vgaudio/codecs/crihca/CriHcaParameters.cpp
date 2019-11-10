#include "CriHcaParameters.h"

using namespace vgaudio::codecs;
using namespace vgaudio::codecs::crihca;


CriHcaParameters::CriHcaParameters()
    : quality(CriHcaQuality::High), bitrate(0), limitBitrate(false),
      channelCount(0), sampleRate(0), looping(false), loopStart(0), loopEnd(0) {
}

CriHcaParameters::CriHcaParameters(const CodecParameters &source)
    : MyBase(source),
      quality(CriHcaQuality::High), bitrate(0), limitBitrate(false),
      channelCount(0), sampleRate(0), looping(false), loopStart(0), loopEnd(0) {
}
