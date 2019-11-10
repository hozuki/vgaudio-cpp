#include "../../common/io/MemoryStream.h"
#include "IAudioWriter.h"

using namespace std;
using namespace vgaudio::formats;
using namespace vgaudio::containers;
using namespace common_lib::io;
using namespace common_lib::utilities;

array_ptr<uint8_t> IAudioWriter::getFile(const shared_ptr<IAudioFormat> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) {
    auto memory = make_shared<MemoryStream>();
    writeToStream(audio, stream, config);
    return memory->toArray();
}

array_ptr<uint8_t> IAudioWriter::getFile(const shared_ptr<AudioData> &audio, const shared_ptr<Stream> &stream, const shared_ptr<Configuration> &config) {
    auto memory = make_shared<MemoryStream>();
    writeToStream(audio, stream, config);
    return memory->toArray();
}
