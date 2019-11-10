#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <cinttypes>
#include <mutex>
#include <vector>

#include "../../thread_fix.h"
#include "../../common/utilities/BitHelper.h"
#include "../../common/utilities/runtime_array.h"
#include "../../common/utilities/ArrayHelper.h"
#include "Mdct.h"

using namespace std;
using namespace vgaudio::utilities;
using namespace common_lib::utilities;

static struct _MdctGlobals {
    mutex tableLock;
    int32_t tableBits;
    vector<array_ptr<double>> sinTables;
    vector<array_ptr<double>> cosTables;
    vector<array_ptr<int32_t>> shuffleTables;

    _MdctGlobals() noexcept
        : tableBits(-1) {
    }

} mdctGlobals;

void Mdct::Dct4Slow(const array_ptr<double> &input, const array_ptr<double> &output) {
    const auto mdctSize = getMdctSize();
    const auto scale = getScale();

    for (auto k = 0; k < mdctSize; k += 1) {
        double sample = 0;

        for (auto n = 0; n < mdctSize; n += 1) {
            double angle = M_PI / mdctSize * (k + 0.5) * (n + 0.5);
            sample += std::cos(angle) * (*input)[n];
        }

        (*output)[k] = sample * scale;
    }
}

int32_t Mdct::getMdctBits() const {
    return _mdctBits;
}

int32_t Mdct::getMdctSize() const {
    return _mdctSize;
}

double Mdct::getScale() const {
    return _scale;
}

void Mdct::GenerateTrigTables(int32_t sizeBits, array_ptr<double> &sin, array_ptr<double> &cos) {
    const int32_t size = 1 << sizeBits;

    sin = make_array_dynamic<double>(size);
    cos = make_array_dynamic<double>(size);

    for (auto i = 0; i < size; i += 1) {
        double value = M_PI * (4 * i + 1) / (4 * size);
        (*sin)[i] = std::sin(value);
        (*cos)[i] = std::cos(value);
    }
}

array_ptr<int32_t> Mdct::GenerateShuffleTable(int32_t sizeBits) {
    const int32_t size = 1 << sizeBits;

    auto table = make_array_dynamic<int32_t>(size);

    for (auto i = 0; i < size; i += 1) {
        const auto v = BitHelper::bitReverse32(i ^ (i / 2), sizeBits);
        (*table)[i] = v;
    }

    return table;
}

void Mdct::SetTables(int32_t maxBits) {
    lock_guard<mutex> lock(mdctGlobals.tableLock);

    if (maxBits > mdctGlobals.tableBits) {
        for (auto i = mdctGlobals.tableBits + 1; i <= maxBits; i += 1) {
            array_ptr<double> sin, cos;

            GenerateTrigTables(i, sin, cos);

            mdctGlobals.sinTables.push_back(sin);
            mdctGlobals.cosTables.push_back(cos);

            auto shuffleTable = GenerateShuffleTable(i);
            mdctGlobals.shuffleTables.push_back(shuffleTable);
        }

        mdctGlobals.tableBits = maxBits;
    }
}

void Mdct::Dct4(const array_ptr<double> &input, const array_ptr<double> &output) {
    const auto mdctBits = getMdctBits();
    const auto mdctSize = getMdctSize();

    const auto &dctTemp = _scratchDct;

    {
        const auto &sinTable = mdctGlobals.sinTables[mdctBits];
        const auto &cosTable = mdctGlobals.cosTables[mdctBits];

        const auto lastIndex = mdctSize - 1;
        const auto halfSize = mdctSize / 2;

        for (auto i = 0; i < halfSize; i += 1) {
            const auto i2 = i * 2;

            const double a = (*input)[i2];
            const double b = (*input)[lastIndex - i2];
            const double sin = (*sinTable)[i];
            const double cos = (*cosTable)[i];

            (*dctTemp)[i2] = a * cos + b * sin;
            (*dctTemp)[i2 + 1] = a * sin - b * cos;
        }
    }

    const auto stageCount = mdctBits - 1;

    for (auto stage = 0; stage < stageCount; stage += 1) {
        const auto blockCount = 1 << stage;
        const auto blockSizeBits = stageCount - stage;
        const auto blockHalfSizeBits = blockSizeBits - 1;
        const auto blockSize = 1 << blockSizeBits;
        const auto blockHalfSize = 1 << blockHalfSizeBits;

        const auto &sinTable = mdctGlobals.sinTables[blockHalfSizeBits];
        const auto &cosTable = mdctGlobals.cosTables[blockHalfSizeBits];

        for (auto block = 0; block < blockCount; block += 1) {
            for (auto i = 0; i < blockHalfSize; i += 1) {
                const auto frontPos = (block * blockSize + i) * 2;
                const auto backPos = frontPos + blockSize;

                const double a = (*dctTemp)[frontPos] - (*dctTemp)[backPos];
                const double b = (*dctTemp)[frontPos + 1] - (*dctTemp)[backPos + 1];
                const double sin = (*sinTable)[i];
                const double cos = (*cosTable)[i];

                (*dctTemp)[frontPos] += (*dctTemp)[backPos];
                (*dctTemp)[frontPos + 1] += (*dctTemp)[backPos + 1];
                (*dctTemp)[backPos] = a * cos + b * sin;
                (*dctTemp)[backPos + 1] = a * sin - b * cos;
            }
        }
    }

    const auto &shuffleTable = mdctGlobals.shuffleTables[mdctBits];
    const auto scale = getScale();

    for (auto i = 0; i < mdctSize; i += 1) {
        const auto shuffledIndex = (*shuffleTable)[i];
        (*output)[i] = (*dctTemp)[shuffledIndex] * scale;
    }
}

