#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <algorithm>

#include "../../../../common/utilities/IntHelper.h"
#include "../../../../common/utilities/ArrayHelper.h"
#include "../../../../common/utilities/macros.h"
#include "../../../utilities/Mdct.h"
#include "../HcaInfo.h"
#include "../CriHcaChannel.h"
#include "../CriHcaFrame.h"
#include "../CriHcaConstants.h"
#include "../CriHcaTables.h"
#include "../CriHcaPacking.h"
#include "CriHcaEncoderStatic.h"

using C = CriHcaConstants;

static int32_t FindScaleFactor(double value) {
    const auto &scalingTable = CriHcaTables::getDequantizerScalingTable();

    int32_t low = 0;
    int32_t high = 63;

    while (low < high) {
        const auto mid = (low + high) / 2;

        if ((*scalingTable)[mid] <= value) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

static int32_t CalculateUsedBits(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t noiseLevel, int32_t evaluationBoundary) {
    int32_t length = 16 + 16 + 16;

    const auto &quantizedSpectrumMaxBits = CriHcaTables::getQuantizedSpectrumMaxBits();
    const auto &quantizerDeadZone = CriHcaTables::getQuantizerDeadZone();
    const auto &quantizerInverseStepSize = CriHcaTables::getQuantizerInverseStepSize();
    const auto &quantizeSpectrumBits = CriHcaTables::getQuantizeSpectrumBits();

    for (const auto &channel : *channels) {
        length += channel->headerLengthBits;

        for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
            const auto noise = i < evaluationBoundary ? noiseLevel - 1 : noiseLevel;
            const auto resolution = CriHcaPacking::calculateResolution((*channel->scaleFactors)[i], noise);

            if (resolution >= 8) {
                const auto bits = (*quantizedSpectrumMaxBits)[resolution] - 1;
                const auto deadZone = (*quantizerDeadZone)[resolution];

                for (const auto scaledSpectra : (*(*channel->scaledSpectra)[i])) {
                    length += bits;

                    if (std::abs(scaledSpectra) >= deadZone) {
                        length += 1;
                    }
                }
            } else {
                const auto stepSizeInv = (*quantizerInverseStepSize)[resolution];
                const auto shiftUp = stepSizeInv + 1;
                const auto shiftDown = static_cast<int32_t>(stepSizeInv + 0.5 - 8);

                for (const auto scaledSpectra : (*(*channel->scaledSpectra)[i])) {
                    const auto quantizedSpectra = static_cast<int32_t>(scaledSpectra * stepSizeInv + shiftUp) - shiftDown;
                    length += (*(*quantizeSpectrumBits)[resolution])[quantizedSpectra];
                }
            }
        }
    }

    return length;
}

static void CalculateOptimalDeltaLength(const shared_ptr<CriHcaChannel> &channel) {
    auto emptyChannel = true;

    for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
        if ((*channel->scaleFactors)[i] != 0) {
            emptyChannel = false;
            break;
        }
    }

    if (emptyChannel) {
        channel->headerLengthBits = 3;
        channel->scaleFactorDeltaBits = 0;
        return;
    }

    int32_t minDeltaBits = 6;
    int32_t minLength = 3 + 6 * channel->codedScaleFactorCount;

    for (auto deltaBits = 1; deltaBits < 6; deltaBits += 1) {
        const auto maxDelta = (1 << (deltaBits - 1)) - 1;
        int32_t length = 3 + 6;

        for (auto band = 1; band < channel->codedScaleFactorCount; band += 1) {
            const auto delta = (*channel->scaleFactors)[band] - (*channel->scaleFactors)[band - 1];
            length += std::abs(delta) > maxDelta ? deltaBits + 6 : deltaBits;
        }

        if (length < minLength) {
            minLength = length;
            minDeltaBits = deltaBits;
        }
    }

    channel->headerLengthBits = minLength;
    channel->scaleFactorDeltaBits = minDeltaBits;
}

