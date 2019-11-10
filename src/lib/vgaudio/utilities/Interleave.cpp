#include <cassert>

#include "../../common/io/Stream.h"
#include "Interleave.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::utilities;

array_ptr<uint8_t> Interleave::shortToInterleavedByte(const jarray2_ptr<int16_t> &input) {
    const auto inputCount = input->size();
    assert(inputCount > 0);

    const auto length = (*input)[0]->size();

    auto output = make_array_dynamic<uint8_t>(inputCount * length * 2);

    for (auto i = 0; i < length; i += 1) {
        for (auto j = 0; j < inputCount; j += 1) {
            const auto offset = (i * inputCount + j) * 2;
            (*output)[offset] = static_cast<uint8_t>((*(*input)[j])[i] & 0xffu);
            (*output)[offset + 1] = static_cast<uint8_t>(((*(*input)[j])[i] >> 8) & 0xffu);
        }
    }

    return output;
}

jarray2_ptr<int16_t> Interleave::interleavedByteToShort(const array_ptr<uint8_t> &input, int32_t outputCount) {
    assert(outputCount > 0);
    assert(input->size() % (2 * outputCount) == 0);

    const auto itemCount = input->size() / 2 / outputCount;
    auto output = make_jagged_array_2_dynamic<int16_t>(outputCount, itemCount);

    for (auto i = 0; i < itemCount; i += 1) {
        for (auto o = 0; o < outputCount; o += 1) {
            const auto offset = (i * outputCount + o) * 2;
            (*(*output)[o])[i] = static_cast<int16_t>(static_cast<int32_t>((*input)[offset]) | (static_cast<int32_t>((*input)[offset + 1]) << 8));
        }
    }

    return output;
}

void Interleave::interleave(const jarray2_ptr<uint8_t> &inputs, const shared_ptr<Stream> &output, int32_t interleaveSize, int32_t outputSize) {
    const int32_t inputSize = (*inputs)[0]->size();

    if (outputSize == -1) {
        outputSize = inputSize;
    }

    for (const auto &input : *inputs) {
        if (input->size() != inputSize) {
            throw std::runtime_error("all input sizes must match");
        }
    }

    const auto inputCount = inputs->size();
    const auto inBlockCount = IntHelper::divideByRoundUp(inputSize, interleaveSize);
    const auto outBlockCount = IntHelper::divideByRoundUp(outputSize, interleaveSize);
    const auto lastInputInterleaveSize = inputSize - (inBlockCount - 1) * interleaveSize;
    const auto lastOutputInterleaveSize = outputSize - (outBlockCount - 1) * interleaveSize;
    const auto blocksToCopy = std::min(inBlockCount, outBlockCount);

    for (auto b = 0; b < blocksToCopy; b += 1) {
        const auto currentInputInterleaveSize = b == inBlockCount - 1 ? lastInputInterleaveSize : interleaveSize;
        const auto currentOutputInterleaveSize = b == outBlockCount - 1 ? lastOutputInterleaveSize : interleaveSize;
        const auto bytesToCopy = std::min(currentInputInterleaveSize, currentOutputInterleaveSize);

        for (auto i = 0; i < inputCount; i += 1) {
            output->write((*inputs)[i], interleaveSize * b, bytesToCopy);

            if (bytesToCopy < currentOutputInterleaveSize) {
                output->setPosition(output->getPosition() + (currentOutputInterleaveSize - bytesToCopy));
            }
        }
    }
}
