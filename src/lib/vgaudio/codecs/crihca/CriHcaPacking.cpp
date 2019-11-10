#include "CriHcaTables.h"
#include "CriHcaConstants.h"
#include "CriHcaChannel.h"
#include "CriHcaFrame.h"
#include "HcaInfo.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "../../utilities/BitWriter.h"
#include "../../utilities/BitReader.h"
#include "../../utilities/Crc16.h"
#include "CriHcaPacking.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::utilities;

using C = CriHcaConstants;

static bool DeltaDecode(const shared_ptr<BitReader> &reader, int32_t deltaBits, int32_t dataBits, int32_t count, const narray_ptr<int32_t, C::SamplesPerSubframe> &output) {
    (*output)[0] = reader->readInt(dataBits);

    const auto maxDelta = 1 << (deltaBits - 1);
    const auto maxValue = (1 << dataBits) - 1;

    for (auto i = 1; i < count; i += 1) {
        const int32_t delta = reader->readOffsetBinary(deltaBits, OffsetBias::Positive);

        if (delta < maxDelta) {
            const int32_t value = (*output)[i - 1] + delta;

            if (value < 0 || value > maxValue) {
                return false;
            }

            (*output)[i] = value;
        } else {
            (*output)[i] = reader->readInt(dataBits);
        }
    }

    return true;
}

static bool ReadScaleFactors(const shared_ptr<CriHcaChannel> &channel, const shared_ptr<BitReader> &reader) {
    channel->scaleFactorDeltaBits = reader->readInt(3);

    if (channel->scaleFactorDeltaBits == 0) {
        ArrayHelper::clearByValue(channel->scaleFactors, 0, channel->scaleFactors->size());
        return true;
    }

    if (channel->scaleFactorDeltaBits >= 6) {
        for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
            (*channel->scaleFactors)[i] = reader->readInt(6);
        }

        return true;
    }

    return DeltaDecode(reader, channel->scaleFactorDeltaBits, 6, channel->codedScaleFactorCount, channel->scaleFactors);
}

static void ReadIntensity(const shared_ptr<BitReader> &reader, const narray_ptr<int32_t, C::SubframesPerFrame> &intensity) {
    for (auto i = 0; i < C::SubframesPerFrame; i += 1) {
        (*intensity)[i] = reader->readInt(4);
    }
}

static void ReadHfrScaleFactors(const shared_ptr<BitReader> &reader, int32_t groupCount, const narray_ptr<int32_t, 8> &hfrScale) {
    for (auto i = 0; i < groupCount; i += 1) {
        (*hfrScale)[i] = reader->readInt(6);
    }
}

static void ReadSpectralCoefficients(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader) {
    const auto &quantizedSpectrumMaxBits = CriHcaTables::getQuantizedSpectrumMaxBits();
    const auto &quantizedSpectrumBits = CriHcaTables::getQuantizedSpectrumBits();
    const auto &quantizedSpectrumValue = CriHcaTables::getQuantizedSpectrumValue();

    for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
        for (const auto &channel : *frame->getChannels()) {
            for (auto s = 0; s < channel->codedScaleFactorCount; s += 1) {
                const auto resolution = (*channel->resolution)[s];
                int32_t bits = (*quantizedSpectrumMaxBits)[resolution];
                const auto code = reader->PeekInt(bits);

                if (resolution < 8) {
                    bits = (*(*quantizedSpectrumBits)[resolution])[code];
                    (*(*channel->quantizedSpectra)[sf])[s] = (*(*quantizedSpectrumValue)[resolution])[code];
                } else {
                    const auto quantizedCoefficient = code / 2 * (1 - (code % 2 * 2));

                    if (quantizedCoefficient == 0) {
                        bits -= 1;
                    }

                    (*(*channel->quantizedSpectra)[sf])[s] = quantizedCoefficient;
                }

                reader->setPosition(reader->getPosition() + bits);
            }

            ArrayHelper::clearByValue((*channel->spectra)[sf], channel->codedScaleFactorCount, 0x80 - channel->codedScaleFactorCount);
        }
    }
}

static bool UnpackFrameHeader(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader) {
    const auto syncWord = reader->readInt(16);

    if (syncWord != 0xffff) {
        throw std::runtime_error("invalid frame header");
    }

    const auto &athCurve = frame->getAthCurve();
    frame->setAcceptableNoiseLevel(reader->readInt(9));
    frame->setEvaluationBoundary(reader->readInt(7));

    for (const auto &channel : *frame->getChannels()) {
        if (!ReadScaleFactors(channel, reader)) {
            return false;
        }

        const auto evalBound = frame->getEvaluationBoundary();

        for (auto i = 0; i < evalBound; i += 1) {
            (*channel->resolution)[i] = CriHcaPacking::calculateResolution((*channel->scaleFactors)[i], (*athCurve)[i] + frame->getAcceptableNoiseLevel() - 1);
        }

        for (auto i = evalBound; i < channel->codedScaleFactorCount; i += 1) {
            (*channel->resolution)[i] = CriHcaPacking::calculateResolution((*channel->scaleFactors)[i], (*athCurve)[i] + frame->getAcceptableNoiseLevel());
        }

        if (channel->type == ChannelType::StereoSecondary) {
            ReadIntensity(reader, channel->intensity);
        } else {
            const auto hfrGroupCount = frame->getHca()->hfrGroupCount;

            if (hfrGroupCount > 0) {
                ReadHfrScaleFactors(reader, hfrGroupCount, channel->hfrScales);
            }
        }
    }

    return true;
}

