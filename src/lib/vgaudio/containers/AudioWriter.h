#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "../../common/utilities/_cxx17.h"
#include "IAudioWriter.h"
#include "../../common/io/Stream.h"
#include "../../common/io/MemoryStream.h"
#include "Configuration.h"
#include "../formats/AudioData.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/type_sys.h"

namespace vgaudio {
    namespace containers {

        template<
            typename TConfig,
            typename Config = typename std::enable_if<std::is_base_of<Configuration, TConfig>::value, TConfig>::type
        >
        struct AudioWriter : public IAudioWriter {

            DECLARE_CLASS(AudioWriter, IAudioWriter);

        private:

            std::shared_ptr<Config> _configuration;
            std::shared_ptr<vgaudio::formats::AudioData> _audioData;

        public:

            ~AudioWriter() override = default;

            void writeToStream(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Config> &config) {
                auto audioData = std::make_shared<vgaudio::formats::AudioData>(audio);
                return writeStream(audioData, stream, config);
            }

            common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Config> &config) {
                auto audioData = std::make_shared<vgaudio::formats::AudioData>(audio);
                return getByteArray(audioData, config);
            }

            void writeToStream(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Config> &config) {
                return writeStream(audio, stream, config);
            }

            common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Config> &config) {
                return getByteArray(audio, config);
            }

        protected:

            AudioWriter() {
                _configuration = std::make_shared<Config>();
            }

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<vgaudio::formats::AudioData> getAudioStream() const {
                return _audioData;
            }

            void setAudioStream(const std::shared_ptr<vgaudio::formats::AudioData> &audioData) {
                _audioData = audioData;
            }

            _CXX17_ATTR_NODISCARD
            std::shared_ptr<Config> getConfiguration() const {
                return _configuration;
            }

            void setConfiguration(const std::shared_ptr<Config> &configuration) {
                _configuration = configuration;
            }

            virtual int32_t getFileSize() = 0;

            virtual void setupWriter(const std::shared_ptr<vgaudio::formats::AudioData> &audio) = 0;

            virtual void writeStream(const std::shared_ptr<common_lib::io::Stream> &stream) = 0;

        private:

            void writeToStream(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) final {
                auto cfg = std::dynamic_pointer_cast<Config>(config);
                return writeToStream(audio, stream, cfg);
            }

            common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::IAudioFormat> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) final {
                auto cfg = std::dynamic_pointer_cast<Config>(config);
                return getFile(audio, stream, cfg);
            }

            void writeToStream(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) final {
                auto cfg = std::dynamic_pointer_cast<Config>(config);
                return writeToStream(audio, stream, cfg);
            }

            common_lib::utilities::array_ptr<uint8_t> getFile(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Configuration> &config) final {
                auto cfg = std::dynamic_pointer_cast<Config>(config);
                return getFile(audio, stream, cfg);
            }

            common_lib::utilities::array_ptr<uint8_t> getByteArray(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<Config> &config) {
                if (config != nullptr) {
                    setConfiguration(config);
                }

                setupWriter(audio);

                std::shared_ptr<common_lib::io::MemoryStream> stream = nullptr;
                common_lib::utilities::array_ptr<uint8_t> file = nullptr;

                const auto fileSize = getFileSize();

                if (fileSize < 0) {
                    stream = std::make_shared<common_lib::io::MemoryStream>();
                } else {
                    file = common_lib::utilities::make_array_dynamic<uint8_t>(fileSize);
                    stream = std::make_shared<common_lib::io::MemoryStream>(file);
                }

                writeStream(stream);

                if (fileSize < 0) {
                    return stream->toArray();
                } else {
                    return file;
                }
            }

            void writeStream(const std::shared_ptr<vgaudio::formats::AudioData> &audio, const std::shared_ptr<common_lib::io::Stream> &stream, const std::shared_ptr<Config> &config) {
                if (config != nullptr) {
                    setConfiguration(config);
                }

                setupWriter(audio);

                writeStream(stream);
            }

        };

    }
}
