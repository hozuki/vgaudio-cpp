#include <algorithm>

#include "../../common/utilities/macros.h"
#include "AudioData.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm16;
using namespace vgaudio::codecs;

AudioData::AudioData(const shared_ptr<IAudioFormat> &format) {
    addFormat(format);
}

void AudioData::addFormat(const shared_ptr<IAudioFormat> &format) {
    // Note that we have to unwrap this smart pointer to get the underlying dynamic type
    auto key = type_index(typeid(*format));
    _formats[key] = format;
}

vector<shared_ptr<IAudioFormat>> AudioData::getAllFormats() {
    vector<shared_ptr<IAudioFormat>> result(_formats.size());

    size_t i = 0;
    for (const auto &kv : _formats) {
        result[i] = kv.second;
        i += 1;
    }

    return result;
}

vector<type_index> AudioData::listAvailableFormats() {
    vector<type_index> result;
    result.reserve(_formats.size());

    for (const auto &kv : _formats) {
        result.push_back(kv.first);
    }

    return result;
}

void AudioData::setLoop(bool loop) {
    auto keys = listAvailableFormats();

    for (const auto key :keys) {
        _formats[key] = _formats[key]->withLoop(loop);
    }
}

void AudioData::setLoop(bool loop, int32_t loopStart, int32_t loopEnd) {
    auto keys = listAvailableFormats();

    for (const auto key :keys) {
        _formats[key] = _formats[key]->withLoop(loop, loopStart, loopEnd);
    }
}

void AudioData::createPcm16(const shared_ptr<CodecParameters> &config) {
    auto ptr = shared_ptr<AudioData>(this, DONT_DELETE(AudioData));

    if (getAudioFormat<Pcm16Format>(ptr) == nullptr) {
        auto iter = _formats.begin();
        auto firstFormat = iter->second;
        auto pcm16 = firstFormat->toPcm16(config);
        addFormat(pcm16);
    }
}

shared_ptr<AudioData> AudioData::combine(const array_ptr<shared_ptr<AudioData>> &audio) {
    if (audio == nullptr) {
        throw std::invalid_argument("audio cannot be null");
    }

    if (audio->empty()) {
        throw std::invalid_argument("audio cannot be empty");
    }

    for (auto x : *audio) {
        if (x == nullptr) {
            throw std::invalid_argument("audio cannot contain null elements");
        }
    }

    vector<type_index> commonTypes;

    do {
        vector<vector<type_index>> allAvailableFormats(audio->size());

        size_t counter = 0;
        for (const auto &a : *audio) {
            allAvailableFormats[counter] = a->listAvailableFormats();
            counter += 1;
        }

        if (allAvailableFormats.empty()) {
            break;
        }

        if (allAvailableFormats.size() == 1) {
            commonTypes = *allAvailableFormats.begin();
            break;
        }

        // Do some intersection!
        // TODO: can be optimized
        vector<type_index> tmp1 = *allAvailableFormats.begin(), tmp2;
        vector<type_index> *pTmp = &tmp1, *pResult = &tmp2;
        vector<type_index>::iterator ii;

        for (auto it = allAvailableFormats.begin() + 1; it != allAvailableFormats.end(); it++) {
            pResult->clear();
            pResult->reserve(it->size() + pTmp->size());
            ii = std::set_intersection(it->begin(), it->begin() + it->size(), pTmp->begin(), pTmp->begin() + pTmp->size(), pResult->begin());
            std::swap(pTmp, pResult);
        }

        for (auto i = pResult->begin(); i != ii; i++) {
            commonTypes.push_back(*i);
        }
    } while (false);

    std::hash<type_index> hashFunc;
    auto formatToUse = type_index(typeid(void));

    if (commonTypes.empty() || (commonTypes.size() == 1 && hashFunc(commonTypes[0]) == hashFunc(type_index(typeid(Pcm16Format))))) {
        formatToUse = type_index(typeid(Pcm16Format));

        for (auto a : *audio) {
            a->createPcm16();
        }
    } else {
        bool found = false;

        for (auto type : commonTypes) {
            if (hashFunc(type) != hashFunc(type_index(typeid(Pcm16Format)))) {
                found = true;
                formatToUse = type;
            }
        }

        if (!found) {
            throw std::runtime_error("did not find a proper format to use");
        }
    }

    shared_ptr<IAudioFormat> &combined = (*audio)[0]->_formats[formatToUse];

    for (auto it = audio->begin() + 1; it != audio->end(); it++) {
        if (!combined->tryAdd((*it)->_formats[formatToUse], combined)) {
            throw std::runtime_error("audio streams cannot be added together");
        }
    }

    return make_shared<AudioData>(combined);
}