static bool IsFrameEmpty(const shared_ptr<CriHcaFrame> &frame) {
    if (frame->getAcceptableNoiseLevel() > 0) {
        return false;
    }

    for (const auto &channel : *frame->getChannels()) {
        if (channel->scaleFactorDeltaBits > 0) {
            return false;
        }
    }

    return true;
}

static bool UnpackingWasSuccessful(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader) {
    return (reader->getRemaining() >= 16 && reader->getRemaining() <= 128) ||
           IsFrameEmpty(frame) ||
           (frame->getAcceptableNoiseLevel() == 0 && reader->getRemaining() >= 16);
}

static void WriteChecksum(const shared_ptr<BitWriter> &writer, const shared_ptr<Crc16> &crc, const array_ptr<uint8_t> &hcaBuffer) {
    writer->setPosition(writer->getLengthBits() - 16);
    const auto crc16 = crc->compute(hcaBuffer, hcaBuffer->size() - 2);
    writer->write(crc16, 16);
}

static void WriteSpectra(const shared_ptr<BitWriter> &writer, const shared_ptr<CriHcaChannel> &channel, int32_t subframe) {
    const auto &quantizeSpectrumBits = CriHcaTables::getQuantizeSpectrumBits();
    const auto &quantizeSpectrumValue = CriHcaTables::getQuantizeSpectrumValue();
    const auto &quantizedSpectrumMaxBits = CriHcaTables::getQuantizedSpectrumMaxBits();
    const auto &quantizedSpectraArray = (*channel->quantizedSpectra)[subframe];

    for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
        const auto resolution = (*channel->resolution)[i];

        if (resolution == 0) {
            continue;
        }

        const auto quantizedSpectra = (*quantizedSpectraArray)[i];

        if (resolution < 8) {
            const auto bits = (*(*quantizeSpectrumBits)[resolution])[quantizedSpectra + 8];
            writer->write((*(*quantizeSpectrumValue)[resolution])[quantizedSpectra + 8], bits);
        } else if (resolution < 16) {
            const auto bits = (*quantizedSpectrumMaxBits)[resolution] - 1;
            writer->write(std::abs(quantizedSpectra), bits);

            if (quantizedSpectra != 0) {
                writer->write(quantizedSpectra > 0 ? 0 : 1, 1);
            }
        }
    }
}

static void WriteScaleFactors(const shared_ptr<BitWriter> &writer, const shared_ptr<CriHcaChannel> &channel) {
    const int32_t deltaBits = channel->scaleFactorDeltaBits;
    const auto &scales = channel->scaleFactors;

    writer->write(deltaBits, 3);

    if (deltaBits == 0) {
        return;
    }

    if (deltaBits == 6) {
        for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
            writer->write((*scales)[i], 6);
        }

        return;
    }

    writer->write((*scales)[0], 6);

    const auto maxDelta = (1 << (deltaBits - 1)) - 1;
    const auto escapeValue = (1 << deltaBits) - 1;

    for (auto i = 1; i < channel->codedScaleFactorCount; i += 1) {
        const auto delta = (*scales)[i] - (*scales)[i - 1];

        if (std::abs(delta) > maxDelta) {
            writer->write(escapeValue, deltaBits);
            writer->write((*scales)[i], 6);
        } else {
            writer->write(maxDelta + delta, deltaBits);
        }
    }
}

int32_t CriHcaPacking::calculateResolution(int32_t scaleFactor, int32_t noiseLevel) {
    if (scaleFactor == 0) {
        return 0;
    }

    int32_t curvePosition = noiseLevel - 5 * scaleFactor / 2 + 2;
    curvePosition = std::clamp(curvePosition, 0, 58);

    const auto &table = CriHcaTables::getScaleToResolutionCurve();

    return (*table)[curvePosition];
}

bool CriHcaPacking::unpackFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<BitReader> &reader) {
    if (!UnpackFrameHeader(frame, reader)) {
        return false;
    }

    ReadSpectralCoefficients(frame, reader);

    const auto r = UnpackingWasSuccessful(frame, reader);

    return r;
}

void CriHcaPacking::packFrame(const shared_ptr<CriHcaFrame> &frame, const shared_ptr<Crc16> &crc, const array_ptr<uint8_t> &outBuffer) {
    auto writer = make_shared<BitWriter>(outBuffer);

    writer->write(0xffff, 16);
    writer->write(frame->getAcceptableNoiseLevel(), 9);
    writer->write(frame->getEvaluationBoundary(), 7);

    const auto &hca = frame->getHca();

    for (const auto &channel : *frame->getChannels()) {
        WriteScaleFactors(writer, channel);

        if (channel->type == ChannelType::StereoSecondary) {
            for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                writer->write((*channel->intensity)[sf], 4);
            }
        } else {
            const auto groupCount = hca->hfrGroupCount;

            if (groupCount > 0) {
                for (auto group = 0; group < groupCount; group += 1) {
                    writer->write((*channel->hfrScales)[group], 6);
                }
            }
        }
    }

    for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
        for (const auto &channel : *frame->getChannels()) {
            WriteSpectra(writer, channel, sf);
        }
    }

    writer->alignPosition(8);

    const auto &writerBuffer = writer->getBuffer();
    for (auto i = writer->getPosition() / 8; i < hca->frameSize - 2; i += 1) {
        (*writerBuffer)[i] = 0;
    }

    WriteChecksum(writer, crc, outBuffer);
}
