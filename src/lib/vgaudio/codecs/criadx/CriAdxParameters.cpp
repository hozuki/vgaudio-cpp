#include "CriAdxParameters.h"

using namespace vgaudio::codecs;
using namespace vgaudio::codecs::criadx;

CriAdxParameters::CriAdxParameters()
    : sampleRate(48000), highPassFrequency(500), frameSize(18),
      version(4), history(0), padding(0), type(CriAdxType::Linear), filter(0) {
}

CriAdxParameters::CriAdxParameters(const CodecParameters &source)
    : MyBase(source),
      sampleRate(48000), highPassFrequency(500), frameSize(18),
      version(4), history(0), padding(0), type(CriAdxType::Linear), filter(0) {
}
