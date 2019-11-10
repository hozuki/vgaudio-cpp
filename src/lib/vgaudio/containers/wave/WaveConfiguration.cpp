#include "WaveConfiguration.h"

using namespace vgaudio::containers;
using namespace vgaudio::containers::wave;

WaveConfiguration::WaveConfiguration()
    : codec(WaveCodec::Pcm16Bit) {
}

WaveConfiguration::WaveConfiguration(const Configuration &other)
    : MyBase(other), codec(WaveCodec::Pcm16Bit) {
}
