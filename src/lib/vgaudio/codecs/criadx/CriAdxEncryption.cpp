#include "CriAdxEncryption.h"
#include "CriAdxKey.h"
#include "../../../common/utilities/IntHelper.h"

using namespace std;
using namespace common_lib::utilities;
using namespace vgaudio::codecs::criadx;

static bool FrameNotEmpty(const array_ptr<uint8_t> &bytes, int32_t start, int32_t length) {
    const auto end = start + length;

    for (auto i = start; i < end; i += 1) {
        if ((*bytes)[i] != 0) {
            return true;
        }
    }

    return false;
}

static void EncryptDecryptChannel(const array_ptr<uint8_t> &adpcm, const shared_ptr<CriAdxKey> &key, int32_t encryptionType, int32_t frameSize, int32_t channelIndex, int32_t channelCount) {
    int32_t xorNum = key->getSeed();
    const int32_t frameCount = IntHelper::divideByRoundUp(adpcm->size(), frameSize);

    const auto mul = key->getMul();
    const auto inc = key->getInc();

    for (auto i = 0; i < channelIndex; i++) {
        xorNum = (xorNum * mul + inc) & 0x7fff;
    }

    for (auto i = 0; i < frameCount; i += 1) {
        const auto pos = i * frameSize;

        if (FrameNotEmpty(adpcm, pos, frameSize)) {
            (*adpcm)[pos] ^= static_cast<uint8_t>(xorNum >> 8);

            if (encryptionType == 9) {
                (*adpcm)[pos] &= 0x1f;
            }

            (*adpcm)[pos + 1] ^= static_cast<uint8_t>(xorNum);
        }

        for (auto c = 0; c < channelCount; c += 1) {
            xorNum = (xorNum * mul + inc) & 0x7fff;
        }
    }
}

void CriAdxEncryption::encryptDecrypt(const jarray2_ptr<uint8_t> &adpcm, const shared_ptr<CriAdxKey> &key, int32_t encryptionType, int32_t frameSize) {
    const auto channelCount = adpcm->size();

    for (auto i = 0; i < channelCount; i += 1) {
        EncryptDecryptChannel((*adpcm)[i], key, encryptionType, frameSize, i, channelCount);
    }
}
