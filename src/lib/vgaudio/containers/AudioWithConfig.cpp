#include "Configuration.h"
#include "../formats/IAudioFormat.h"
#include "../formats/AudioData.h"
#include "AudioWithConfig.h"

using namespace std;
using namespace vgaudio::formats;
using namespace vgaudio::containers;

AudioWithConfig::AudioWithConfig(const shared_ptr<IAudioFormat> &audioFormat, const shared_ptr<Configuration> &config)
    : _audioFormat(audioFormat), _configuration(config) {
}

shared_ptr<IAudioFormat> AudioWithConfig::getAudioFormat() const {
    return _audioFormat;
}

shared_ptr<Configuration> AudioWithConfig::getConfiguration() const {
    return _configuration;
}

shared_ptr<AudioData> AudioWithConfig::getAudio() const {
    return make_shared<AudioData>(getAudioFormat());
}
