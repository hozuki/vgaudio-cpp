#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstdint>

#include "../../src/api/vgaudio.h"
#include "../../src/lib/common/utilities/runtime_array.h"
#include "../../src/lib/common/utilities/defer.h"

using namespace std;
using namespace common_lib::utilities;

void testDeflate();

void inspect(const char *name, const array_ptr<uint8_t> &bin);

int main() {
    testDeflate();
    return 0;
}

void testDeflate() {
    size_t deflateOutSize = 0;
    auto deflateOut = vgaUtilCreateByteBuffer();
    size_t inflateOutSize = 0;
    auto inflateOut = vgaUtilCreateByteBuffer();

    auto_defer([&deflateOut, &inflateOut] {
        vgaUtilDestroyByteBuffer(&deflateOut);
        vgaUtilDestroyByteBuffer(&inflateOut);
    });

    char src[] = "hello, world!";

    vgaUtilDeflateData(src, sizeof(src), 0, deflateOut, &deflateOutSize); // .NET does not have zlib footer (4 bytes ADLER32)

    {
        auto deflateArray = make_array_dynamic<uint8_t>(static_cast<const uint8_t *>(vgaUtilGetByteBufferData(deflateOut)), deflateOutSize, false, false);
        inspect("deflated data", deflateArray);
    }

    vgaUtilInflateData(vgaUtilGetByteBufferData(deflateOut), deflateOutSize, 8, inflateOut, &inflateOutSize);

    printf("Original size: %zu\n", sizeof(src));

    char ss[sizeof(src)] = {0};
    memcpy(ss, vgaUtilGetByteBufferData(inflateOut), sizeof(ss));
    ss[sizeof(ss) - 1] = '\0';
    printf("%s\n", ss);

    if (strcmp(src, static_cast<const char *>(vgaUtilGetByteBufferData(inflateOut))) == 0) {
        cout << "Deflate passed";
    } else {
        cout << "Deflate failed";
    }

    cout << endl;
}

void inspect(const char *name, const array_ptr<uint8_t> &bin) {
    printf("Data of %s:", name);

    for (auto b : *bin) {
        int high = b >> 4;
        int low = b & 0xf;

        if (0 <= high && high <= 9) {
            high = high + '0';
        } else {
            high = high - 10 + 'a';
        }

        if (0 <= low && low <= 9) {
            low = low + '0';
        } else {
            low = low - 10 + 'a';
        }

        printf(" 0x%c%c,", high, low);
    }

    printf(" (length %zu)", bin->size());

    printf("\n");
}
