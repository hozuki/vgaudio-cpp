#pragma once

#include <cstdint>
#include <memory>

#include "../../../../common/utilities/runtime_array.h"

using namespace std;

namespace vgaudio::codecs::crihca {
    struct CriHcaFrame;
    struct CriHcaChannel;
}

namespace vgaudio::utilities {
    struct BitReader;
    struct BitWriter;
    struct Crc16;
}

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::utilities;

void ReadSpectralCoefficients(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader);

bool UnpackFrameHeader(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader);

bool UnpackingWasSuccessful(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader);

void WriteChecksum(const shared_ptr<BitWriter> &writer, const shared_ptr<Crc16> &crc, const array_ptr<uint8_t> &hcaBuffer);

void WriteSpectra(const shared_ptr<BitWriter> &writer, const shared_ptr<CriHcaChannel> &channel, int32_t subframe);

void WriteScaleFactors(const shared_ptr<BitWriter> &writer, const shared_ptr<CriHcaChannel> &channel);
