#include "../utilities/endian.h"
#include "../utilities/bswap.h"
#include "BinaryReaderLE.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

BinaryReaderLE::BinaryReaderLE(const shared_ptr<Stream> &stream)
    : MyBase(stream) {
}

#define DEFINE_READ_METHOD(value_type, type_name) \
    value_type BinaryReaderLE::read##type_name() { \
        auto value = MyBase::read##type_name(); \
        if (machine_is_big_endian()) { \
            value = bswap(value); \
        } \
        return value; \
    }

DEFINE_READ_METHOD(int16_t, Int16)

DEFINE_READ_METHOD(uint16_t, UInt16)

DEFINE_READ_METHOD(int32_t, Int32)

DEFINE_READ_METHOD(uint32_t, UInt32)

DEFINE_READ_METHOD(int64_t, Int64)

DEFINE_READ_METHOD(uint64_t, UInt64)

DEFINE_READ_METHOD(float, Single)

DEFINE_READ_METHOD(double, Double)
