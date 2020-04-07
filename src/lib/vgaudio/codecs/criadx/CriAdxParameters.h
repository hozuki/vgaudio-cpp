#pragma once

#include "../CodecParameters.h"
#include "CriAdxType.h"

namespace vgaudio::codecs::criadx {

    struct CriAdxParameters : public CodecParameters {

        DECLARE_CLASS(CriAdxParameters, CodecParameters);

    public:

        CriAdxParameters();

        explicit CriAdxParameters(const CodecParameters &source);

        CriAdxParameters(const CriAdxParameters &) = default;

        CriAdxParameters(CriAdxParameters &&) = default;

        ~CriAdxParameters() override = default;

        int32_t sampleRate;
        int32_t highPassFrequency;
        int32_t frameSize;
        int32_t version;
        int16_t history;
        int32_t padding;
        CriAdxType type;
        int32_t filter;

    };

}
