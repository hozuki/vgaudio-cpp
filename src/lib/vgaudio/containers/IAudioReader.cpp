#include "../../common/io/MemoryStream.h"
#include "AudioWithConfig.h"
#include "IAudioReader.h"

using namespace std;
using namespace vgaudio::formats;
using namespace vgaudio::containers;
using namespace common_lib::io;
using namespace common_lib::utilities;

shared_ptr<IAudioFormat> IAudioReader::readFormat(const array_ptr<uint8_t> &file) {
    auto stream = make_shared<MemoryStream>(file, false);
    return readFormat(stream);
}

shared_ptr<AudioData> IAudioReader::read(const array_ptr<uint8_t> &file) {
    auto stream = make_shared<MemoryStream>(file, false);
    return read(stream);
}

shared_ptr<AudioWithConfig> IAudioReader::readWithConfig(const array_ptr<uint8_t> &file) {
    auto stream = make_shared<MemoryStream>(file, false);
    return readWithConfig(stream);
}
