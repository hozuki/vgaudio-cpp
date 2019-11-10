#include "../utilities/endian.h"
#include "../utilities/bswap.h"
#include "BinaryWriterLE.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;

BinaryWriterLE::BinaryWriterLE(const shared_ptr<Stream> &stream)
    : MyBase(stream) {
}

#define DEFINE_WRITE_METHOD(value_type) \
    int32_t BinaryWriterLE::write(value_type value) { \
        if (machine_is_big_endian()) { \
            value = bswap(value); \
        } \
        return MyBase::write(value); \
    }

DEFINE_WRITE_METHOD(int16_t)

DEFINE_WRITE_METHOD(uint16_t)

DEFINE_WRITE_METHOD(int32_t)

DEFINE_WRITE_METHOD(uint32_t)

DEFINE_WRITE_METHOD(int64_t)

DEFINE_WRITE_METHOD(uint64_t)

DEFINE_WRITE_METHOD(float)

DEFINE_WRITE_METHOD(double)
