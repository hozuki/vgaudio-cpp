#pragma once

#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

#include "../../common/utilities/static_class.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/runtime_jagged_array.h"
#include "../../common/utilities/IntHelper.h"
#include "../../common/utilities/ArrayHelper.h"

namespace common_lib::io {
    struct Stream;
}

namespace vgaudio::utilities {
    struct Interleave {

        IMPLEMENT_STATIC_CLASS(Interleave);

    public:

        template<typename T, typename U = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type>
        static common_lib::utilities::jarray2_ptr<U> deinterleave(const common_lib::utilities::array_ptr<T> &input, int32_t interleaveSize, int32_t outputCount, int32_t outputSize = -1) {
            if (input->size() % outputCount != 0) {
                throw std::invalid_argument("input array length must be divisible by number of outputs");
            }

            const int32_t inputSize = input->size() / outputCount;

            if (outputSize < 0) {
                outputSize = inputSize;
            }

            const auto inBlockCount = common_lib::utilities::IntHelper::divideByRoundUp(inputSize, interleaveSize);
            const auto outBlockCount = common_lib::utilities::IntHelper::divideByRoundUp(outputSize, interleaveSize);
            const auto lastInputInterleaveSize = inputSize - (inBlockCount - 1) * interleaveSize;
            const auto lastOutputInterleaveSize = outputSize - (outBlockCount - 1) * interleaveSize;
            const auto blocksToCopy = std::min(inBlockCount, outBlockCount);

            auto outputs = common_lib::utilities::make_jagged_array_2_dynamic<U>(outputCount, outputSize);

            for (auto b = 0; b < blocksToCopy; b += 1) {
                const auto currentInputInterleaveSize = b == inBlockCount - 1 ? lastInputInterleaveSize : interleaveSize;
                const auto currentOutputInterleaveSize = b == outBlockCount - 1 ? lastOutputInterleaveSize : interleaveSize;
                const auto elementsToCopy = std::min(currentInputInterleaveSize, currentOutputInterleaveSize);

                for (auto o = 0; o < outputCount; o += 1) {
                    common_lib::utilities::ArrayHelper::copyByValue(input, interleaveSize * b * outputCount + currentInputInterleaveSize * o, (*outputs)[o], interleaveSize * b, elementsToCopy);
                }
            }

            return outputs;
        }

        static common_lib::utilities::array_ptr<uint8_t> shortToInterleavedByte(const common_lib::utilities::jarray2_ptr<int16_t> &input);

        static common_lib::utilities::jarray2_ptr<int16_t> interleavedByteToShort(const common_lib::utilities::array_ptr<uint8_t> &input, int32_t outputCount);

        static void interleave(const common_lib::utilities::jarray2_ptr<uint8_t> &inputs, const std::shared_ptr<common_lib::io::Stream> &output, int32_t interleaveSize, int32_t outputSize = -1);

        static common_lib::utilities::jarray2_ptr<uint8_t> deinterleave(const std::shared_ptr<common_lib::io::Stream> &input, int32_t length, int32_t interleaveSize, int32_t outputCount, int32_t outputSize = -1);

    };

}
