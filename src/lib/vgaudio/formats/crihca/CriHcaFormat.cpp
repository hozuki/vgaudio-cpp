#include <cmath>
#include <type_traits>

#include "../../../common/utilities/ArrayHelper.h"
#include "../../IProgressReport.h"
#include "../../codecs/crihca/HcaInfo.h"
#include "../../codecs/crihca/CriHcaParameters.h"
#include "../../codecs/crihca/CriHcaDecoder.h"
#include "../../codecs/crihca/CriHcaEncoder.h"
#include "../../codecs/crihca/CriHcaConstants.h"
#include "../pcm16/Pcm16Format.h"
#include "../pcm16/Pcm16FormatBuilder.h"
#include "CriHcaFormatBuilder.h"
#include "CriHcaFormat.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::formats;
using namespace vgaudio::formats::pcm16;
using namespace vgaudio::formats::crihca;

CriHcaFormat::CriHcaFormat(const shared_ptr<CriHcaFormatBuilder> &builder)
    : MyBase(builder), _hca(builder->getHca()), _audioData(builder->getAudioData()) {
}

shared_ptr<HcaInfo> CriHcaFormat::getHca() const {
    return _hca;
}

jarray2_ptr<uint8_t> CriHcaFormat::getAudioData() const {
    return _audioData;
}

shared_ptr<Pcm16Format> CriHcaFormat::toPcm16() {
    return toPcm16(make_shared<CodecParameters>());
}

shared_ptr<Pcm16Format> CriHcaFormat::toPcm16(const shared_ptr<CodecParameters> &config) {
    return toPcm16(make_shared<CriHcaParameters>(*config));
}

shared_ptr<Pcm16Format> CriHcaFormat::toPcm16(const shared_ptr<CriHcaParameters> &config) {
    auto audio = CriHcaDecoder::decode(getHca(), getAudioData(), config);

    auto builder = make_shared<Pcm16FormatBuilder>(audio, getSampleRate());
    builder->withLoop(isLooping(), getUnalignedLoopStart(), getUnalignedLoopEnd());
    return dynamic_pointer_cast<Pcm16Format>(builder->build());
}

shared_ptr<IAudioFormat> CriHcaFormat::encodeFromPcm16(const shared_ptr<Pcm16Format> &pcm16) {
    auto config = make_shared<CriHcaParameters>();
    config->channelCount = pcm16->getChannelCount();
    config->sampleRate = pcm16->getSampleRate();
    return encodeFromPcm16WithConfig(pcm16, config);
}

shared_ptr<IAudioFormat> CriHcaFormat::encodeFromPcm16WithConfig(const shared_ptr<Pcm16Format> &pcm16, const shared_ptr<CodecParameters> &config) {
    auto cfg = dynamic_pointer_cast<CriHcaParameters>(config);
    cfg->channelCount = pcm16->getChannelCount();
    cfg->sampleRate = pcm16->getSampleRate();
    cfg->sampleCount = pcm16->getSampleCount();
    cfg->looping = pcm16->isLooping();
    cfg->loopStart = pcm16->getLoopStart();
    cfg->loopEnd = pcm16->getLoopEnd();
    const auto &progress = config->progress;

    auto encoder = CriHcaEncoder::createNew(cfg);
    const auto &hca = encoder->getHca();

    const auto &pcm = pcm16->getChannels();
    const auto &pcmBuffer = make_jagged_array_2_dynamic<int16_t>(pcm16->getChannelCount(), CriHcaConstants::SamplesPerFrame);

    if (progress != nullptr) {
        progress->setTotal(hca->frameCount);
    }

    auto audio = make_jagged_array_2_dynamic<uint8_t>(hca->frameCount, encoder->getFrameSize());

    int32_t frameNum = 0;
    for (auto i = 0; frameNum < hca->frameCount; i += 1) {
        const auto samplesToCopy = std::min(pcm16->getSampleCount() - i * CriHcaConstants::SamplesPerFrame, CriHcaConstants::SamplesPerFrame);

        for (auto c = 0; c < pcm->size(); c += 1) {
            ArrayHelper::copyByValue((*pcm)[c], i * CriHcaConstants::SamplesPerFrame, (*pcmBuffer)[c], 0, samplesToCopy);
        }

        auto framesWritten = encoder->encode(pcmBuffer, (*audio)[frameNum]);

        if (framesWritten == 0) {
            throw std::runtime_error("should not happen: encoder didn't encode any audio");
        }

        if (framesWritten > 0) {
            frameNum += 1;
            framesWritten -= 1;

            if (progress != nullptr) {
                progress->reportAdd(1);
            }
        }

        while (framesWritten > 0) {
            (*audio)[frameNum] = encoder->getPendingFrame();
            frameNum += 1;
            framesWritten -= 1;

            if (progress != nullptr) {
                progress->reportAdd(1);
            }
        }
    }

    auto builder = make_shared<CriHcaFormatBuilder>(audio, hca);
    return builder->build();
}

shared_ptr<AudioFormatBase> CriHcaFormat::getChannelsInternal(const array_ptr<int32_t> &channelRange) {
    throw std::runtime_error("unsupported");
}

shared_ptr<AudioFormatBase> CriHcaFormat::addInternal(const shared_ptr<IAudioFormat> &format) {
    throw std::runtime_error("unsupported");
}

shared_ptr<AudioFormatBaseBuilder> CriHcaFormat::getCloneBuilder() {
    throw std::runtime_error("unsupported");
}

bool CriHcaFormat::canAcceptAudioFormat(const IAudioFormat *format) const {
    return dynamic_cast<const CriHcaFormat *>(format) != nullptr;
}

bool CriHcaFormat::canAcceptConfig(const CodecParameters *config) const {
    return dynamic_cast<const CriHcaParameters *>(config) != nullptr;
}

shared_ptr<CodecParameters> CriHcaFormat::createConfig() const {
    return make_shared<CriHcaParameters>();
}
