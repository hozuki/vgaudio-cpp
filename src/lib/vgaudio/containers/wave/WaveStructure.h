#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "../IStructure.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::utilities::riff {
    struct RiffSubChunk;
}

namespace vgaudio::containers::wave {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace vgaudio::utilities::riff;

    struct WaveStructure : public IStructure {

        WaveStructure();

        ~WaveStructure() override = default;

        vector<shared_ptr<RiffSubChunk>> riffSubChunks;

        int32_t channelCount;
        int32_t sampleRate;
        int32_t bitsPerSample;
        int32_t sampleCount;
        bool looping;
        int32_t loopStart;
        int32_t loopEnd;

        jarray2_ptr<int16_t> audioData16;
        jarray2_ptr<uint8_t> audioData8;

    };

}