static int32_t BinarySearchLevel(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t availableBits, int32_t low, int32_t high) {
    int32_t max = high;
    int32_t midValue = 0;

    while (low != high) {
        const auto mid = (low + high) / 2;
        midValue = CalculateUsedBits(channels, mid, 0);

        if (midValue > availableBits) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low == max && midValue > availableBits ? -1 : low;
}

static int32_t BinarySearchBoundary(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t availableBits, int32_t noiseLevel, int32_t low, int32_t high) {
    int32_t max = high;

    while (std::abs(high - low) > 1) {
        const auto mid = (low + high) / 2;
        const auto midValue = CalculateUsedBits(channels, noiseLevel, mid);

        if (availableBits < midValue) {
            high = mid - 1;
        } else {
            low = mid;
        }
    }

    if (low == high) {
        return low < max ? low : -1;
    }

    const auto highValue = CalculateUsedBits(channels, noiseLevel, high);

    return highValue > availableBits ? low : high;
}

void CalculateBandCounts(const shared_ptr<HcaInfo> &hca, int32_t bitrate, int32_t cutoffFrequency) {
    hca->frameSize = bitrate * 1024 / hca->sampleRate / 8;
    int32_t numGroups = 0;
    // TODO: bit depth fixed to s16
    const auto pcmBitrate = hca->sampleRate * hca->channelCount * 16;
    int32_t hfrRatio, cutoffRatio;

    if (hca->channelCount <= 1 || pcmBitrate / bitrate <= 6) {
        hfrRatio = 6;
        cutoffRatio = 12;
    } else {
        hfrRatio = 8;
        cutoffRatio = 16;
    }

    int32_t cutoffFreq;

    if (bitrate < pcmBitrate / cutoffRatio) {
        cutoffFreq = std::min(cutoffFrequency, cutoffRatio * bitrate / (32 * hca->channelCount));
    } else {
        cutoffFreq = cutoffFrequency;
    }

    const auto totalBandCount = static_cast<int32_t>(std::round(cutoffFreq * 256.0 / hca->sampleRate));
    const auto hfrStartBand = static_cast<int32_t>(std::min(static_cast<double>(totalBandCount), std::round((hfrRatio * bitrate * 128.0) / pcmBitrate)));
    const auto stereoStartBand = hfrRatio == 6 ? hfrStartBand : (hfrStartBand + 1) / 2;

    const auto hfrBandCount = totalBandCount - hfrStartBand;
    const auto bandsPerGroup = IntHelper::divideByRoundUp(hfrBandCount, 8);

    if (bandsPerGroup > 0) {
        numGroups = IntHelper::divideByRoundUp(hfrBandCount, bandsPerGroup);
    }

    hca->totalBandCount = totalBandCount;
    hca->baseBandCount = stereoStartBand;
    hca->stereoBandCount = hfrStartBand - stereoStartBand;
    hca->hfrGroupCount = numGroups;
    hca->bandsPerHfrGroup = bandsPerGroup;
}

void CalculateLoopInfo(const shared_ptr<HcaInfo> &hca, int32_t loopStart, int32_t loopEnd) {
    const auto start = loopStart + hca->insertedSamples;
    const auto end = loopEnd + hca->insertedSamples;

    hca->loopStartFrame = start / C::SamplesPerFrame;
    hca->preLoopSamples = start % C::SamplesPerFrame;
    hca->loopEndFrame = end / C::SamplesPerFrame;
    hca->postLoopSamples = C::SamplesPerFrame - end % C::SamplesPerFrame;

    if (hca->postLoopSamples == C::SamplesPerFrame) {
        hca->loopEndFrame -= 1;
        hca->postLoopSamples = 0;
    }
}

void CalculateHeaderSize(const shared_ptr<HcaInfo> &hca) {
    constexpr int32_t baseHeaderSize = 96;
    constexpr int32_t baseHeaderAlignment = 32;
    constexpr int32_t loopFrameAlignment = 2048;

    hca->headerSize = IntHelper::getNextMultiple(baseHeaderSize + hca->getCommentLength(), baseHeaderAlignment);

    if (hca->looping) {
        const auto loopFrameOffset = hca->headerSize + hca->frameSize * hca->loopStartFrame;
        const auto paddingBytes = IntHelper::getNextMultiple(loopFrameOffset, loopFrameAlignment) - loopFrameOffset;
        const auto paddingFrames = paddingBytes / hca->frameSize;

        hca->insertedSamples += paddingFrames * C::SamplesPerFrame;
        hca->loopStartFrame += paddingFrames;
        hca->loopEndFrame += paddingFrames;
        hca->headerSize += paddingBytes % hca->frameSize;
    }
}

void QuantizeSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    const auto quantizerInverseStepSize = CriHcaTables::getQuantizerInverseStepSize();

    for (const auto &channel : *channels) {
        for (auto i = 0; i < channel->codedScaleFactorCount; i += 1) {
            const auto &scaled = (*channel->scaledSpectra)[i];
            const auto resolution = (*channel->resolution)[i];
            const double stepSizeInv = (*quantizerInverseStepSize)[resolution];
            const double shiftUp = stepSizeInv + 1;
            // lround?
            const auto shiftDown = static_cast<int32_t>(stepSizeInv + 0.5);

            for (auto sf = 0; sf < scaled->size(); sf += 1) {
                const auto quantizedSpectra = static_cast<int32_t>((*scaled)[sf] * stepSizeInv + shiftUp) - shiftDown;
                (*(*channel->quantizedSpectra)[sf])[i] = quantizedSpectra;
            }
        }
    }
}

