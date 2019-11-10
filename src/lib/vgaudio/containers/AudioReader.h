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

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    template<
        typename TStructure, typename TConfig,
        typename Structure = typename std::enable_if<std::is_base_of<IStructure, TStructure>::value, TStructure>::type,
        typename Config = typename std::enable_if<std::is_base_of<Configuration, TConfig>::value, TConfig>::type
    >
    struct AudioReader : public IAudioReader {

        DECLARE_CLASS(AudioReader, IAudioReader);

    public:

        ~AudioReader() override = default;

        shared_ptr<IAudioFormat> readFormat(const shared_ptr<Stream> &stream) final {
            return readStream(stream)->getAudioFormat();
        }

        shared_ptr<IAudioFormat> readFormat(const array_ptr<uint8_t> &file) final {
            return readByteArray(file)->getAudioFormat();
        }

        shared_ptr<AudioData> read(const shared_ptr<Stream> &stream) final {
            return readStream(stream)->getAudio();
        }

        shared_ptr<AudioData> read(const array_ptr<uint8_t> &file) final {
            return readByteArray(file)->getAudio();
        }

        shared_ptr<AudioWithConfig> readWithConfig(const shared_ptr<Stream> &stream) final {
            return readStream(stream);
        }

        shared_ptr<AudioWithConfig> readWithConfig(const array_ptr<uint8_t> &file) final {
            return readByteArray(file);
        }

        shared_ptr<Structure> readMetadata(const shared_ptr<Stream> &stream) {
            return readStructure(stream, false);
        }

    protected:

        virtual shared_ptr<Config> getConfiguration(const shared_ptr<Structure> &structure) {
            return make_shared<Config>();
        }

        shared_ptr<Structure> readFile(const shared_ptr<Stream> &stream) {
            return readFile(stream, false);
        }

        virtual shared_ptr<Structure> readFile(const shared_ptr<Stream> &stream, bool readAudioData) = 0;

        virtual shared_ptr<IAudioFormat> toAudioStream(const shared_ptr<Structure> &structure) = 0;

    private:

        shared_ptr<AudioWithConfig> readByteArray(const array_ptr<uint8_t> &file) {
            auto stream = make_shared<MemoryStream>(file, false);
            return readStream(stream);
        }

        shared_ptr<AudioWithConfig> readStream(const shared_ptr<Stream> &stream) {
            auto structure = readStructure(stream);

            auto audioStream = toAudioStream(structure);
            auto config = getConfiguration(structure);

            return make_shared<AudioWithConfig>(audioStream, config);
        }

        shared_ptr<Structure> readStructure(const shared_ptr<Stream> &stream, bool readAudioData = true) {
            if (!stream->canSeek()) {
                throw std::runtime_error("a seekable stream is required");
            }

            return readFile(stream, readAudioData);
        }

    };

}
