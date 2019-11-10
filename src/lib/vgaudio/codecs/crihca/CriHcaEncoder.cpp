#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <cinttypes>

#include "CriHcaTables.h"
#include "CriHcaConstants.h"
#include "CriHcaPacking.h"
#include "CriHcaParameters.h"
#include "CriHcaChannel.h"
#include "CriHcaFrame.h"
#include "HcaInfo.h"
#include "../../utilities/Crc16.h"
#include "../../utilities/Mdct.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "../../../common/utilities/macros.h"
#include "CriHcaEncoder.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

using C = CriHcaConstants;

static void CalculateBandCounts(const shared_ptr<HcaInfo> &hca, int32_t bitrate, int32_t cutoffFrequency);

static void SetChannelConfiguration(const shared_ptr<HcaInfo> &hca, int32_t channelConfig = -1);

static void CalculateLoopInfo(const shared_ptr<HcaInfo> &hca, int32_t loopStart, int32_t loopEnd);

static void CalculateHeaderSize(const shared_ptr<HcaInfo> &hca);

static void QuantizeSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

static void CalculateFrameResolutions(const shared_ptr<CriHcaFrame> &frame);

static void CalculateNoiseLevel(const shared_ptr<CriHcaFrame> &frame);

static void CalculateEvaluationBoundary(const shared_ptr<CriHcaFrame> &frame);

static int32_t BinarySearchLevel(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t availableBits, int32_t low, int32_t high);

static int32_t BinarySearchBoundary(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t availableBits, int32_t noiseLevel, int32_t low, int32_t high);

static int32_t CalculateUsedBits(const array_ptr<shared_ptr<CriHcaChannel>> &channels, int32_t noiseLevel, int32_t evaluationBoundary);

static void CalculateFrameHeaderLength(const shared_ptr<CriHcaFrame> &frame);

static void CalculateOptimalDeltaLength(const shared_ptr<CriHcaChannel> &channel);

static void ScaleSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

static void CalculateScaleFactors(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

static int32_t FindScaleFactor(double value);

static void EncodeIntensityStereo(const shared_ptr<CriHcaFrame> &frame);

static void CalculateHfrGroupAverages(const shared_ptr<CriHcaFrame> &frame);

static void CalculateHfrScale(const shared_ptr<CriHcaFrame> &frame);

static void RunMdct(const array_ptr<shared_ptr<CriHcaChannel>> &channels);

static void PcmToFloat(const jarray2_ptr<int16_t> &pcm, const array_ptr<shared_ptr<CriHcaChannel>> &channels);

static int32_t CalculateBitrate(const shared_ptr<HcaInfo> &hca, CriHcaQuality quality, int32_t bitrate, bool limitBitrate);

CriHcaEncoder::CriHcaEncoder()
    : _quality(CriHcaQuality::NotSet), _bitrate(0), _cutoffFrequency(0) {
    _crc = make_shared<Crc16>(0x8005);

    _bufferPosition = 0;
    _bufferPreSamples = 0;
    _samplesProcessed = 0;
    _framesProcessed = 0;
    _postSamples = 0;
}

void CriHcaEncoder::setHca(const shared_ptr<HcaInfo> &hca) {
    _hca = hca;
}

void CriHcaEncoder::setQuality(CriHcaQuality quality) {
    _quality = quality;
}

void CriHcaEncoder::setBitrate(int32_t bitrate) {
    _bitrate = bitrate;
}

void CriHcaEncoder::setCutoffFrequency(int32_t frequency) {
    _cutoffFrequency = frequency;
}

shared_ptr<HcaInfo> CriHcaEncoder::getHca() const {
    return _hca;
}

CriHcaQuality CriHcaEncoder::getQuality() const {
    return _quality;
}

int32_t CriHcaEncoder::getBitrate() const {
    return _bitrate;
}

int32_t CriHcaEncoder::getCutoffFrequency() const {
    return _cutoffFrequency;
}

size_t CriHcaEncoder::getPendingFrameCount() const {
    return _hcaOutputBuffer.size();
}

int32_t CriHcaEncoder::getFrameSize() const {
    return _hca->frameSize;
}

size_t CriHcaEncoder::getBufferRemaining() const {
    return C::SamplesPerFrame - _bufferPosition;
}

void CriHcaEncoder::setFramesProcessed(int32_t n) {
    _framesProcessed = n;
}

size_t CriHcaEncoder::getFramesProcessed() const {
    return _framesProcessed;
}

void CriHcaEncoder::encodeFrame(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut) {
    PcmToFloat(pcm, _channels);
    RunMdct(_channels);
    EncodeIntensityStereo(_frame);
    CalculateScaleFactors(_channels);
    ScaleSpectra(_channels);
    CalculateHfrGroupAverages(_frame);
    CalculateHfrScale(_frame);
    CalculateFrameHeaderLength(_frame);
    CalculateNoiseLevel(_frame);
    CalculateEvaluationBoundary(_frame);
    CalculateFrameResolutions(_frame);
    QuantizeSpectra(_channels);
    CriHcaPacking::packFrame(_frame, _crc, hcaOut);
}

int32_t CriHcaEncoder::encodePreAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput) {
    while (_bufferPreSamples > C::SamplesPerFrame) {
        _bufferPosition = C::SamplesPerFrame;
        framesOutput = outputFrame(framesOutput, hcaOut);
        _bufferPreSamples -= C::SamplesPerFrame;
    }

    const auto &pcmBuffer = _pcmBuffer;

    for (auto j = 0; j < _bufferPreSamples; j += 1) {
        for (auto i = 0; i < pcm->size(); i += 1) {
            (*(*pcmBuffer)[i])[j] = (*(*pcm)[i])[0];
        }
    }

    _bufferPosition = _bufferPreSamples;
    _bufferPreSamples = 0;

    return framesOutput;
}

