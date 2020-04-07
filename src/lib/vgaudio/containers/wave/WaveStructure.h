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

    struct WaveStructure : public IStructure {

        WaveStructure();

        ~WaveStructure() override = default;

        std::vector<std::shared_ptr<vgaudio::utilities::riff::RiffSubChunk>> riffSubChunks;

        int32_t channelCount;
        int32_t sampleRate;
        int32_t bitsPerSample;
        int32_t sampleCount;
        bool looping;
        int32_t loopStart;
        int32_t loopEnd;

        common_lib::utilities::jarray2_ptr<int16_t> audioData16;
        common_lib::utilities::jarray2_ptr<uint8_t> audioData8;

    };

}
