#include "AdxWriter.h"
#include "../../formats/criadx/CriAdxFormat.h"
#include "../../formats/criadx/CriAdxHelpers.h"
#include "../../formats/criadx/CriAdxChannel.h"
#include "../../codecs/criadx/CriAdxParameters.h"
#include "../../codecs/criadx/CriAdxEncryption.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../../common/utilities/EndianHelper.h"
#include "../../../common/utilities/ArrayHelper.h"
#include "../../../common/io/Stream.h"
#include "../../../common/io/BinaryWriter.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::containers::adx;
using namespace vgaudio::codecs::criadx;
using namespace vgaudio::formats;
using namespace vgaudio::formats::criadx;

namespace vgaudio {
    namespace containers {
        namespace adx {
            namespace __internal {

                struct AdxWriterFormatHelper {

                private:

                    shared_ptr<CriAdxFormat> _adpcm;
                    shared_ptr<AdxConfiguration> _config;
                    int32_t _alignmentBytes;

                public:

                    explicit AdxWriterFormatHelper(const shared_ptr<CriAdxFormat> &adpcm, const shared_ptr<AdxConfiguration> &config)
                        : _adpcm(adpcm), _config(config), _alignmentBytes(0) {
                    }

                    [[nodiscard]]
                    int32_t getFrameSize() const {
                        return _adpcm->getFrameSize();
                    }

                    [[nodiscard]]
                    int32_t getSamplesPerFrame() const {
                        return (getFrameSize() - 2) * 2;
                    }

                    [[nodiscard]]
                    int32_t getVersion() const {
                        return _adpcm->getVersion();
                    }

                    [[nodiscard]]
                    int32_t getSampleCount() const {
                        return _config->trimFile && _adpcm->isLooping() ? (_adpcm->getLoopEnd() + getSamplesPerFrame() * 3) : _adpcm->getSampleCount();
                    }

                    [[nodiscard]]
                    int32_t getFrameCount() const {
                        return IntHelper::divideByRoundUp(getSampleCount(), getSamplesPerFrame());
                    }

                    [[nodiscard]]
                    int32_t getChannelCount() const {
                        return _adpcm->getChannelCount();
                    }

                    [[nodiscard]]
                    int32_t getAlignmentBytes() const {
                        return _alignmentBytes;
                    }

                    [[nodiscard]]
                    int32_t getBaseHeaderSize() const {
                        return _adpcm->isLooping() ? (getVersion() == 4 ? 60 : 52) : (getVersion() == 4 ? 36 : 32);
                    }

                    [[nodiscard]]
                    int32_t getHeaderSize() const {
                        return getBaseHeaderSize() + getAlignmentBytes();
                    }

                    [[nodiscard]]
                    int32_t getAudioOffset() const {
                        return getHeaderSize() + 4;
                    }

                    [[nodiscard]]
                    int32_t getAudioSize() const {
                        return getFrameSize() * getFrameCount() * getChannelCount();
                    }

                    [[nodiscard]]
                    int32_t getFooterOffset() const {
                        return getAudioOffset() + getAudioSize();
                    }

                    [[nodiscard]]
                    int32_t getFooterSize() const {
                        return _adpcm->isLooping() ? (IntHelper::getNextMultiple(getFooterOffset() + getFrameSize(), 0x800) - getFooterOffset()) : getFrameSize();
                    }

                    [[nodiscard]]
                    int32_t getLoopStartOffset() const {
                        return getAudioOffset() + CriAdxHelpers::sampleCountToByteCount(_adpcm->getLoopStart(), getFrameSize()) * getChannelCount();
                    }

                    [[nodiscard]]
                    int32_t getLoopEndOffset() const {
                        return getAudioOffset() + IntHelper::getNextMultiple(CriAdxHelpers::sampleCountToByteCount(_adpcm->getLoopEnd(), getFrameSize()), getFrameSize()) * getChannelCount();
                    }

                    void calculateAlignmentBytes() {
                        const int32_t startLoopOffset = CriAdxHelpers::sampleCountToByteCount(_adpcm->getLoopStart(), getFrameSize()) * getChannelCount() + getBaseHeaderSize() + 4;
                        _alignmentBytes = IntHelper::getNextMultiple(startLoopOffset, 0x800) - startLoopOffset;

                        if (getVersion() == 3) {
                            _alignmentBytes += _adpcm->getAlignmentSamples() / getSamplesPerFrame() * 0x800;
                        }
                    }

                };

            }
        }
    }
}

#define ADX_HEADER_SIGNATURE (0x8000)
#define ADX_FOOTER_SIGNATURE (0x8001)