int32_t CriHcaEncoder::encodeMainAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput, int32_t &pcmPosition) {
    auto toCopy = std::min(static_cast<int32_t>(getBufferRemaining()), C::SamplesPerFrame - pcmPosition);
    toCopy = std::min(toCopy, getHca()->sampleCount - _samplesProcessed);

    const auto &pcmBuffer = _pcmBuffer;

    for (auto i = 0; i < pcm->size(); i += 1) {
        ArrayHelper::copyByValue((*pcm)[i], pcmPosition, (*pcmBuffer)[i], _bufferPosition, toCopy);
    }

    _bufferPosition += toCopy;
    _samplesProcessed += toCopy;
    pcmPosition += toCopy;

    framesOutput = outputFrame(framesOutput, hcaOut);

    return framesOutput;
}

int32_t CriHcaEncoder::encodePostAudio(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut, int32_t framesOutput) {
    int32_t postPos = 0;
    int32_t remaining = _postSamples;

    const auto &pcmBuffer = _pcmBuffer;
    const auto &postAudio = _postAudio;

    while (postPos < remaining) {
        const auto toCopy = std::min(static_cast<int32_t>(getBufferRemaining()), remaining - postPos);

        for (auto i = 0; i < pcm->size(); i += 1) {
            ArrayHelper::copyByValue((*postAudio)[i], postPos, (*pcmBuffer)[i], _bufferPosition, toCopy);
        }

        _bufferPosition += toCopy;
        postPos += toCopy;
    }

    const auto &hca = getHca();

    while (_framesProcessed < hca->frameCount) {
        for (auto i = 0; i < pcm->size(); i += 1) {
            ArrayHelper::clearByValue((*pcm)[i], _bufferPosition, getBufferRemaining());
        }

        _bufferPosition = C::SamplesPerFrame;
        framesOutput = outputFrame(framesOutput, hcaOut);
    }

    return framesOutput;
}

