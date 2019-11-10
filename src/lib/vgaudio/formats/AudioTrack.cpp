#include <cmath>
#include <algorithm>

#include "../../common/utilities/IntHelper.h"
#include "AudioTrack.h"

using namespace std;
using namespace vgaudio::formats;
using namespace common_lib::utilities;

AudioTrack::AudioTrack()
    : _channelCount(0), _channelLeft(0), _channelRight(0),
      _panning(0x40), _volume(0x7f), _surroundPanning(0), _flags(0) {
}

AudioTrack::AudioTrack(int32_t channelCount, int32_t channelLeft, int32_t channelRight, int32_t panning, int32_t volume)
    : AudioTrack() {
    _channelCount = channelCount;
    _channelLeft = channelLeft;
    _channelRight = channelRight;
    _panning = panning;
    _volume = volume;
}

AudioTrack::AudioTrack(int32_t channelCount, int32_t channelLeft, int32_t channelRight)
    : AudioTrack() {
    _channelCount = channelCount;
    _channelLeft = channelLeft;
    _channelRight = channelRight;
}

int32_t AudioTrack::getChannelCount() const {
    return _channelCount;
}

void AudioTrack::setChannelCount(int32_t count) {
    _channelCount = count;
}

int32_t AudioTrack::getChannelLeft() const {
    return _channelLeft;
}

void AudioTrack::setChannelLeft(int32_t count) {
    _channelLeft = count;
}

int32_t AudioTrack::getChannelRight() const {
    return _channelRight;
}

void AudioTrack::setChannelRight(int32_t count) {
    _channelRight = count;
}

int32_t AudioTrack::getPanning() const {
    return _panning;
}

void AudioTrack::setPanning(int32_t panning) {
    _panning = panning;
}

int32_t AudioTrack::getVolume() const {
    return _volume;
}

void AudioTrack::setVolume(int32_t volume) {
    _volume = volume;
}

int32_t AudioTrack::getSurroundPanning() const {
    return _surroundPanning;
}

void AudioTrack::setSurroundPanning(int32_t panning) {
    _surroundPanning = panning;
}

int32_t AudioTrack::getFlags() const {
    return _flags;
}

void AudioTrack::setFlags(int32_t flags) {
    _flags = flags;
}

shared_ptr<list<shared_ptr<AudioTrack>>> AudioTrack::getDefaultTrackList(int32_t channelCount) {
    const auto trackCount = IntHelper::divideByRoundUp(channelCount, 2);

    auto result = make_shared<std::result_of<decltype(AudioTrack::getDefaultTrackList) &(int32_t)>::type::element_type>();

    for (auto i = 0; i < trackCount; i += 1) {
        const auto trackChannelCount = std::min(channelCount - i * 2, 2);

        auto track = new AudioTrack(channelCount, i * 2, trackChannelCount >= 2 ? i * 2 + 1 : 0);
        auto trackPtr = shared_ptr<AudioTrack>(track);

        result->push_back(trackPtr);
    }

    return result;
}
