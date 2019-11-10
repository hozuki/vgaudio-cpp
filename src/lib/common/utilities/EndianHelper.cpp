#include "../io/Stream.h"
#include "../io/BinaryWriter.h"
#include "../io/BinaryReader.h"
#include "../io/BinaryReaderBE.h"
#include "../io/BinaryReaderLE.h"
#include "../io/BinaryWriterBE.h"
#include "../io/BinaryWriterLE.h"

#include "EndianHelper.h"

using namespace std;
using namespace common_lib::io;
using namespace common_lib::utilities;

shared_ptr<BinaryReader> EndianHelper::createBinaryReader(const shared_ptr<Stream> &stream, Endianess endian) {
    switch (endian) {
        case Endianess::BigEndian:
            return make_shared<BinaryReaderBE>(stream);
        case Endianess::LittleEndian:
            return make_shared<BinaryReaderLE>(stream);
        default:
            throw std::out_of_range("invalid enum value");
    }
}

shared_ptr<BinaryWriter> EndianHelper::createBinaryWriter(const shared_ptr<Stream> &stream, Endianess endian) {
    switch (endian) {
        case Endianess::BigEndian:
            return make_shared<BinaryWriterBE>(stream);
        case Endianess::LittleEndian:
            return make_shared<BinaryWriterLE>(stream);
        default:
            throw std::out_of_range("invalid enum value");
    }
}
