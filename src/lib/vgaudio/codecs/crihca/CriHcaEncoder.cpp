#include <cinttypes>

#include "CriHcaConstants.h"
#include "CriHcaPacking.h"
#include "CriHcaParameters.h"
#include "CriHcaChannel.h"
#include "CriHcaFrame.h"
#include "HcaInfo.h"
#include "../../utilities/Crc16.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "../../../common/utilities/macros.h"
#include "private/CriHcaEncoderStatic.h"
#include "CriHcaEncoder.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

using C = CriHcaConstants;

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
