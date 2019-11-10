#include <stdexcept>

#include "AudioFormatBaseBuilder.h"

using namespace std;
using namespace vgaudio::formats;

AudioFormatBaseBuilder::AudioFormatBaseBuilder()
    : _looping(false), _loopStart(0), _loopEnd(0),
      _sampleCount(0), _sampleRate(0) {
}

decltype(AudioFormatBaseBuilder::_looping) AudioFormatBaseBuilder::isLooping() const {
    return _looping;
}

void AudioFormatBaseBuilder::setLooping(decltype(_looping) looping) {
    _looping = looping;
}

decltype(AudioFormatBaseBuilder::_loopStart) AudioFormatBaseBuilder::getLoopStart() const {
    return _loopStart;
}

void AudioFormatBaseBuilder::setLoopStart(decltype(_loopStart) start) {
    _loopStart = start;
}

decltype(AudioFormatBaseBuilder::_loopEnd) AudioFormatBaseBuilder::getLoopEnd() const {
    return _loopEnd;
}

void AudioFormatBaseBuilder::setLoopEnd(decltype(_loopEnd) end) {
    _loopEnd = end;
}

decltype(AudioFormatBaseBuilder::_sampleCount) AudioFormatBaseBuilder::getSampleCount() const {
    return _sampleCount;
}

void AudioFormatBaseBuilder::setSampleCount(decltype(_sampleCount) count) {
    _sampleCount = count;
}

void AudioFormatBaseBuilder::setTracks(const shared_ptr<list<shared_ptr<AudioTrack>>> &tracks) {
    _tracks = tracks;
}

decltype(AudioFormatBaseBuilder::_sampleRate) AudioFormatBaseBuilder::getSampleRate() const {
    return _sampleRate;
}

void AudioFormatBaseBuilder::setSampleRate(decltype(_sampleRate) sampleRate) {
    _sampleRate = sampleRate;
}

decltype(AudioFormatBaseBuilder::_tracks) AudioFormatBaseBuilder::getTracks() const {
    return _tracks;
}

void AudioFormatBaseBuilder::withLoop(bool loop) {
    setLooping(loop);
    setLoopStart(0);
    setLoopEnd(loop ? getSampleCount() : 0);
}

void AudioFormatBaseBuilder::withLoop(bool loop, int32_t loopStart, int32_t loopEnd) {
    if (!loop) {
        return withLoop(false);
    }

    if (loopStart < 0 || loopStart > getSampleCount()) {
        throw std::invalid_argument("bad loop start");
    }

    if (loopEnd < 0 || loopEnd > getSampleCount()) {
        throw std::invalid_argument("bad loop end");
    }

    if (loopEnd < loopStart) {
        throw std::invalid_argument("loop end should be after loop start");
    }

    setLooping(true);
    setLoopStart(loopStart);
    setLoopEnd(loopEnd);
}

void AudioFormatBaseBuilder::withTracks(const shared_ptr<list<shared_ptr<AudioTrack>>> &tracks) {
    setTracks(tracks);
}