void CriHcaEncoder::saveLoopAudio(const jarray2_ptr<int16_t> &pcm) {
    const auto &hca = getHca();

    const auto startPos = std::max(hca->getLoopStartSample() - _samplesProcessed, 0);
    const auto loopPos = std::max(_samplesProcessed - hca->getLoopStartSample(), 0);
    const auto endPos = std::min(hca->getLoopStartSample() - _samplesProcessed + _postSamples, C::SamplesPerFrame);
    const auto length = endPos - startPos;

    const auto &postAudio = _postAudio;

    for (auto i = 0; i < pcm->size(); i += 1) {
        ArrayHelper::copyByValue((*pcm)[i], startPos, (*postAudio)[i], loopPos, length);
    }
}

int32_t CriHcaEncoder::outputFrame(int32_t framesOutput, const array_ptr<uint8_t> &hcaOut) {
    if (getBufferRemaining() != 0) {
        return framesOutput;
    }

    array_ptr<uint8_t> hcaBuffer;

    if (framesOutput == 0) {
        hcaBuffer = hcaOut;
    } else {
        hcaBuffer = make_array_dynamic<uint8_t>(getHca()->frameSize);
    }

    encodeFrame(_pcmBuffer, hcaBuffer);

    if (framesOutput > 0) {
        _hcaOutputBuffer.push(hcaBuffer);
    }

    _bufferPosition = 0;
    _framesProcessed += 1;

    return framesOutput + 1;
}

array_ptr<uint8_t> CriHcaEncoder::getPendingFrame() {
    if (getPendingFrameCount() == 0) {
        throw std::runtime_error("no pending frame available");
    }

    auto r = _hcaOutputBuffer.front();
    _hcaOutputBuffer.pop();
    return r;
}

int32_t CriHcaEncoder::encode(const jarray2_ptr<int16_t> &pcm, const array_ptr<uint8_t> &hcaOut) {
    const auto &hca = getHca();

    if (_framesProcessed >= hca->frameCount) {
        throw std::runtime_error("audio is all processed");
    }

    int32_t framesOutput = 0;
    int32_t pcmPosition = 0;

    if (_bufferPreSamples > 0) {
        framesOutput = encodePreAudio(pcm, hcaOut, framesOutput);
    }

    if (hca->looping && hca->getLoopStartSample() + _postSamples >= _samplesProcessed && hca->getLoopStartSample() < _samplesProcessed + C::SamplesPerFrame) {
        saveLoopAudio(pcm);
    }

    while (C::SamplesPerFrame - pcmPosition > 0 && hca->sampleCount > _samplesProcessed) {
        framesOutput = encodeMainAudio(pcm, hcaOut, framesOutput, pcmPosition);
    }

    if (hca->sampleCount == _samplesProcessed) {
        framesOutput = encodePostAudio(pcm, hcaOut, framesOutput);
    }

    return framesOutput;
}

