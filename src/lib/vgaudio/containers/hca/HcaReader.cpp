#include <algorithm>

#include "../../../common/io/BinaryReader.h"
#include "../../../common/utilities/EndianHelper.h"
#include "../../../common/utilities/BitHelper.h"
#include "../../../common/utilities/StrHelper.h"
#include "../../codecs/crihca/CriHcaEncryption.h"
#include "../../codecs/crihca/HcaInfo.h"
#include "../../codecs/crihca/CriHcaKey.h"
#include "../../formats/crihca/CriHcaFormatBuilder.h"
#include "../../utilities/Crc16.h"
#include "HcaReader.h"

using namespace std;
using namespace vgaudio::utilities;
using namespace vgaudio::containers::hca;
using namespace vgaudio::codecs::crihca;
using namespace vgaudio::formats::crihca;
using namespace common_lib::io;

static shared_ptr<Crc16> Crc;

struct _HcaReader_Static_Init {

    _HcaReader_Static_Init() noexcept {
        Crc = make_shared<Crc16>(0x8005);
    }

};

[[maybe_unused]]
static _HcaReader_Static_Init _init;

static inline void ReadChunkId(const shared_ptr<BinaryReader> &reader, char *buffer, size_t size = 4) {
    reader->readBytes(buffer, size, 0, size);

    for (auto i = 0; i < size; i += 1) {
        buffer[i] = static_cast<char>(static_cast<uint32_t>(buffer[i]) & 0x7fu);
    }
}

static inline bool SignaturesMatch(const char *sig, const char *ref, size_t size = 4) {
    for (auto i = 0; i < size; i += 1) {
        if (sig[i] != ref[i]) {
            return false;
        }
    }

    return true;
}

static void ReadHcaData(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto &hca = structure->hca;
    const auto audioData = make_jagged_array_2_dynamic<uint8_t>(hca->frameCount);
    structure->audioData = audioData;

    for (auto i = 0; i < hca->frameCount; i += 1) {
        auto data = reader->readBytes(hca->frameSize);

        const auto computedCrc = Crc->compute(data, data->size() - 2);
        const auto expectedCrc = ((*data)[data->size() - 2] << 8) | (*data)[data->size() - 1];

        if (computedCrc != expectedCrc) {
            // TODO: something is wrong; data may be contaminated
        }

        (*audioData)[i] = data;
    }
}

static void ReadFmtChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto hca = structure->hca;
    hca->channelCount = reader->readUInt8();

    {
        const auto b = reader->readUInt8();
        const auto s = reader->readUInt16();
        hca->sampleRate = (b << 16) | s;
    }

    hca->frameCount = reader->readInt32();
    hca->insertedSamples = reader->readUInt16();
    hca->appendedSamples = reader->readUInt16();
    hca->sampleCount = hca->frameCount * 1024 - hca->insertedSamples - hca->appendedSamples;
}

static void ReadCompChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto &hca = structure->hca;
    hca->frameSize = reader->readUInt16();
    hca->minResolution = reader->readUInt8();
    hca->maxResolution = reader->readUInt8();
    hca->trackCount = reader->readUInt8();
    hca->channelConfig = reader->readUInt8();
    hca->totalBandCount = reader->readUInt8();
    hca->baseBandCount = reader->readUInt8();
    hca->stereoBandCount = reader->readUInt8();
    hca->bandsPerHfrGroup = reader->readUInt8();
    structure->reserved1 = reader->readUInt8();
    structure->reserved2 = reader->readUInt8();
}

static void ReadDecChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto &hca = structure->hca;
    hca->frameSize = reader->readUInt16();
    hca->minResolution = reader->readUInt8();
    hca->maxResolution = reader->readUInt8();
    hca->totalBandCount = reader->readUInt8() + 1;
    hca->baseBandCount = reader->readUInt8() + 1;

    {
        const auto a = reader->readUInt8();
        hca->trackCount = BitHelper::getHighNibble(a);
        hca->channelConfig = BitHelper::getLowNibble(a);
    }

    hca->decStereoType = reader->readUInt8();

    if (hca->decStereoType == 0) {
        hca->baseBandCount = hca->totalBandCount;
    } else {
        hca->stereoBandCount = hca->totalBandCount - hca->baseBandCount;
    }
}

static void ReadLoopChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto &hca = structure->hca;
    hca->looping = true;
    hca->loopStartFrame = reader->readInt32();
    hca->loopEndFrame = reader->readInt32();
    hca->preLoopSamples = reader->readUInt16();
    hca->postLoopSamples = reader->readUInt16();
    hca->sampleCount = std::min(hca->sampleCount, hca->getLoopEndSample());
}

