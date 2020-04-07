#include "AdxReader.h"
#include "../../codecs/criadx/CriAdxEncryption.h"
#include "../../../common/utilities/EndianHelper.h"
#include "../../../common/io/Stream.h"
#include "../../../common/io/BinaryReader.h"
#include "../../formats/IAudioFormat.h"
#include "../../formats/criadx/CriAdxChannel.h"
#include "../../formats/criadx/CriAdxFormatBuilder.h"
#include "../../../common/utilities/IntHelper.h"
#include "../../utilities/Interleave.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::containers::adx;
using namespace vgaudio::codecs::criadx;
using namespace vgaudio::formats;
using namespace vgaudio::formats::criadx;
using namespace vgaudio::utilities;

static void ReadHeader(const shared_ptr<BinaryReader> &reader, const shared_ptr<AdxStructure> &structure) {
    structure->headerSize = reader->readInt16();
    structure->encodingType = static_cast<CriAdxType>(reader->readUInt8());
    structure->frameSize = reader->readUInt8();
    structure->bitDepth = reader->readUInt8();
    structure->channelCount = reader->readUInt8();
    structure->sampleRate = reader->readInt32();
    structure->sampleCount = reader->readInt32();
    structure->highPassFrequency = reader->readInt16();
    structure->version = reader->readUInt8();
    structure->revision = reader->readUInt8();

    structure->historySamples = make_jagged_array_2_dynamic<int16_t>(structure->channelCount, 2);

    const auto stream = reader->getBaseStream();

    if (structure->version >= 4) {
        stream->setPosition(stream->getPosition() + 4);

        for (const auto &history : *structure->historySamples) {
            (*history)[0] = reader->readInt16();
            (*history)[1] = reader->readInt16();
        }

        if (structure->channelCount == 1) {
            stream->setPosition(stream->getPosition() + 4);
        }
    }

    if (stream->getPosition() + 24 > structure->headerSize) {
        return;
    }

    structure->insertedSamples = reader->readInt16();
    structure->loopCount = reader->readInt16();

    if (structure->loopCount <= 0) {
        return;
    }

    structure->looping = true;
    structure->loopType = reader->readInt32();
    structure->loopStartSample = reader->readInt32();
    structure->loopStartByte = reader->readInt32();
    structure->loopEndSample = reader->readInt32();
    structure->loopEndByte = reader->readInt32();
}

static void ReadData(const shared_ptr<BinaryReader> &reader, const shared_ptr<AdxStructure> &structure) {
    const int32_t audioOffset = structure->headerSize + 4;

    reader->getBaseStream()->setPosition(audioOffset);

    const int32_t frameCount = IntHelper::divideByRoundUp(structure->sampleCount, structure->getSamplesPerFrame());
    const int32_t audioSize = structure->frameSize * frameCount * structure->channelCount;

    const auto audioData = Interleave::deinterleave(reader->getBaseStream(), audioSize, structure->frameSize, structure->channelCount);
    structure->audioData = audioData;
}

shared_ptr<CriAdxKey> AdxReader::getEncryptionKey() const {
    return _encryptionKey;
}

void AdxReader::setEncryptionKey(const shared_ptr<CriAdxKey> &encryptionKey) {
    _encryptionKey = encryptionKey;
}

shared_ptr<AdxStructure> AdxReader::readFile(const shared_ptr<Stream> &stream, bool readAudioData) {
    auto reader = EndianHelper::createBinaryReader(stream, Endianess::BigEndian);

    const auto signature = reader->readUInt16();

    if (signature != 0x8000) {
        throw std::runtime_error("File does not have ADX signature (0x80 0x00).");
    }

    auto structure = make_shared<AdxStructure>();

    ReadHeader(reader, structure);

    if (readAudioData) {
        reader->getBaseStream()->setPosition(structure->headerSize + 4);
        ReadData(reader, structure);
    }

    structure->encryptionKey = getEncryptionKey();

    return structure;
}

shared_ptr<IAudioFormat> AdxReader::toAudioStream(const shared_ptr<AdxStructure> &structure) {
    if (structure->encryptionKey != nullptr) {
        CriAdxEncryption::encryptDecrypt(structure->audioData, structure->encryptionKey, structure->revision, structure->frameSize);
    }

    const auto channels = make_array_dynamic<shared_ptr<CriAdxChannel>>(structure->channelCount);

    for (auto i = 0; i < structure->channelCount; i += 1) {
        const auto channel = make_shared<CriAdxChannel>((*structure->audioData)[i], (*(*structure->historySamples)[i])[0], structure->version);
        (*channels)[i] = channel;
    }

    const auto builder = make_shared<CriAdxFormatBuilder>(channels, structure->sampleCount - structure->insertedSamples, structure->sampleRate, structure->frameSize, structure->highPassFrequency);
    builder->withLoop(structure->looping, structure->loopStartSample - structure->insertedSamples, structure->loopEndSample - structure->insertedSamples);
    builder->setAlignmentSamples(structure->insertedSamples);
    builder->setType(structure->encodingType);

    return builder->build();
}

shared_ptr<AdxConfiguration> AdxReader::getConfiguration(const shared_ptr<AdxStructure> &structure) {
    const auto configuration = make_shared<AdxConfiguration>();
    configuration->frameSize = structure->frameSize;
    configuration->type = structure->encodingType;
    configuration->encryptionType = structure->revision;
    configuration->encryptionKey = structure->encryptionKey;

    return configuration;
}
