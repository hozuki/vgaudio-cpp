#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <algorithm>

#include "../../../common//utilities/_cxx17.h"
#include "CriAdxCodec.h"
#include "CriAdxParameters.h"
#include "../../IProgressReport.h"
#include "../../../common/utilities/runtime_jagged_array.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/ArrayHelper.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::criadx;

static jarray2_ptr<int16_t> Coefficients;

struct _CriAdx_Coefficients_Init {

    _CriAdx_Coefficients_Init() noexcept {
        Coefficients = make_jagged_array_2_dynamic<int16_t>(4, 2);

        (*(*Coefficients)[0])[0] = 0x0000;
        (*(*Coefficients)[0])[1] = 0x0000;
        (*(*Coefficients)[1])[0] = 0x0f00;
        (*(*Coefficients)[1])[1] = 0x0000;
        (*(*Coefficients)[2])[0] = 0x1cc0;
        (*(*Coefficients)[2])[1] = 0xf300;
        (*(*Coefficients)[3])[0] = 0x1880;
        (*(*Coefficients)[3])[1] = 0xf240;
    }

};

_CXX17_MAYBE_UNUSED
static _CriAdx_Coefficients_Init _init;

static int32_t CalculateScale(int32_t maxDistance, double *gain, int32_t *scaleToWrite, bool exponential = false) {
    int32_t scale = (maxDistance - 1) / 7 + 1;

    if (scale > 0x1000) {
        scale = 0x1000;
    }

    *scaleToWrite = scale - 1;

    if (exponential) {
        const int32_t power = *scaleToWrite == 0 ? 0 : (std::log2(*scaleToWrite) + 1);
        scale = 1 << power;
        *scaleToWrite = 12 - power;
        maxDistance = 8 * scale - 1;
    }

    *gain = maxDistance == 0 ? 0 : (static_cast<double>(INT16_MAX) / maxDistance);

    return scale;
}

static int32_t ScaleShortToNibble(int32_t sample) {
    sample = (sample + (INT16_MAX / 14) * IntHelper::sgn(sample)) / (INT16_MAX / 7);
    return IntHelper::clamp(sample, -8, 7);
}

static array_ptr<int16_t> CalculateCoefficients(int32_t highPassFrequency, int32_t sampleRate) {
    const double a = M_SQRT2 - std::cos(2 * M_PI * highPassFrequency / sampleRate);
    constexpr double b = M_SQRT2 - 1;
    const double c = (a - std::sqrt((a + b) * (a - b))) / b;

    const auto coeff1 = static_cast<int16_t>(c * 8192);
    const auto coeff2 = static_cast<int16_t>(c * c * (-4096));

    const auto arr = make_array_dynamic_with(coeff1, coeff2);

    return arr;
}

static void EncodeFrame(const array_ptr<int16_t> &pcmIn, const array_ptr<uint8_t> &adpcmOut, const array_ptr<int16_t> &coeffs, int32_t samplesPerFrame, CriAdxType type, int32_t version) {
    int32_t maxDistance = 0;
    auto adpcm = make_array_dynamic<int32_t>(samplesPerFrame);

    for (auto i = 0; i < samplesPerFrame; i += 1) {
        const int32_t predictedSample = (((*pcmIn)[i + 1] * (*coeffs)[0]) >> 12) + (((*pcmIn)[i] * (*coeffs)[1]) >> 12);
        int32_t distance = (*pcmIn)[i + 2] - predictedSample;
        distance = std::abs(static_cast<int32_t>(IntHelper::clamp<int32_t>(distance, INT16_MIN, INT16_MAX)));

        if (distance > maxDistance) {
            maxDistance = distance;
        }
    }

    double gain;
    int32_t scaleOut;

    const auto scale = CalculateScale(maxDistance, &gain, &scaleOut, type == CriAdxType::Exponential);

    for (auto i = 0; i < samplesPerFrame; i += 1) {
        int32_t predictedSample = (((*pcmIn)[i + 1] * (*coeffs)[0]) >> 12) + (((*pcmIn)[i] * (*coeffs)[1]) >> 12);
        const int32_t rawDistance = (*pcmIn)[i + 2] - predictedSample;
        const int32_t scaledDistance = IntHelper::clamp<int32_t>(static_cast<int32_t>(rawDistance * gain), INT16_MIN, INT16_MAX);

        const int32_t adpcmSample = ScaleShortToNibble(scaledDistance);
        (*adpcm)[i] = adpcmSample;

        const int16_t decodedDistance = IntHelper::clamp<int32_t>(scale * adpcmSample, INT16_MIN, INT16_MAX);

        if (version == 4) {
            predictedSample = ((*pcmIn)[i + 1] * (*coeffs)[0] + (*pcmIn)[i] * (*coeffs)[1]) >> 12;
        }

        const int32_t decodedSample = decodedDistance + predictedSample;

        (*pcmIn)[i + 2] = IntHelper::clamp<int32_t>(decodedSample, INT16_MIN, INT16_MAX);
    }

    (*adpcmOut)[0] = static_cast<uint8_t>((scaleOut >> 8) & 0x1f);
    (*adpcmOut)[1] = static_cast<uint8_t>(scaleOut);

    const auto adpcmLength = samplesPerFrame / 2;

    for (auto i = 0; i < adpcmLength; i += 1) {
        (*adpcmOut)[i + 2] = IntHelper::combineNibbles((*adpcm)[i * 2], (*adpcm)[i * 2 + 1]);
    }
}