void CalculateFrameResolutions(const shared_ptr<CriHcaFrame> &frame) {
    const auto evalBound = frame->getEvaluationBoundary();

    for (const auto &channel : *frame->getChannels()) {
        const auto resolution = channel->resolution;
        const auto &scaleFactors = channel->scaleFactors;

        for (auto i = 0; i < evalBound; i += 1) {
            (*resolution)[i] = CriHcaPacking::calculateResolution((*scaleFactors)[i], frame->getAcceptableNoiseLevel() - 1);
        }

        for (auto i = evalBound; i < channel->codedScaleFactorCount; i += 1) {
            (*resolution)[i] = CriHcaPacking::calculateResolution((*scaleFactors)[i], frame->getAcceptableNoiseLevel());
        }

        ArrayHelper::clearByValue(resolution, channel->codedScaleFactorCount, resolution->size() - channel->codedScaleFactorCount);
    }
}

void CalculateNoiseLevel(const shared_ptr<CriHcaFrame> &frame) {
    const auto &hca = frame->getHca();

    auto highestBand = hca->baseBandCount + hca->stereoBandCount - 1;
    const auto availableBits = hca->frameSize * 8;
    constexpr int32_t maxLevel = 255;
    constexpr int32_t minLevel = 0;

    const auto &channels = frame->getChannels();

    auto level = BinarySearchLevel(channels, availableBits, minLevel, maxLevel);

    while (level < 0) {
        highestBand -= 2;

        if (highestBand < 0) {
            throw std::invalid_argument("bitrate is too low");
        }

        for (const auto &channel : *channels) {
            (*channel->scaleFactors)[highestBand + 1] = 0;
            (*channel->scaleFactors)[highestBand + 2] = 0;
        }

        CalculateFrameHeaderLength(frame);

        level = BinarySearchLevel(channels, availableBits, minLevel, maxLevel);
    }

    frame->setAcceptableNoiseLevel(level);
}

void CalculateEvaluationBoundary(const shared_ptr<CriHcaFrame> &frame) {
    if (frame->getAcceptableNoiseLevel() == 0) {
        frame->setEvaluationBoundary(0);
        return;
    }

    const auto availableBits = frame->getHca()->frameSize * 8;
    constexpr int32_t maxLevel = 127;
    constexpr int32_t minLevel = 0;
    const auto level = BinarySearchBoundary(frame->getChannels(), availableBits, frame->getAcceptableNoiseLevel(), minLevel, maxLevel);

    if (level >= 0) {
        frame->setEvaluationBoundary(level);
    } else {
        throw std::runtime_error("not implemented");
    }
}

void CalculateFrameHeaderLength(const shared_ptr<CriHcaFrame> &frame) {
    const auto hfrGroupCount = frame->getHca()->hfrGroupCount;

    for (const auto &channel : *frame->getChannels()) {
        CalculateOptimalDeltaLength(channel);

        if (channel->type == ChannelType::StereoSecondary) {
            channel->headerLengthBits += 32;
        } else if (hfrGroupCount > 0) {
            channel->headerLengthBits += 6 * hfrGroupCount;
        }
    }
}

