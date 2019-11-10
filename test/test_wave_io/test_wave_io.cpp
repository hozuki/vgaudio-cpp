#include <cstdint>

#include "../../src/lib/common/io/FileStream.h"
#include "../../src/lib/vgaudio/formats/pcm16/Pcm16Format.h"
#include "../../src/lib/vgaudio/containers/wave/WaveConfiguration.h"
#include "../../src/lib/vgaudio/containers/wave/WaveReader.h"
#include "../../src/lib/vgaudio/containers/wave/WaveWriter.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::formats::pcm16;
using namespace vgaudio::containers::wave;

int main() {
    const char *fileIn = R"(C:\Users\MIC\Desktop\Alstroemeria.wav)";
    const char *fileOut = R"(C:\Users\MIC\Desktop\als1.wav)";

    auto inStream = make_shared<FileStream>(fileIn, "rb");
    auto outStream = make_shared<FileStream>(fileOut, "wb");

    auto reader = make_shared<WaveReader>();
    auto writer = make_shared<WaveWriter>();

    auto wave = reader->readWithConfig(inStream);
    auto waveConfig = make_shared<WaveConfiguration>(*wave->getConfiguration());

    writer->writeToStream(wave->getAudio(), outStream, waveConfig);

    printf("Done.\n");

    return 0;
}