array_ptr<int16_t> CriAdxCodec::decode(const array_ptr<uint8_t> &adpcm, int32_t sampleCount, const shared_ptr<CriAdxParameters> &config) {
    shared_ptr<CriAdxParameters> c;

    if (config == nullptr) {
        c = make_shared<CriAdxParameters>();
    } else {
        c = config;
    }

    const int32_t samplesPerFrame = (c->frameSize - 2) * 2;

    jarray2_ptr<int16_t> coeffs;

    if (c->type == CriAdxType::Fixed) {
        coeffs = Coefficients;
    } else {
        const auto coeffRow0 = CalculateCoefficients(c->highPassFrequency, c->sampleRate);
        coeffs = make_jagged_array_2_dynamic<int16_t>(1);
        (*coeffs)[0] = coeffRow0;
    }

    const auto pcm = make_array_dynamic<int16_t>(sampleCount);

    int32_t hist1 = c->history;
    int32_t hist2 = c->history;
    const auto frameCount = IntHelper::divideByRoundUp(sampleCount, samplesPerFrame);

    int32_t currentSample = 0;
    int32_t startSample = c->padding > 0 ? (c->padding % samplesPerFrame) : 0;
    int32_t inIndex = c->padding / samplesPerFrame * c->frameSize;

    const auto progress = config->progress;

    for (auto i = 0; i < frameCount; i += 1) {
        const int32_t filterNum = IntHelper::getHighNibble((*adpcm)[inIndex]) >> 1;
        auto scale = static_cast<int16_t>((((*adpcm)[inIndex] << 8) | ((*adpcm)[inIndex + 1])) & 0x1fff);
        scale = static_cast<int16_t>(c->type == CriAdxType::Exponential ? (1 << (12 - scale)) : (scale + 1));
        inIndex += 2 + startSample / 2;

        const int32_t samplesToRead = std::min(samplesPerFrame, sampleCount - currentSample);

        for (auto s = startSample; s < samplesToRead; s += 1) {
            int32_t sample = s % 2 == 0 ? IntHelper::getHighNibbleSigned((*adpcm)[inIndex]) : IntHelper::getLowNibbleSigned((*adpcm)[inIndex]);
            inIndex += 1;

            if (c->version == 4) {
                sample = scale * sample + ((hist1 * (*(*coeffs)[filterNum])[0] + hist2 * (*(*coeffs)[filterNum])[1]) >> 12);
            } else {
                sample = scale * sample + ((hist1 * (*(*coeffs)[filterNum])[0]) >> 12) + ((hist2 * (*(*coeffs)[filterNum])[1]) >> 12);
            }

            const int16_t finalSample = IntHelper::clamp<int32_t>(sample, INT16_MIN, INT16_MAX);

            hist2 = hist1;
            hist1 = finalSample;
            (*pcm)[currentSample] = finalSample;
            currentSample += 1;
        }

        startSample = 0;

        if (progress != nullptr) {
            progress->reportAdd(1);
        }
    }

    return pcm;
}

array_ptr<uint8_t> CriAdxCodec::encode(const array_ptr<int16_t> &pcm, const shared_ptr<CriAdxParameters> &config) {
    const int32_t sampleCount = pcm->size() + config->padding;
    const int32_t samplesPerFrame = (config->frameSize - 2) * 2;
    const int32_t frameCount = IntHelper::divideByRoundUp(sampleCount, samplesPerFrame);
    int32_t paddingRemaining = config->padding;

    array_ptr<int16_t> coeffs;

    if (config->type == CriAdxType::Fixed) {
        coeffs = (*Coefficients)[config->filter];
    } else {
        coeffs = CalculateCoefficients(500, config->sampleRate);
    }

    const auto pcmBuffer = make_array_dynamic<int16_t>(samplesPerFrame * 2);
    const auto adpcmBuffer = make_array_dynamic<uint8_t>(config->frameSize);
    const auto adpcmOut = make_array_dynamic<uint8_t>(frameCount * config->frameSize);

    if (config->version == 4 && config->padding == 0) {
        (*pcmBuffer)[0] = (*pcm)[0];
        (*pcmBuffer)[1] = (*pcm)[0];
        config->history = (*pcm)[0];
    }

    const auto progress = config->progress;

    for (auto i = 0; i < frameCount; i += 1) {
        int32_t samplesToCopy = std::min(samplesPerFrame, sampleCount - i * samplesPerFrame);
        int32_t pcmBufferStart = 2;

        if (paddingRemaining > 0) {
            while (paddingRemaining > 0 && samplesToCopy > 0) {
                paddingRemaining -= 1;
                samplesToCopy -= 1;
                pcmBufferStart += 1;
            }

            if (samplesToCopy == 0) {
                continue;
            }
        }

        ArrayHelper::copyByValue(pcm, std::max(i * samplesPerFrame - config->padding, 0), pcmBuffer, pcmBufferStart, samplesToCopy);
        ArrayHelper::clearByValue(pcmBuffer, pcmBufferStart + samplesToCopy, samplesPerFrame - samplesToCopy - (pcmBufferStart - 2));

        EncodeFrame(pcmBuffer, adpcmBuffer, coeffs, samplesPerFrame, config->type, config->version);

        if (config->type == CriAdxType::Fixed) {
            (*adpcmBuffer)[0] |= static_cast<uint8_t>(config->filter << 5);
        }

        ArrayHelper::copyByValue(adpcmBuffer, 0, adpcmOut, i * config->frameSize, config->frameSize);

        (*pcmBuffer)[0] = (*pcmBuffer)[samplesPerFrame];
        (*pcmBuffer)[1] = (*pcmBuffer)[samplesPerFrame + 1];

        if (progress != nullptr) {
            progress->reportAdd(1);
        }
    }

    return adpcmOut;
}