void SetChannelConfiguration(const shared_ptr<HcaInfo> &hca, int32_t channelConfig) {
    const auto channelsPerTrack = hca->channelCount / hca->trackCount;

    const auto &defaultChannelMapping = CriHcaTables::getDefaultChannelMapping();

    if (channelConfig == -1) {
        channelConfig = (*defaultChannelMapping)[channelsPerTrack];
    }

    const auto &validChannelMappings = CriHcaTables::getValidChannelMappings();

    if ((*(*validChannelMappings)[channelsPerTrack - 1])[channelConfig] != 1) {
        throw std::invalid_argument("'" TO_STRING(channelConfig) "' invalid: channel mapping is invalid");
    }

    hca->channelConfig = channelConfig;
}

void ScaleSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    const auto &scalingTable = CriHcaTables::getQuantizerScalingTable();

    for (const auto &channel : *channels) {
        for (auto b = 0; b < channel->codedScaleFactorCount; b += 1) {
            const auto &scaledSpectra = (*channel->scaledSpectra)[b];
            const auto scaleFactor = (*channel->scaleFactors)[b];

            for (auto sf = 0; sf < scaledSpectra->size(); sf += 1) {
                const double coeff = (*(*channel->spectra)[sf])[b];

                (*scaledSpectra)[sf] = scaleFactor == 0 ? 0 : std::clamp(coeff * (*scalingTable)[scaleFactor], -0.999999999999, 0.999999999999);
            }
        }
    }
}

void CalculateScaleFactors(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    for (const auto &channel : *channels) {
        for (auto b = 0; b < channel->codedScaleFactorCount; b += 1) {
            double max = 0;

            for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                const double coeff = std::abs((*(*channel->spectra)[sf])[b]);
                max = std::max(coeff, max);
            }

            (*channel->scaleFactors)[b] = FindScaleFactor(max);
        }

        const auto &scaleFactors = channel->scaleFactors;
        const auto codedScaleFactorCount = channel->codedScaleFactorCount;
        ArrayHelper::clearByValue(scaleFactors, codedScaleFactorCount, (scaleFactors->size() - codedScaleFactorCount));
    }
}

void EncodeIntensityStereo(const shared_ptr<CriHcaFrame> &frame) {
    const auto &hca = frame->getHca();

    if (hca->stereoBandCount <= 0) {
        return;
    }

    const auto &channels = frame->getChannels();
    const auto channelCount = channels->size();

    const auto &ratioBounds = CriHcaTables::getIntensityRatioBoundsTable();

    for (auto c = 0; c < channelCount; c += 1) {
        if ((*channels)[c]->type != ChannelType::StereoPrimary) {
            continue;
        }

        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            const auto &thisChannel = (*channels)[c];
            const auto &nextChannel = (*channels)[c + 1];

            const auto &l = (*thisChannel->spectra)[sf];
            const auto &r = (*nextChannel->spectra)[sf];

            double energyL = 0, energyR = 0, energyTotal = 0;

            for (auto b = hca->baseBandCount; b < hca->totalBandCount; b += 1) {
                energyL += std::abs((*l)[b]);
                energyR += std::abs((*r)[b]);
                energyTotal += std::abs((*l)[b] + (*r)[b]);
            }

            energyTotal *= 2;

            const auto energyLR = energyR + energyL;
            const auto storedValue = 2 * energyL / energyLR;
            double energyRatio;

            int32_t quantized;

            if (energyR > 0 || energyL > 0) {
                quantized = 1;
                energyRatio = std::clamp(energyLR / energyTotal, 0.5, M_SQRT2 / 2);

                while (quantized < 13 && (*ratioBounds)[quantized] >= storedValue) {
                    quantized += 1;
                }
            } else {
                quantized = 0;
                energyRatio = 1;
            }

            (*nextChannel->intensity)[sf] = quantized;

            for (auto b = hca->baseBandCount; b < hca->totalBandCount; b += 1) {
                (*l)[b] = ((*l)[b] + (*r)[b]) * energyRatio;
                (*r)[b] = 0;
            }
        }
    }
}