void Mdct::RunMdct(const array_ptr<double> &input, const array_ptr<double> &output) {
    const auto mdctSize = getMdctSize();

    if (input->size() < mdctSize) {
        throw std::invalid_argument(StrHelper::format("input (%zu) must be at least as long as MDCT size (%" PRId32 ")", input->size(), mdctSize));
    }

    if (output->size() < mdctSize) {
        throw std::invalid_argument(StrHelper::format("output (%zu) must be at least as long as MDCT size (%" PRId32 ")", output->size(), mdctSize));
    }

    const auto half = mdctSize / 2;
    const auto &dctIn = _scratchMdct;
    const auto &imdctWindow = _imdctWindow;
    const auto &mdctPrevious = _mdctPrevious;

    for (auto i = 0; i < half; i += 1) {
        const double a = (*imdctWindow)[half - i - 1] * -(*input)[half + i];
        const double b = (*imdctWindow)[half + i] * (*input)[half - i - 1];
        const double c = (*imdctWindow)[i] * (*mdctPrevious)[i];
        const double d = (*imdctWindow)[mdctSize - i - 1] * (*mdctPrevious)[mdctSize - i - 1];

        (*dctIn)[i] = a - b;
        (*dctIn)[half + i] = c - d;
    }

    Dct4(dctIn, output);

    ArrayHelper::copyByValue(input, mdctPrevious, input->size());
}

void Mdct::RunImdct(const array_ptr<double> &input, const array_ptr<double> &output) {
    const auto mdctSize = getMdctSize();

    if (input->size() < mdctSize) {
        throw std::invalid_argument(StrHelper::format("input (%zu) must be at least as long as MDCT size (%" PRId32 ")", input->size(), mdctSize));
    }

    if (output->size() < mdctSize) {
        throw std::invalid_argument(StrHelper::format("output (%zu) must be at least as long as MDCT size (%" PRId32 ")", output->size(), mdctSize));
    }

    const auto half = mdctSize / 2;
    const auto &dctOut = _scratchMdct;

    Dct4(input, dctOut);

    const auto &imdctWindow = _imdctWindow;
    const auto &imdctPrevious = _imdctPrevious;

    for (auto i = 0; i < half; i += 1) {
        (*output)[i] = (*imdctWindow)[i] * (*dctOut)[i + half] + (*imdctPrevious)[i];
        (*output)[i + half] = (*imdctWindow)[i + half] * -(*dctOut)[mdctSize - i - 1] - (*imdctPrevious)[i + half];
        (*imdctPrevious)[i] = (*imdctWindow)[mdctSize - i - 1] * -(*dctOut)[half - i - 1];
        (*imdctPrevious)[i + half] = (*imdctWindow)[half - i - 1] * (*dctOut)[i];
    }
}

Mdct::Mdct(int32_t mdctBits, const array_ptr<double> &window, double scale)
    : _mdctBits(mdctBits), _mdctSize(1 << mdctBits), _scale(scale) {
    SetTables(mdctBits);

    const auto mdctSize = getMdctSize();

    if (window->size() < mdctSize) {
        throw std::invalid_argument(StrHelper::format("window (%zu) must be at least as long as MDCT size (%" PRId32 ")", window->size(), mdctSize));
    }

    _mdctPrevious = make_array_dynamic<double>(mdctSize);
    _imdctPrevious = make_array_dynamic<double>(mdctSize);
    _scratchMdct = make_array_dynamic<double>(mdctSize);
    _scratchDct = make_array_dynamic<double>(mdctSize);
    _imdctWindow = window;
}
