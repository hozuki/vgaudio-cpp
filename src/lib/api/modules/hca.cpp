#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <memory>

#include "../../common/io/FileStream.h"
#include "../../vgaudio/codecs/crihca/CriHcaKey.h"
#include "../../vgaudio/formats/crihca/CriHcaFormat.h"
#include "../../vgaudio/formats/crihca/CriHcaFormatBuilder.h"
#include "../../vgaudio/formats/pcm16/Pcm16Format.h"
#include "../../vgaudio/containers/hca/HcaReader.h"
#include "../../vgaudio/containers/hca/HcaWriter.h"
#include "../../vgaudio/containers/hca/HcaConfiguration.h"
#include "../../vgaudio/containers/wave/WaveReader.h"
#include "../../vgaudio/containers/wave/WaveWriter.h"
#include "../../vgaudio/containers/wave/WaveConfiguration.h"
#include "progress_report.h"
#include "../../../api/modules/hca.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::formats::crihca;
using namespace vgaudio::containers::hca;
using namespace vgaudio::containers::wave;

VGA_API_IMPL(int32_t) vgaEncodeHcaFile(const char *waveFileIn, const char *hcaFileOut, const HCA_ENCODE_CONFIG *config) {
    const auto inputFileStream = make_shared<FileStream>(waveFileIn, "rb");
    const auto outputFileStream = make_shared<FileStream>(hcaFileOut, "wb");

    const auto waveReader = make_shared<WaveReader>();
    const auto hcaWriter = make_shared<HcaWriter>();

    const auto waveAudio = waveReader->readWithConfig(inputFileStream);

    const auto pcm16 = waveAudio->getAudioFormat()->toPcm16();

    CriHcaQuality q = CriHcaQuality::NotSet;

    if (config != nullptr) {
        q = static_cast<CriHcaQuality>(config->quality);
    }

    const auto hcaConfig = make_shared<HcaConfiguration>(*waveAudio->getConfiguration());
    hcaConfig->quality = q;

    if (config != nullptr && config->reporter != nullptr) {
        hcaConfig->progress = vgaudio::api::CreateProgressReporter(config->reporter, config->reporterData);
    }

    hcaWriter->writeToStream(pcm16, outputFileStream, hcaConfig);

    return 0;
}

VGA_API_IMPL(int32_t) vgaDecodeHcaFile(const char *hcaFileIn, const char *waveFileOut, const HCA_DECODE_CONFIG *config) {
    const auto inputFileStream = make_shared<FileStream>(hcaFileIn, "rb");
    const auto outputFileStream = make_shared<FileStream>(waveFileOut, "wb");

    const auto hcaReader = make_shared<HcaReader>();
    const auto waveWriter = make_shared<WaveWriter>();

    uint64_t key = 0;

    if (config != nullptr) {
        key = config->key;
    }

    if (key != 0) {
        const auto hcaKey = make_shared<CriHcaKey>(key);
        hcaReader->setEncryptionKey(hcaKey);
    }

    const auto hcaAudio = hcaReader->readWithConfig(inputFileStream);

    const auto pcm16 = hcaAudio->getAudioFormat()->toPcm16();

    const auto waveConfig = make_shared<WaveConfiguration>(*hcaAudio->getConfiguration());

    if (config != nullptr && config->reporter != nullptr) {
        waveConfig->progress = vgaudio::api::CreateProgressReporter(config->reporter, config->reporterData);
    }

    waveWriter->writeToStream(pcm16, outputFileStream, waveConfig);

    return 0;
}