static void WriteHeader(const shared_ptr<BinaryWriter> &writer, const shared_ptr<CriAdxFormat> &adpcm, const shared_ptr<vgaudio::containers::adx::__internal::AdxWriterFormatHelper> &h, const shared_ptr<AdxConfiguration> &config) {
    writer->write(static_cast<uint16_t>(ADX_HEADER_SIGNATURE));
    writer->write(static_cast<int16_t>(h->getHeaderSize()));
    writer->write(static_cast<uint8_t>(adpcm->getType()));
    writer->write(static_cast<uint8_t>(h->getFrameSize()));
    writer->write(static_cast<uint8_t>(4));
    writer->write(static_cast<uint8_t>(h->getChannelCount()));
    writer->write(adpcm->getSampleRate());
    writer->write(h->getSampleCount());
    writer->write(adpcm->getType() != CriAdxType::Fixed ? adpcm->getHighPassFrequency() : static_cast<int16_t>(0));
    writer->write(static_cast<uint8_t>(h->getVersion()));
    writer->write(static_cast<uint8_t>(config->encryptionType));

    if (h->getVersion() == 4) {
        writer->write(static_cast<int32_t>(0));

        const auto channels = adpcm->getChannels();
        const auto channelCount = h->getChannelCount();

        for (auto i = 0; i < channelCount; i += 1) {
            writer->write((*channels)[i]->history);
            writer->write((*channels)[i]->history);
        }

        if (channelCount == 1) {
            writer->write(static_cast<int32_t>(0));
        }
    }

    writer->write(static_cast<int16_t>(adpcm->getAlignmentSamples()));
    writer->write(static_cast<int16_t>(adpcm->isLooping() ? 1 : 0));
    writer->write(static_cast<int32_t>(adpcm->isLooping() ? 1 : 0));
    writer->write(adpcm->getLoopStart());
    writer->write(h->getLoopStartOffset());
    writer->write(adpcm->getLoopEnd());
    writer->write(h->getLoopEndOffset());

    writer->getBaseStream()->setPosition(h->getHeaderSize() - 2);
    writer->writeFixedString(std::string("(c)CRI"));
}

static void WriteData(const shared_ptr<BinaryWriter> &writer, const shared_ptr<CriAdxFormat> &adpcm, const shared_ptr<vgaudio::containers::adx::__internal::AdxWriterFormatHelper> &h, const shared_ptr<AdxConfiguration> &config) {
    const auto channels = adpcm->getChannels();
    const auto channelCount = channels->size();

    auto audio = make_jagged_array_2_dynamic<uint8_t>(channelCount);

    if (config->encryptionKey != nullptr) {
        for (auto i = 0; i < channelCount; i += 1) {
            const auto source = (*channels)[i]->audio;

            // Do not encrypt original audio data; execute on a copy.
            const auto data = make_array_dynamic<uint8_t>(source->size());
            ArrayHelper::copyByValue(source, data, source->size());

            (*audio)[i] = data;
        }

        CriAdxEncryption::encryptDecrypt(audio, config->encryptionKey, config->encryptionType, h->getFrameSize());
    } else {
        for (auto i = 0; i < channelCount; i += 1) {
            (*audio)[i] = (*channels)[i]->audio;
        }
    }
}

static void WriteFooter(const shared_ptr<BinaryWriter> &writer, const shared_ptr<vgaudio::containers::adx::__internal::AdxWriterFormatHelper> &h) {
    const int32_t padding = h->getFooterSize() - 4;
    writer->write(static_cast<uint16_t>(ADX_FOOTER_SIGNATURE));
    writer->write(static_cast<int16_t>(padding));
}

int32_t AdxWriter::getFileSize() {
    return _h->getAudioOffset() + _h->getAudioSize() + _h->getFooterSize();
}

void AdxWriter::setupWriter(const shared_ptr<AudioData> &audio) {
    const auto configuration = getConfiguration();

    const auto encodingConfig = make_shared<CriAdxParameters>();
    encodingConfig->progress = configuration->progress;
    encodingConfig->version = configuration->version;
    encodingConfig->frameSize = configuration->frameSize;
    encodingConfig->filter = configuration->filter;
    encodingConfig->type = configuration->type;

    const auto adpcm = AudioData::getFormat<CriAdxFormat>(audio, encodingConfig);

    _adpcm = adpcm;
    _h = make_shared<__internal::AdxWriterFormatHelper>(adpcm, configuration);

    if (adpcm->isLooping()) {
        _h->calculateAlignmentBytes();
    }
}

void AdxWriter::writeStream(const shared_ptr<Stream> &stream) {
    auto writer = EndianHelper::createBinaryWriter(stream, Endianess::BigEndian);

    const auto configuration = getConfiguration();

    WriteHeader(writer, _adpcm, _h, configuration);
    WriteData(writer, _adpcm, _h, configuration);
    WriteFooter(writer, _h);
}
