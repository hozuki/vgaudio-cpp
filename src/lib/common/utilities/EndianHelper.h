#pragma once

#include <memory>

#include "endian.h"
#include "static_class.h"

namespace common_lib::io {
    struct Stream;
    struct BinaryReader;
    struct BinaryWriter;
}

namespace common_lib::utilities {

    using namespace std;
    using namespace common_lib::io;

    struct EndianHelper {

        IMPLEMENT_STATIC_CLASS(EndianHelper);

        static shared_ptr<BinaryReader> createBinaryReader(const shared_ptr<Stream> &stream, Endianess endian);

        static shared_ptr<BinaryWriter> createBinaryWriter(const shared_ptr<Stream> &stream, Endianess endian);

    };

}
