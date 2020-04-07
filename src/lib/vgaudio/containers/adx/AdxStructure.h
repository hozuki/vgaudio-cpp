#pragma once

#include <memory>

#include "../IStructure.h"
#include "../../codecs/criadx/CriAdxType.h"
#include "../../../common/utilities/runtime_jagged_array.h"

namespace vgaudio::codecs::criadx {
    struct CriAdxKey;
}

namespace vgaudio::containers::adx {

    struct AdxStructure : public IStructure {

        AdxStructure();

        ~AdxStructure() override = default;

        int16_t headerSize;
        vgaudio::codecs::criadx::CriAdxType encodingType;
        std::shared_ptr<vgaudio::codecs::criadx::CriAdxKey> encryptionKey;
        uint8_t frameSize;
        uint8_t bitDepth;
        uint8_t channelCount;
        int32_t sampleRate;
        int32_t sampleCount;
        int16_t highPassFrequency;
        uint8_t version;
        uint8_t revision;
        int16_t insertedSamples;
        int32_t loopCount;
        bool looping;
        int32_t loopType;
        int32_t loopStartSample;
        int32_t loopStartByte;
        int32_t loopEndSample;
        int32_t loopEndByte;
        common_lib::utilities::jarray2_ptr<int16_t> historySamples;
        common_lib::utilities::jarray2_ptr<uint8_t> audioData;

        [[nodiscard]]
        int32_t getSamplesPerFrame() const;

        [[nodiscard]]
        int32_t getAudioDataLength() const;

    };

}
