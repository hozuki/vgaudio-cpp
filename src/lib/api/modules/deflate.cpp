#include <cstdint>
#include <memory>

#include "../../common/utilities/runtime_array.h"
#include "../../common/io/MemoryStream.h"
#include "../../common/io/compression/DeflateStream.h"
#include "../../../api/modules/deflate.h"
#include "../../../api/modules/vga_byte_buffer.h"
#include "vga_byte_buffer_impl.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace common_lib::io::compression;

constexpr size_t DefaultDeflateBufferSize = 4096;

VGA_API_IMPL(void) vgaUtilDeflateData(const void *data, size_t dataSize, size_t bufferSize, vga_byte_buffer *output, size_t *outputSize) {
    if (output == nullptr) {
        return;
    }

    if (bufferSize == 0) {
        bufferSize = DefaultDeflateBufferSize;
    }

    auto inBuffer = make_array_dynamic<uint8_t>(static_cast<const uint8_t *>(data), dataSize, false, false);
    auto inMemory = make_shared<MemoryStream>(inBuffer, false);

    auto outMemory = make_shared<MemoryStream>();

    {
        auto deflate = make_shared<DeflateStream>(outMemory, CompressionMode::Compress);
        inMemory->copyTo(deflate, bufferSize);
    }

    auto outBuffer = outMemory->toArray();

    vgaUtilAllocByteBuffer(output, outBuffer->size());

    if (outputSize != nullptr) {
        *outputSize = outBuffer->size();
    }
    if (!outBuffer->empty()) {
        memcpy(vgaUtilGetByteBufferData(output), outBuffer->data(), outBuffer->size());
    }
}

VGA_API_IMPL(void) vgaUtilInflateData(const void *data, size_t dataSize, size_t bufferSize, vga_byte_buffer *output, size_t *outputSize) {
    if (output == nullptr) {
        return;
    }

    if (bufferSize == 0) {
        bufferSize = DefaultDeflateBufferSize;
    }

    auto inBuffer = make_array_dynamic<uint8_t>(static_cast<const uint8_t *>(data), dataSize, false, false);
    auto inMemory = make_shared<MemoryStream>(inBuffer, false);

    auto outMemory = make_shared<MemoryStream>();

    {
        auto deflate = make_shared<DeflateStream>(inMemory, CompressionMode::Decompress);
        deflate->copyTo(outMemory, bufferSize);
    }

    auto outBuffer = outMemory->toArray();

    vgaUtilAllocByteBuffer(output, outBuffer->size());

    if (outputSize != nullptr) {
        *outputSize = outBuffer->size();
    }
    if (!outBuffer->empty()) {
        memcpy(vgaUtilGetByteBufferData(output), outBuffer->data(), outBuffer->size());
    }
}