void CriHcaEncoder::initialize(const shared_ptr<CriHcaParameters> &config) {
    if (config->channelCount > 8) {
        throw std::invalid_argument(StrHelper::format("invalid channel count: %" PRId32 ", must be at most 8", config->channelCount));
    }

    setCutoffFrequency(config->sampleRate / 2);
    setQuality(config->quality);
    _postSamples = 128;

    auto hca = make_shared<HcaInfo>();

    hca->channelCount = config->channelCount;
    hca->trackCount = 1;
    hca->sampleCount = config->sampleCount;
    hca->sampleRate = config->sampleRate;
    hca->minResolution = 1;
    hca->maxResolution = 15;
    hca->insertedSamples = C::SamplesPerSubframe;

    setHca(hca);

    setBitrate(CalculateBitrate(hca, getQuality(), config->bitrate, config->limitBitrate));
    CalculateBandCounts(hca, getBitrate(), getCutoffFrequency());
    hca->calculateHfrValues();
    SetChannelConfiguration(hca);

    auto inputSampleCount = hca->sampleCount;

    if (config->looping) {
        hca->looping = true;
        hca->sampleCount = std::min(config->loopEnd, config->sampleCount);
        hca->insertedSamples += IntHelper::getNextMultiple(config->loopStart, C::SamplesPerFrame) - config->loopStart;
        CalculateLoopInfo(hca, config->loopStart, config->loopEnd);

        inputSampleCount = std::min(IntHelper::getNextMultiple(hca->sampleCount, C::SamplesPerSubframe), config->sampleCount);
        inputSampleCount += C::SamplesPerSubframe * 2;

        _postSamples = inputSampleCount - hca->sampleCount;
    }

    CalculateHeaderSize(hca);

    const auto totalSamples = inputSampleCount + hca->insertedSamples;

    hca->frameCount = IntHelper::divideByRoundUp(totalSamples, C::SamplesPerFrame);
    hca->appendedSamples = hca->frameCount * C::SamplesPerFrame - hca->insertedSamples - inputSampleCount;

    _frame = make_shared<CriHcaFrame>(hca);
    _channels = _frame->getChannels();
    _pcmBuffer = make_jagged_array_2_dynamic<int16_t>(hca->channelCount, C::SamplesPerFrame);
    _postAudio = make_jagged_array_2_dynamic<int16_t>(hca->channelCount, _postSamples);

    // _hcaOutputBuffer is initialized in the constructor (default std::queue<T> ctor)

    _bufferPreSamples = hca->insertedSamples - 128;
}

shared_ptr<CriHcaEncoder> CriHcaEncoder::createNew(const shared_ptr<CriHcaParameters> &config) {
    auto encoder = new CriHcaEncoder();

    encoder->initialize(config);

    return shared_ptr<CriHcaEncoder>(encoder);
}

static void CalculateBandCounts(const shared_ptr<HcaInfo> &hca, int32_t bitrate, int32_t cutoffFrequency) {
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

static void CalculateLoopInfo(const shared_ptr<HcaInfo> &hca, int32_t loopStart, int32_t loopEnd) {
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

static void CalculateHeaderSize(const shared_ptr<HcaInfo> &hca) {
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

static void QuantizeSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
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

static void CalculateFrameResolutions(const shared_ptr<CriHcaFrame> &frame) {
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

static void CalculateNoiseLevel(const shared_ptr<CriHcaFrame> &frame) {
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

static void CalculateEvaluationBoundary(const shared_ptr<CriHcaFrame> &frame) {
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

static void CalculateFrameHeaderLength(const shared_ptr<CriHcaFrame> &frame) {
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

static void SetChannelConfiguration(const shared_ptr<HcaInfo> &hca, int32_t channelConfig) {
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
            //printf("sf[b] = %d, sf[b-1] = %d, d = %d\n", (*channel->scaleFactors)[band], (*channel->scaleFactors)[band - 1], delta);
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

static void ScaleSpectra(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
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

static void CalculateScaleFactors(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
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

static void EncodeIntensityStereo(const shared_ptr<CriHcaFrame> &frame) {
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

static void CalculateHfrGroupAverages(const shared_ptr<CriHcaFrame> &frame) {
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

static void CalculateHfrScale(const shared_ptr<CriHcaFrame> &frame) {
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

static void RunMdct(const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
    for (const auto &channel : *channels) {
        for (auto sf = 0; sf < C::SubframesPerFrame; sf += 1) {
            channel->mdct->RunMdct(ArrayHelper::arrayToDynamic((*channel->pcmFloat)[sf]), ArrayHelper::arrayToDynamic((*channel->spectra)[sf]));
        }
    }
}

static void PcmToFloat(const jarray2_ptr<int16_t> &pcm, const array_ptr<shared_ptr<CriHcaChannel>> &channels) {
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

static int32_t CalculateBitrate(const shared_ptr<HcaInfo> &hca, CriHcaQuality quality, int32_t bitrate, bool limitBitrate) {
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
