#pragma once

#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

#include "../../common/utilities/static_class.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/runtime_jagged_array_2.h"
#include "../../common/utilities/IntHelper.h"
#include "../../common/utilities/ArrayHelper.h"

namespace common_lib::io {
    struct Stream;
}

namespace vgaudio::utilities {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct Interleave {

        IMPLEMENT_STATIC_CLASS(Interleave);

    public:

        template<typename T, typename U = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type>
        static jarray2_ptr<U> deinterleave(const array_ptr<T> &input, int32_t interleaveSize, int32_t outputCount, int32_t outputSize = -1) {
            if (input->size() % outputCount != 0) {
                throw std::invalid_argument("input array length must be divisible by number of outputs");
            }

            const int32_t inputSize = input->size() / outputCount;

            if (outputSize < 0) {
                outputSize = inputSize;
            }

            const auto inBlockCount = IntHelper::divideByRoundUp(inputSize, interleaveSize);
            const auto outBlockCount = IntHelper::divideByRoundUp(outputSize, interleaveSize);
            const auto lastInputInterleaveSize = inputSize - (inBlockCount - 1) * interleaveSize;
            const auto lastOutputInterleaveSize = outputSize - (outBlockCount - 1) * interleaveSize;
            const auto blocksToCopy = std::min(inBlockCount, outBlockCount);

            auto outputs = make_jagged_array_2_dynamic<U>(outputCount, outputSize);

            for (auto b = 0; b < blocksToCopy; b += 1) {
                const auto currentInputInterleaveSize = b == inBlockCount - 1 ? lastInputInterleaveSize : interleaveSize;
                const auto currentOutputInterleaveSize = b == outBlockCount - 1 ? lastOutputInterleaveSize : interleaveSize;
                const auto elementsToCopy = std::min(currentInputInterleaveSize, currentOutputInterleaveSize);

                for (auto o = 0; o < outputCount; o += 1) {
                    ArrayHelper::copyByValue(input, interleaveSize * b * outputCount + currentInputInterleaveSize * o, (*outputs)[o], interleaveSize * b, elementsToCopy);
                }
            }

            return outputs;
        }

        static array_ptr<uint8_t> shortToInterleavedByte(const jarray2_ptr<int16_t> &input);

        static jarray2_ptr<int16_t> interleavedByteToShort(const array_ptr<uint8_t> &input, int32_t outputCount);

        static void interleave(const jarray2_ptr<uint8_t> &inputs, const shared_ptr<Stream> &output, int32_t interleaveSize, int32_t outputSize = -1);

    };

}
