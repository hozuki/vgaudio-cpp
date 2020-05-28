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

namespace vgaudio {
    namespace formats {

        struct IAudioFormat;

        struct AudioData final {

        private:

            std::unordered_map<std::type_index, std::shared_ptr<IAudioFormat>> _formats;

        public:

            explicit AudioData(const std::shared_ptr<IAudioFormat> &format);

            ~AudioData() = default;

            std::vector<std::shared_ptr<IAudioFormat>> getAllFormats();

            std::vector<std::type_index> listAvailableFormats();

            void setLoop(bool loop);

            void setLoop(bool loop, int32_t loopStart, int32_t loopEnd);

            static std::shared_ptr<AudioData> combine(const common_lib::utilities::array_ptr<std::shared_ptr<AudioData>> &audio);

        private:

            void addFormat(const std::shared_ptr<IAudioFormat> &format);

            void createPcm16(const std::shared_ptr<vgaudio::codecs::CodecParameters> &config = nullptr);

            template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
            static std::shared_ptr<AudioFormat> getAudioFormat(const std::shared_ptr<AudioData> &audioData) {
                const auto key = std::type_index(typeid(AudioFormat));
                auto iter = audioData->_formats.find(key);

                if (iter == audioData->_formats.end()) {
                    return nullptr;
                } else {
                    return std::dynamic_pointer_cast<AudioFormat>(iter->second);
                }
            }

            template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
            static void createFormat(const std::shared_ptr<AudioData> &audioData, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config = nullptr) {
                auto pcm = getAudioFormat<vgaudio::formats::pcm16::Pcm16Format>(audioData);
                auto format = std::dynamic_pointer_cast<IAudioFormat>(std::make_shared<AudioFormat>());
                auto encoded = format->encodeFromPcm16(pcm, config);
                audioData->addFormat(encoded);
            }

        public:

            template<typename TAudioFormat, typename AudioFormat = typename std::enable_if<std::is_base_of<IAudioFormat, TAudioFormat>::value, TAudioFormat>::type>
            static std::shared_ptr<AudioFormat> getFormat(const std::shared_ptr<AudioData> &audioData, const std::shared_ptr<vgaudio::codecs::CodecParameters> &config = nullptr) {
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
}
