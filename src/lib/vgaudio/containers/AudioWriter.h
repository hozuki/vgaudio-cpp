#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "IAudioWriter.h"
#include "../../common/io/Stream.h"
#include "../../common/io/MemoryStream.h"
#include "Configuration.h"
#include "AudioWithConfig.h"
#include "IStructure.h"
#include "../formats/AudioData.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/type_sys.h"

namespace vgaudio::containers {

    using namespace std;

    template<
        typename TConfig,
        typename Config = typename std::enable_if<std::is_base_of<Configuration, TConfig>::value, TConfig>::type
    >
    struct AudioWriter : public IAudioWriter {

        DECLARE_CLASS(AudioWriter, IAudioWriter);

    private:

        shared_ptr<Config> _configuration;
        shared_ptr<AudioData> _audioData;

    public:

        ~AudioWriter() override = default;

        void writeToStream(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Config> &config) {
            auto audioData = make_shared<AudioData>(audio);
            return writeStream(audioData, stream, config);
        }

        array_ptr<uint8_t> getFile(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Config> &config) {
            auto audioData = make_shared<AudioData>(audio);
            return getByteArray(audioData, config);
        }

        void writeToStream(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Config> &config) {
            return writeStream(audio, stream, config);
        }

        array_ptr<uint8_t> getFile(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Config> &config) {
            return getByteArray(audio, config);
        }

    protected:

        AudioWriter() {
            _configuration = make_shared<Config>();
        }

        [[nodiscard]]
        shared_ptr<AudioData> getAudioStream() const {
            return _audioData;
        }

        void setAudioStream(const shared_ptr<AudioData> &audioData) {
            _audioData = audioData;
        }

        [[nodiscard]]
        shared_ptr<Config> getConfiguration() const {
            return _configuration;
        }

        void setConfiguration(const shared_ptr<Config> &configuration) {
            _configuration = configuration;
        }

        virtual int32_t getFileSize() = 0;

        virtual void setupWriter(const shared_ptr<AudioData> &audio) = 0;

        virtual void writeStream(const shared_ptr<Stream> &stream) = 0;

    private:

        void writeToStream(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) final {
            auto cfg = dynamic_pointer_cast<Config>(config);
            return writeToStream(audio, stream, cfg);
        }

        array_ptr<uint8_t> getFile(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) final {
            auto cfg = dynamic_pointer_cast<Config>(config);
            return getFile(audio, stream, cfg);
        }

        void writeToStream(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) final {
            auto cfg = dynamic_pointer_cast<Config>(config);
            return writeToStream(audio, stream, cfg);
        }

        array_ptr<uint8_t> getFile(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) final {
            auto cfg = dynamic_pointer_cast<Config>(config);
            return getFile(audio, stream, cfg);
        }

        array_ptr<uint8_t> getByteArray(const shared_ptr<AudioData> &audio, const shared_ptr<Config> &config) {
            if (config != nullptr) {
                setConfiguration(config);
            }

            setupWriter(audio);

            shared_ptr<MemoryStream> stream = nullptr;
            array_ptr<uint8_t> file = nullptr;

            const auto fileSize = getFileSize();

            if (fileSize < 0) {
                stream = make_shared<MemoryStream>();
            } else {
                file = make_array_dynamic<uint8_t>(fileSize);
                stream = make_shared<MemoryStream>(file);
            }

            writeStream(stream);

            if (fileSize < 0) {
                return stream->toArray();
            } else {
                return file;
            }
        }

        void writeStream(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Config> &config) {
            if (config != nullptr) {
                setConfiguration(config);
            }

            setupWriter(audio);

            writeStream(stream);
        }

    };

}
