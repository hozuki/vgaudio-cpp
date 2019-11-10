#pragma once

#include <cstdint>
#include <memory>

#include "../../common/utilities/runtime_array.h"

namespace vgaudio::utilities {

    using namespace std;
    using namespace common_lib::utilities;

    struct Mdct final {

    public:

        [[nodiscard]]
        int32_t getMdctBits() const;

        [[nodiscard]]
        int32_t getMdctSize() const;

        [[nodiscard]]
        double getScale() const;

    private:

        array_ptr<double> _mdctPrevious;
        array_ptr<double> _imdctPrevious;
        array_ptr<double> _imdctWindow;

        array_ptr<double> _scratchMdct;
        array_ptr<double> _scratchDct;

    private:

        const int32_t _mdctBits;
        const int32_t _mdctSize;
        const double _scale;

    private:

        static void SetTables(int32_t maxBits);

    public:

        Mdct(int32_t mdctBits, const array_ptr<double> &window, double scale = 1);

        ~Mdct() = default;

    public:

        void RunMdct(const array_ptr<double> &input, const array_ptr<double> &output);

        void RunImdct(const array_ptr<double> &input, const array_ptr<double> &output);

    private:

        void Dct4(const array_ptr<double> &input, const array_ptr<double> &output);

        void Dct4Slow(const array_ptr<double> &input, const array_ptr<double> &output);

    private:

        static void GenerateTrigTables(int32_t sizeBits, array_ptr<double> &sin, array_ptr<double> &cos);

        static array_ptr<int32_t> GenerateShuffleTable(int32_t sizeBits);

    };

}
