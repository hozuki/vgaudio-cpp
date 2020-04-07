#pragma once

#include <cstdint>

#include "../../common/utilities/runtime_array.h"

namespace vgaudio::utilities {

    struct Mdct final {

    public:

        [[nodiscard]]
        int32_t getMdctBits() const;

        [[nodiscard]]
        int32_t getMdctSize() const;

        [[nodiscard]]
        double getScale() const;

    private:

        common_lib::utilities::array_ptr<double> _mdctPrevious;
        common_lib::utilities::array_ptr<double> _imdctPrevious;
        common_lib::utilities::array_ptr<double> _imdctWindow;

        common_lib::utilities::array_ptr<double> _scratchMdct;
        common_lib::utilities::array_ptr<double> _scratchDct;

    private:

        const int32_t _mdctBits;
        const int32_t _mdctSize;
        const double _scale;

    private:

        static void SetTables(int32_t maxBits);

    public:

        Mdct(int32_t mdctBits, const common_lib::utilities::array_ptr<double> &window, double scale = 1);

        ~Mdct() = default;

    public:

        void RunMdct(const common_lib::utilities::array_ptr<double> &input, const common_lib::utilities::array_ptr<double> &output);

        void RunImdct(const common_lib::utilities::array_ptr<double> &input, const common_lib::utilities::array_ptr<double> &output);

    private:

        void Dct4(const common_lib::utilities::array_ptr<double> &input, const common_lib::utilities::array_ptr<double> &output);

        void Dct4Slow(const common_lib::utilities::array_ptr<double> &input, const common_lib::utilities::array_ptr<double> &output);

    private:

        static void GenerateTrigTables(int32_t sizeBits, common_lib::utilities::array_ptr<double> &sin, common_lib::utilities::array_ptr<double> &cos);

        static common_lib::utilities::array_ptr<int32_t> GenerateShuffleTable(int32_t sizeBits);

    };

}
