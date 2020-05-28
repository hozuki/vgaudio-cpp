#include "../../../../common/utilities/ArrayHelper.h"
#include "../../../../common/utilities/IntHelper.h"
#include "../../../utilities/BitReader.h"
#include "../../../utilities/Mdct.h"
#include "../HcaInfo.h"
#include "../CriHcaConstants.h"
#include "../CriHcaFrame.h"
#include "../CriHcaChannel.h"
#include "../CriHcaTables.h"
#include "../CriHcaPacking.h"
#include "CriHcaDecoderStatic.h"

using C = CriHcaConstants;

using namespace vgaudio::utilities;

static void RunImdct(const shared_ptr<CriHcaFrame> &frame) {
    for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
        for (const auto &channel : *frame->getChannels()) {
            channel->mdct->RunImdct(ArrayHelper::arrayToDynamic((*channel->spectra)[sf]), ArrayHelper::arrayToDynamic((*channel->pcmFloat)[sf]));
        }
    }
}

static void PcmFloatToShort(const shared_ptr<CriHcaFrame> &frame, const jarray2_ptr<int16_t> &pcm) {
    const auto &channels = frame->getChannels();
    const auto channelCount = channels->size();

    for (auto c = 0; c < channelCount; c += 1) {
        const auto &channel = (*channels)[c];
        const auto &pcm0 = (*pcm)[c];

        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            auto buf1 = (*channel->pcmFloat)[sf];

            for (auto s = 0; s < C::SamplesPerSubframe; s += 1) {
                const auto sample = static_cast<int32_t>((*buf1)[s] * (INT16_MAX + 1));
                (*pcm0)[sf * C::SamplesPerSubframe + s] = static_cast<int16_t>(IntHelper::clamp<int32_t>(sample, INT16_MIN, INT16_MAX));
            }
        }
    }
}

static void ApplyIntensityStereo(const shared_ptr<CriHcaFrame> &frame) {
    const auto hca = frame->getHca();

    if (hca->stereoBandCount <= 0) {
        return;
    }

    const auto &channels = frame->getChannels();

    if (channels->empty()) {
        return;
    }

    const auto channelCount = channels->size();

    const auto &intensityRatio = CriHcaTables::getIntensityRatioTable();

    const auto baseBandCount = hca->baseBandCount;
    const auto totalBandCount = hca->totalBandCount;

    for (auto c = 0; c < channelCount; c += 1) {
        if ((*channels)[c]->type != ChannelType::StereoPrimary) {
            continue;
        }

        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            const auto &l = (*(*channels)[c]->spectra)[sf];
            const auto &r = (*(*channels)[c + 1]->spectra)[sf];
            const auto intensityIndex = (*(*channels)[c + 1]->intensity)[sf];
            double ratioL = (*intensityRatio)[intensityIndex];
            double ratioR = ratioL - 2.0;

            for (auto b = baseBandCount; b < totalBandCount; b += 1) {
                (*r)[b] = (*l)[b] * ratioR;
                (*l)[b] = (*l)[b] * ratioL;
            }
        }
    }
}

static void ReconstructHighFrequency(const shared_ptr<CriHcaFrame> &frame) {
    const auto &hca = frame->getHca();

    if (hca->hfrGroupCount <= 0) {
        return;
    }

    const auto totalBandCount = std::min<int32_t>(hca->totalBandCount, 127);

    const auto hfrStartBand = hca->baseBandCount + hca->stereoBandCount;
    const auto hfrBandCount = std::min(hca->hfrBandCount, totalBandCount - hca->hfrBandCount);

    const auto &scaleConversion = CriHcaTables::getScaleConversionTable();

    for (const auto &channel : *frame->getChannels()) {
        if (channel->type == ChannelType::StereoSecondary) {
            continue;
        }

        for (auto group = 0, band = 0; group < hca->hfrGroupCount; group += 1) {
            for (auto i = 0; i < hca->bandsPerHfrGroup && band < hfrBandCount; band += 1, i += 1) {
                const auto highBand = hfrStartBand + band;
                const auto lowBand = hfrStartBand - band - 1;
                const auto index = (*channel->hfrScales)[group] - (*channel->scaleFactors)[lowBand] + 64;

                for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                    (*(*channel->spectra)[sf])[highBand] = (*scaleConversion)[index] * (*(*channel->spectra)[sf])[lowBand];
                }
            }
        }
    }
}

static void CalculateGain(const shared_ptr<CriHcaChannel> &channel) {
    const auto &scalingTable = CriHcaTables::getDequantizerScalingTable();
    const auto &stepSizeTable = CriHcaTables::getQuantizerStepSize();

    for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
        const auto scaleIndex = (*channel->scaleFactors)[i];
        const auto stepSizeIndex = (*channel->resolution)[i];

        (*channel->gain)[i] = (*scalingTable)[scaleIndex] * (*stepSizeTable)[stepSizeIndex];
    }
}

static void RestoreMissingBands(const shared_ptr<CriHcaFrame> &frame) {
    ReconstructHighFrequency(frame);
    ApplyIntensityStereo(frame);
}

static void DequantizeFrame(const shared_ptr<CriHcaFrame> &frame) {
    for (const auto &channel : *frame->getChannels()) {
        CalculateGain(channel);
    }

    for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
        for (const auto &channel : *frame->getChannels()) {
            for (auto s = 0; s < channel->codedScaleFactorCount; s += 1) {
                (*(*channel->spectra)[sf])[s] = (*(*channel->quantizedSpectra)[sf])[s] * (*channel->gain)[s];
            }
        }
    }
}

void DecodeFrame(const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaFrame> &frame, const jarray2_ptr<int16_t> &pcmOut) {
    auto reader = make_shared<BitReader>(audio);

    CriHcaPacking::unpackFrame(frame, reader);
    DequantizeFrame(frame);
    RestoreMissingBands(frame);
    RunImdct(frame);
    PcmFloatToShort(frame, pcmOut);
}

void CopyPcmToOutput(const jarray2_ptr<int16_t> &pcmIn, const jarray2_ptr<int16_t> &pcmOut, const shared_ptr<HcaInfo> &hca, int32_t frame) {
    const int32_t currentSample = frame * C::SamplesPerFrame - hca->insertedSamples;
    const int32_t remainingSamples = std::min(hca->sampleCount - currentSample, hca->sampleCount);
    const int32_t srcStart = IntHelper::clamp(0 - currentSample, 0, C::SamplesPerFrame);
    const int32_t dstStart = std::max(currentSample, 0);

    const int32_t length = std::min(C::SamplesPerFrame - srcStart, remainingSamples);

    if (length <= 0) {
        return;
    }

    for (auto c = 0; c < pcmOut->size(); c += 1) {
        ArrayHelper::copyByValue((*pcmIn)[c], srcStart, (*pcmOut)[c], dstStart, length);
    }
}
