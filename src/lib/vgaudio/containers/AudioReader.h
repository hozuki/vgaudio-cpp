#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "IAudioReader.h"
#include "../../common/io/Stream.h"
#include "../../common/io/MemoryStream.h"
#include "Configuration.h"
#include "AudioWithConfig.h"
#include "IStructure.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/type_sys.h"

namespace vgaudio::containers {

    template<
        typename TStructure, typename TConfig,
        typename Structure = typename std::enable_if<std::is_base_of<IStructure, TStructure>::value, TStructure>::type,
        typename Config = typename std::enable_if<std::is_base_of<Configuration, TConfig>::value, TConfig>::type
    >
    struct AudioReader : public IAudioReader {

        DECLARE_CLASS(AudioReader, IAudioReader);

    public:

        ~AudioReader() override = default;

        std::shared_ptr<vgaudio::formats::IAudioFormat> readFormat(const std::shared_ptr<common_lib::io::Stream> &stream) final {
            return readStream(stream)->getAudioFormat();
        }

        std::shared_ptr<vgaudio::formats::IAudioFormat> readFormat(const common_lib::utilities::array_ptr<uint8_t> &file) final {
            return readByteArray(file)->getAudioFormat();
        }

        std::shared_ptr<vgaudio::formats::AudioData> read(const std::shared_ptr<common_lib::io::Stream> &stream) final {
            return readStream(stream)->getAudio();
        }

        std::shared_ptr<vgaudio::formats::AudioData> read(const common_lib::utilities::array_ptr<uint8_t> &file) final {
            return readByteArray(file)->getAudio();
        }

        std::shared_ptr<AudioWithConfig> readWithConfig(const std::shared_ptr<common_lib::io::Stream> &stream) final {
            return readStream(stream);
        }

        std::shared_ptr<AudioWithConfig> readWithConfig(const common_lib::utilities::array_ptr<uint8_t> &file) final {
            return readByteArray(file);
        }

        std::shared_ptr<Structure> readMetadata(const std::shared_ptr<common_lib::io::Stream> &stream) {
            return readStructure(stream, false);
        }

    protected:

        virtual std::shared_ptr<Config> getConfiguration(const std::shared_ptr<Structure> &structure) {
            return std::make_shared<Config>();
        }

        std::shared_ptr<Structure> readFile(const std::shared_ptr<common_lib::io::Stream> &stream) {
            return readFile(stream, false);
        }

        virtual std::shared_ptr<Structure> readFile(const std::shared_ptr<common_lib::io::Stream> &stream, bool readAudioData) = 0;

        virtual std::shared_ptr<vgaudio::formats::IAudioFormat> toAudioStream(const std::shared_ptr<Structure> &structure) = 0;

    private:

        std::shared_ptr<AudioWithConfig> readByteArray(const common_lib::utilities::array_ptr<uint8_t> &file) {
            auto stream = std::make_shared<common_lib::io::MemoryStream>(file, false);
            return readStream(stream);
        }

        std::shared_ptr<AudioWithConfig> readStream(const std::shared_ptr<common_lib::io::Stream> &stream) {
            auto structure = readStructure(stream);

            auto audioStream = toAudioStream(structure);
            auto config = getConfiguration(structure);

            return std::make_shared<AudioWithConfig>(audioStream, config);
        }

        std::shared_ptr<Structure> readStructure(const std::shared_ptr<common_lib::io::Stream> &stream, bool readAudioData = true) {
            if (!stream->canSeek()) {
                throw std::runtime_error("a seekable stream is required");
            }

            return readFile(stream, readAudioData);
        }

    };

}
