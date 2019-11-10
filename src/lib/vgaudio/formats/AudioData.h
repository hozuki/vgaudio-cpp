#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

#include "pcm16/Pcm16Format.h"
#include "../codecs/CodecParameters.h"
#include "../../common/utilities/runtime_array.h"

namespace vgaudio::formats {

    struct IAudioFormat;

    using namespace std;
    using namespace vgaudio::codecs;
    using namespace common_lib::utilities;
    using namespace vgaudio::formats::pcm16;

    struct AudioData final {

    private:

        unordered_map<type_index, shared_ptr<IAudioFormat>> _formats;

    public:

        explicit AudioData(const shared_ptr<IAudioFormat> &format);

        ~AudioData() = default;

        vector<shared_ptr<IAudioFormat>> getAllFormats();

        vector<type_index> listAvailableFormats();

        void setLoop(bool loop);

        void setLoop(bool loop, int32_t loopStart, int32_t loopEnd);

        static shared_ptr<AudioData> combine(const array_ptr<shared_ptr<AudioData>> &audio);

    private:

        void addFormat(const shared_ptr<IAudioFormat> &format);

        void createPcm16(const shared_ptr<CodecParameters> &config = nullptr);

        template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
        static shared_ptr<AudioFormat> getAudioFormat(const shared_ptr<AudioData> &audioData) {
            const auto key = type_index(typeid(AudioFormat));
            auto iter = audioData->_formats.find(key);

            if (iter == audioData->_formats.end()) {
                return nullptr;
            } else {
                return dynamic_pointer_cast<AudioFormat>(iter->second);
            }
        }

        template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
        static void createFormat(const shared_ptr<AudioData> &audioData, const shared_ptr<CodecParameters> &config = nullptr) {
            auto pcm = getAudioFormat<Pcm16Format>(audioData);
            auto format = dynamic_pointer_cast<IAudioFormat>(make_shared<AudioFormat>());
            auto encoded = format->encodeFromPcm16(pcm, config);
            audioData->addFormat(encoded);
        }

    public:

        template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
        static shared_ptr<AudioFormat> getFormat(const shared_ptr<AudioData> &audioData, const shared_ptr<CodecParameters> &config = nullptr) {
            auto format = getAudioFormat<TAudioFormat, AudioFormat>(audioData);

            if (format != nullptr) {
                return format;
            }

            audioData->createPcm16(config);
            createFormat<TAudioFormat, AudioFormat>(audioData, config);

            return getAudioFormat<TAudioFormat, AudioFormat>(audioData);
        }

    };

}
