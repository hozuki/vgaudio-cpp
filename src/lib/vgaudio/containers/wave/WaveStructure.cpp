#include "WaveStructure.h"

using namespace vgaudio::containers::wave;

WaveStructure::WaveStructure()
    : channelCount(0), sampleRate(0), bitsPerSample(0), sampleCount(0), looping(false), loopStart(0), loopEnd(0) {
}
