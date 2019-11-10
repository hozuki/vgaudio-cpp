#include "../../utilities/Crc16.h"
#include "HcaInfo.h"
#include "CriHcaKey.h"
#include "CriHcaEncryption.h"

using namespace std;
using namespace vgaudio::codecs::crihca;
using namespace common_lib::utilities;
using namespace vgaudio::utilities;

static shared_ptr<Crc16> Crc;

struct _CriHcaEncryption_Static_Init {

    _CriHcaEncryption_Static_Init() noexcept {
        Crc = make_shared<Crc16>(0x8005);
    }

};

static _CriHcaEncryption_Static_Init _init;

static void CryptFrame(const shared_ptr<HcaInfo> &hca, const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key, bool decrypt) {
    const auto frameSize = hca->frameSize;
    const auto table = decrypt ? key->getDecryptionTable() : key->getEncryptionTable();

    for (auto b = 0; b < frameSize - 2; b += 1) {
        (*audio)[b] = (*table)[(*audio)[b]];
    }

    const auto crc16 = Crc->compute(audio, frameSize - 2);

    (*audio)[frameSize - 2] = static_cast<uint8_t>(crc16 >> 8);
    (*audio)[frameSize - 1] = static_cast<uint8_t>(crc16 & 0xff);
}

void CriHcaEncryption::encrypt(const shared_ptr<HcaInfo> &hca, const jarray2_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key) {
    for (auto frame = 0; frame < hca->frameCount; frame += 1) {
        encryptFrame(hca, (*audio)[frame], key);
    }
}

void CriHcaEncryption::decrypt(const shared_ptr<HcaInfo> &hca, const jarray2_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key) {
    for (auto frame = 0; frame < hca->frameCount; frame += 1) {
        decryptFrame(hca, (*audio)[frame], key);
    }
}

void CriHcaEncryption::encryptFrame(const shared_ptr<HcaInfo> &hca, const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key) {
    CryptFrame(hca, audio, key, false);
}

void CriHcaEncryption::decryptFrame(const shared_ptr<HcaInfo> &hca, const array_ptr<uint8_t> &audio, const shared_ptr<CriHcaKey> &key) {
    CryptFrame(hca, audio, key, true);
}