static void ReadAthChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    const auto ath = reader->readUInt16();
    structure->hca->useAthCurve = ath != 0;
}

static void ReadVbrChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    structure->hca->vbrMaxFrameSize = reader->readUInt16();
    structure->hca->vbrNoiseLevel = reader->readUInt16();
}

static void ReadCiphChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    structure->hca->encryptionType = static_cast<CriHcaKeyType>(reader->readUInt16());
}

static void ReadRvaChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    structure->hca->volume = reader->readSingle();
}

static void ReadCommChunk(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    auto stream = reader->getBaseStream();
    stream->setPosition(stream->getPosition() + 1);
    structure->hca->comment = reader->readNTString();
}

static void ReadHcaHeader(const shared_ptr<BinaryReader> &reader, const shared_ptr<HcaStructure> &structure) {
    char signature[5] = {0};

    const auto &hca = structure->hca;

    ReadChunkId(reader, signature);

    if (!SignaturesMatch(signature, "HCA\0")) {
        throw std::runtime_error("invalid HCA file");
    }

    structure->version = reader->readUInt16();
    structure->headerSize = reader->readUInt16();
    hca->headerSize = structure->headerSize;

    auto hasAthChunk = false;
    auto baseStream = reader->getBaseStream();

    while (baseStream->getPosition() < structure->headerSize) {
        ReadChunkId(reader, signature);

        if (SignaturesMatch(signature, "fmt\0")) {
            ReadFmtChunk(reader, structure);
        } else if (SignaturesMatch(signature, "comp")) {
            ReadCompChunk(reader, structure);
        } else if (SignaturesMatch(signature, "dec\0")) {
            ReadDecChunk(reader, structure);
        } else if (SignaturesMatch(signature, "loop")) {
            ReadLoopChunk(reader, structure);
        } else if (SignaturesMatch(signature, "ath\0")) {
            ReadAthChunk(reader, structure);
            hasAthChunk = true;
        } else if (SignaturesMatch(signature, "ciph")) {
            ReadCiphChunk(reader, structure);
        } else if (SignaturesMatch(signature, "rva\0")) {
            ReadRvaChunk(reader, structure);
        } else if (SignaturesMatch(signature, "vbr\0")) {
            ReadVbrChunk(reader, structure);
        } else if (SignaturesMatch(signature, "comm")) {
            ReadCommChunk(reader, structure);
            baseStream->setPosition(structure->headerSize);
        } else if (SignaturesMatch(signature, "pad\0")) {
            baseStream->setPosition(structure->headerSize);
        } else {
            throw std::runtime_error(StrHelper::format("unsupported chunk '%s'", signature));
        }
    }

    if (structure->version < 0x0200 && !hasAthChunk) {
        hca->useAthCurve = true;
    }

    if (hca->trackCount < 1) {
        hca->trackCount = 1;
    }

    hca->calculateHfrValues();
}

shared_ptr<CriHcaKey> HcaReader::getEncryptionKey() const {
    return _encryptionKey;
}

void HcaReader::setEncryptionKey(const shared_ptr<CriHcaKey> &encryptionKey) {
    _encryptionKey = encryptionKey;
}

shared_ptr<HcaStructure> HcaReader::readFile(const shared_ptr<Stream> &stream, bool readAudioData) {
    auto reader = EndianHelper::createBinaryReader(stream, Endianess::BigEndian);

    auto structure = make_shared<HcaStructure>();

    ReadHcaHeader(reader, structure);

    switch (structure->hca->encryptionType) {
        case CriHcaKeyType::Type0:
            break;
        case CriHcaKeyType::Type1:
            structure->encryptionKey = make_shared<CriHcaKey>(CriHcaKeyType::Type1);
            break;
        case CriHcaKeyType::Type56:
            structure->encryptionKey = getEncryptionKey();
            break;
        default:
            throw std::out_of_range("unknown encryption type");
    }

    if (readAudioData) {
        reader->getBaseStream()->setPosition(structure->headerSize);

        ReadHcaData(reader, structure);
    }

    return structure;
}

shared_ptr<IAudioFormat> HcaReader::toAudioStream(const shared_ptr<HcaStructure> &structure) {
    if (structure->encryptionKey != nullptr) {
        CriHcaEncryption::decrypt(structure->hca, structure->audioData, structure->encryptionKey);
        structure->hca->encryptionType = CriHcaKeyType::Type0;
    }

    auto builder = make_shared<CriHcaFormatBuilder>(structure->audioData, structure->hca);
    return builder->build();
}

shared_ptr<HcaConfiguration> HcaReader::getConfiguration(const shared_ptr<HcaStructure> &structure) {
    auto config = make_shared<HcaConfiguration>();
    config->encryptionKey = structure->encryptionKey;
    return config;
}
