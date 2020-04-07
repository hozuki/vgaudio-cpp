#include "CriAdxChannel.h"

using namespace common_lib::utilities;
using namespace vgaudio::formats::criadx;

CriAdxChannel::CriAdxChannel(const array_ptr<uint8_t> &audio, int16_t history, int32_t version)
    : audio(audio), history(history), version(version) {
}