void CalculateHfrGroupAverages(const shared_ptr<CriHcaFrame> &frame) {
    const auto &hca = frame->getHca();

    if (hca->hfrGroupCount == 0) {
        return;
    }

    const auto hfrStartBand = hca->stereoBandCount + hca->baseBandCount;

    for (const auto &channel : *frame->getChannels()) {
        if (channel->type == ChannelType::StereoSecondary) {
            continue;
        }

        for (auto group = 0, band = hfrStartBand; group < hca->hfrGroupCount; group += 1) {
            double sum = 0;
            int32_t count = 0;

            for (auto i = 0; i < hca->bandsPerHfrGroup && band < C::SamplesPerSubframe; band += 1, i += 1) {
                for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                    sum += std::abs((*(*channel->spectra)[sf])[band]);
                }

                count += C::SubframesPerFrame;
            }

            (*channel->hfrGroupAverageSpectra)[group] = sum / count;
        }
    }
}

void CalculateHfrScale(const shared_ptr<CriHcaFrame> &frame) {
    const auto &hca = frame->getHca();

    if (hca->hfrGroupCount == 0) {
        return;
    }

    const auto hfrStartBand = hca->stereoBandCount + hca->baseBandCount;
    const auto hfrBandCount = std::min(hca->hfrBandCount, hca->totalBandCount - hca->hfrBandCount);

    for (const auto &channel : *frame->getChannels()) {
        if (channel->type == ChannelType::StereoSecondary) {
            continue;
        }

        const auto &groupSpectra = channel->hfrGroupAverageSpectra;

        for (auto group = 0, band = 0; group < hca->hfrGroupCount; group += 1) {
            double sum = 0;
            int32_t count = 0;

            for (auto i = 0; i < hca->bandsPerHfrGroup && band < hfrBandCount; band += 1, i += 1) {
                for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
                    sum += std::abs((*(*channel->scaledSpectra)[hfrStartBand - band - 1])[sf]);
                }

                count += C::SubframesPerFrame;
            }

            const double averageSpectra = sum / count;

            if (averageSpectra > 0) {
                (*groupSpectra)[group] *= std::min(1.0 / averageSpectra, M_SQRT2);
            }

            (*channel->hfrScales)[group] = FindScaleFactor((*groupSpectra)[group]);
        }
    }
}

void RunMdct(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    for (const auto &channel : *channels) {
        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            channel->mdct->RunMdct(ArrayHelper::arrayToDynamic((*channel->pcmFloat)[sf]), ArrayHelper::arrayToDynamic((*channel->spectra)[sf]));
        }
    }
}

void PcmToFloat(const jarray2_ptr<int16_t> &pcm, const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    const auto channelCount = channels->size();

    for (auto c = 0; c < channelCount; c += 1) {
        int32_t pcmIndex = 0;
        const auto &channel = (*channels)[c];

        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            for (auto i = 0; i < C::SamplesPerSubframe; i += 1) {
                (*(*channel->pcmFloat)[sf])[i] = (*(*pcm)[c])[pcmIndex] * (1.0 / 32768.0);
                pcmIndex += 1;
            }
        }
    }
}

int32_t CalculateBitrate(const shared_ptr<HcaInfo> &hca, CriHcaQuality quality, int32_t bitrate, bool limitBitrate) {
    // TODO: bit depth fixed to s16
    const auto pcmBitrate = hca->sampleRate * hca->channelCount * 16;
    const auto maxBitrate = pcmBitrate / 4;
    int32_t minBitrate = 0;
    int32_t rate;

    if (bitrate > 0) {
        rate = bitrate;
    } else {
        int32_t compressionRatio;

        switch (quality) {
            case CriHcaQuality::Highest:
                compressionRatio = 4;
                break;
            case CriHcaQuality::High:
                compressionRatio = 6;
                break;
            case CriHcaQuality::Middle:
                compressionRatio = 8;
                break;
            case CriHcaQuality::Low:
                compressionRatio = hca->channelCount <= 1 ? 10 : 12;
                break;
            case CriHcaQuality::Lowest:
                compressionRatio = hca->channelCount <= 1 ? 12 : 16;
                break;
            default:
                compressionRatio = 6;
                break;
        }

        rate = pcmBitrate / compressionRatio;
    }

    if (limitBitrate) {
        minBitrate = std::min(hca->channelCount <= 1 ? 42666 : 32000 * hca->channelCount, pcmBitrate / 6);
    }

    return std::clamp(rate, minBitrate, maxBitrate);
}
