#include <algorithm>

#include "../../../common/utilities/ArrayHelper.h"
#include "../../IProgressReport.h"
#include "CriHcaParameters.h"
#include "private/CriHcaDecoderStatic.h"
#include "CriHcaFrame.h"
#include "CriHcaConstants.h"
#include "HcaInfo.h"
#include "CriHcaDecoder.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::crihca;

using C = CriHcaConstants;

jarray2_ptr<int16_t> CriHcaDecoder::decode(const shared_ptr<HcaInfo> &hca, const jarray2_ptr<uint8_t> &audio, const shared_ptr<CriHcaParameters> &config) {
    if (config && config->progress) {
        config->progress->setTotal(hca->frameCount);
    }

    auto pcmOut = make_jagged_array_2_dynamic<int16_t>(hca->channelCount, hca->sampleCount);
    auto pcmBuffer = make_jagged_array_2_dynamic<int16_t>(hca->channelCount, C::SamplesPerFrame);

    auto frame = make_shared<CriHcaFrame>(hca);

    for (auto i = 0; i < hca->frameCount; i += 1) {
        DecodeFrame((*audio)[i], frame, pcmBuffer);
        CopyPcmToOutput(pcmBuffer, pcmOut, hca, i);

        if (config && config->progress) {
            config->progress->reportAdd(1);
        }
    }

    return pcmOut;
}

void CriHcaDecoder::copyBuffer(const jarray2_ptr<int16_t> &bufferIn, const jarray2_ptr<int16_t> &bufferOut, int32_t startIndex, int32_t bufferIndex) {
    if (!bufferIn || !bufferOut || bufferIn->empty() || bufferOut->empty()) {
        throw std::invalid_argument("buffers should be non-null and positive-length");
    }

    const auto bufferLength = (*bufferIn)[0]->size();
    const auto outLength = (*bufferOut)[0]->size();

    const int32_t currentIndex = bufferIndex * bufferLength - startIndex;
    const int32_t remainingElements = std::min(outLength - currentIndex, outLength);
    const int32_t srcStart = std::clamp(0 - currentIndex, 0, C::SamplesPerFrame);
    const int32_t dstStart = std::max(currentIndex, 0);

    const int32_t length = std::min(C::SamplesPerFrame - srcStart, remainingElements);

    if (length <= 0) {
        return;
    }

    for (auto c = 0; c < bufferOut->size(); c += 1) {
        ArrayHelper::copyByValue((*bufferIn)[c], srcStart, (*bufferOut)[c], dstStart, length);
    }
}
