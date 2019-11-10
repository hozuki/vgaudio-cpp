#pragma once

#include <cstdint>
#include <string>

#include "CriHcaKeyType.h"

namespace vgaudio::codecs::crihca {

    struct HcaInfo final {

        HcaInfo();

        int32_t channelCount;
        int32_t sampleRate;
        int32_t sampleCount;
        int32_t frameCount;
        int32_t insertedSamples;
        int32_t appendedSamples;
        int32_t headerSize;

        int32_t frameSize;
        int32_t minResolution;
        int32_t maxResolution;
        int32_t trackCount;
        int32_t channelConfig;
        int32_t totalBandCount;
        int32_t baseBandCount;
        int32_t stereoBandCount;
        int32_t hfrBandCount;
        int32_t bandsPerHfrGroup;
        int32_t hfrGroupCount;

        int32_t decStereoType;

        bool looping;
        int32_t loopStartFrame;
        int32_t loopEndFrame;
        int32_t preLoopSamples;
        int32_t postLoopSamples;

        bool useAthCurve;

        int32_t vbrMaxFrameSize;
        int32_t vbrNoiseLevel;

        CriHcaKeyType encryptionType;

        float volume;

        void calculateHfrValues();

        std::string comment;

        [[nodiscard]]
        int32_t getLoopStartSample() const;

        [[nodiscard]]
        int32_t getLoopEndSample() const;

        [[nodiscard]]
        size_t getCommentLength() const;

    };

}
