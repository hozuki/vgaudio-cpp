#include "../../../common/utilities/EndianHelper.h"
#include "../../../common/utilities/defer.h"
#include "../../../common/io/BinaryWriter.h"
#include "../../formats/crihca/CriHcaFormat.h"
#include "../../codecs/crihca/CriHcaParameters.h"
#include "../../codecs/crihca/CriHcaEncryption.h"
#include "../../codecs/crihca/HcaInfo.h"
#include "../../codecs/crihca/CriHcaKey.h"
#include "../../utilities/Crc16.h"
#include "HcaWriter.h"

using namespace std;
using namespace vgaudio::containers;
using namespace vgaudio::containers::hca;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::formats;
using namespace vgaudio::formats::crihca;
using namespace vgaudio::utilities;
using namespace common_lib::utilities;
using namespace common_lib::io;

HcaWriter::HcaWriter()
    : _version(0x0200) {
    _crc = make_shared<Crc16>(0x8005);
}

int32_t HcaWriter::getFileSize() {
    return getHeaderSize() + _hca->frameSize * _hca->frameCount;
}

void HcaWriter::setupWriter(const shared_ptr<AudioData> &audio) {
    auto configuration = getConfiguration();
    auto encodingConfig = make_shared<CriHcaParameters>();

    encodingConfig->progress = configuration->progress;
    encodingConfig->bitrate = configuration->bitrate;
    encodingConfig->limitBitrate = configuration->limitBitrate;

    if (configuration->quality != CriHcaQuality::NotSet) {
        encodingConfig->quality = configuration->quality;
    }

    const auto hcaFormat = AudioData::getFormat<CriHcaFormat>(audio, encodingConfig);
    _hca = hcaFormat->getHca();
    _audioData = hcaFormat->getAudioData();

    if (configuration->encryptionKey != nullptr) {
        CriHcaEncryption::encrypt(_hca, _audioData, configuration->encryptionKey);
        _hca->encryptionType = configuration->encryptionKey->getKeyType();
    }
}

void HcaWriter::writeStream(const shared_ptr<Stream> &stream) {
    writeHeader(stream);
    writeAudioData(stream);
}

int32_t HcaWriter::getHeaderSize() const {
    return _hca->headerSize;
}

void HcaWriter::writeHeader(const shared_ptr<Stream> &stream) {
    auto memory = make_shared<MemoryStream>();
    auto memoryWriter = EndianHelper::createBinaryWriter(memory, Endianess::BigEndian);

    writeHcaChunk(memoryWriter);
    writeFmtChunk(memoryWriter);
    writeCompChunk(memoryWriter);
    writeLoopChunk(memoryWriter);
    writeCiphChunk(memoryWriter);
    writeRvaChunk(memoryWriter);

    if (_hca->comment.length() == 0) {
        writePadChunk(memoryWriter);
    } else {
        writeCommChunk(memoryWriter);
    }

    memory->setPosition(0);
    auto header = make_array_dynamic<uint8_t>(getHeaderSize() - 2);
    dynamic_pointer_cast<Stream>(memory)->read(header, 0, header->size());
    const auto crc16 = _crc->compute(header, header->size());
    memory->setPosition(0);
    memoryWriter->write(header, header->size());
    memoryWriter->write(crc16);

    memory->writeTo(stream);
}

void HcaWriter::writeHcaChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "HCA");
    writer->write(_version);
    writer->write(static_cast<uint16_t>(getHeaderSize()));
}

void HcaWriter::writeFmtChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "fmt");
    writer->write(static_cast<uint8_t>(_hca->channelCount));

    // 24-bit, little endian
    writer->write(static_cast<uint8_t>(_hca->sampleRate >> 16));
    writer->write(static_cast<uint16_t>(_hca->sampleRate));

    writer->write(_hca->frameCount);
    writer->write(static_cast<uint16_t>(_hca->insertedSamples));
    writer->write(static_cast<uint16_t>(_hca->appendedSamples));
}

void HcaWriter::writeCompChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "comp");
    writer->write(static_cast<uint16_t>(_hca->frameSize));
    writer->write(static_cast<uint8_t>(_hca->minResolution));
    writer->write(static_cast<uint8_t>(_hca->maxResolution));
    writer->write(static_cast<uint8_t>(_hca->trackCount));
    writer->write(static_cast<uint8_t>(_hca->channelConfig));
    writer->write(static_cast<uint8_t>(_hca->totalBandCount));
    writer->write(static_cast<uint8_t>(_hca->baseBandCount));
    writer->write(static_cast<uint8_t>(_hca->stereoBandCount));
    writer->write(static_cast<uint8_t>(_hca->bandsPerHfrGroup));
    writer->write(static_cast<uint16_t>(0));
}

void HcaWriter::writeLoopChunk(const shared_ptr<BinaryWriter> &writer) {
    if (!_hca->looping) {
        return;
    }

    writeChunkId(writer, "loop");
    writer->write(_hca->loopStartFrame);
    writer->write(_hca->loopEndFrame);
    writer->write(static_cast<uint16_t>(_hca->preLoopSamples));
    writer->write(static_cast<uint16_t>(_hca->postLoopSamples));
}

void HcaWriter::writeCiphChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "ciph");
    writer->write(static_cast<uint16_t>(_hca->encryptionType));
}

void HcaWriter::writeRvaChunk(const shared_ptr<BinaryWriter> &writer) {
    const auto volume = _hca->volume;

    if (volume != 1) {
        writeChunkId(writer, "rva");
        writer->write(volume);
    }
}

void HcaWriter::writeCommChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "comm\0", 5);
    writer->writeNTString(_hca->comment);
}

void HcaWriter::writePadChunk(const shared_ptr<BinaryWriter> &writer) {
    writeChunkId(writer, "pad", 3);
}

void HcaWriter::writeChunkId(const shared_ptr<BinaryWriter> &writer, const char *id, size_t size) {
    auto w = new char[size];

    auto_defer([&]() {
        delete[] w;
    });

    for (auto i = 0; i < size; i += 1) {
        w[i] = id[i];
    }

    if (getConfiguration()->encryptionKey != nullptr) {
        for (auto i = 0; i < size; i += 1) {
            if (w[i] != 0) {
                w[i] = w[i] | 0x80;
            }
        }
    }

    writer->write(w, size, 0, size);
}

void HcaWriter::writeAudioData(const shared_ptr<Stream> &stream) {
    const auto frameCount = _hca->frameCount;
    const auto frameSize = _hca->frameSize;

    const auto &audioData = _audioData;

    for (auto i = 0; i < frameCount; i += 1) {
        const auto &data = (*audioData)[i];

        stream->write(data, 0, frameSize);
    }
}
