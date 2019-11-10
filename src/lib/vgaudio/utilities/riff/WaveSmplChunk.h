#pragma once

#include "RiffSubChunk.h"

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::utilities;
    using namespace common_lib::io;

    struct RiffParser;

    struct WaveSmplChunk : public RiffSubChunk {

        DECLARE_CLASS(WaveSmplChunk, RiffSubChunk);

    public:

        struct SampleLoop final {

            SampleLoop();

            ~SampleLoop() = default;

            int32_t cuePointId;
            int32_t type;
            int32_t start;
            int32_t end;
            int32_t fraction;
            int32_t playCount;

        };

    private:

        int32_t _manufacturer;
        int32_t _product;
        int32_t _samplePeriod;
        int32_t _midiUnityNote;
        int32_t _midiPitchFraction;
        int32_t _smpteFormat;
        int32_t _smpteOffset;
        int32_t _samplerData;
        array_ptr<SampleLoop> _loops;

    public:

        ~WaveSmplChunk() override = default;

        static shared_ptr<WaveSmplChunk> parse(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

        [[nodiscard]]
        int32_t getManufacturer() const;

        void setManufacturer(int32_t manufacturer);

        [[nodiscard]]
        int32_t getProduct() const;

        void setProduct(int32_t product);

        [[nodiscard]]
        int32_t getSamplePeriod() const;

        void setSamplePeriod(int32_t period);

        [[nodiscard]]
        int32_t getMidiUnityNote() const;

        void setMidiUnityNote(int32_t note);

        [[nodiscard]]
        int32_t getMidiPitchFraction() const;

        void setMidiPitchFraction(int32_t fraction);

        [[nodiscard]]
        int32_t getSmpteFormat() const;

        void setSmpteFormat(int32_t format);

        [[nodiscard]]
        int32_t getSmpteOffset() const;

        void setSmpteOffset(int32_t offset);

        [[nodiscard]]
        int32_t getSampleLoopCount() const;

        [[nodiscard]]
        int32_t getSamplerData() const;

        void setSamplerData(int32_t data);

        [[nodiscard]]
        array_ptr<SampleLoop> getLoops() const;

        void setLoops(const array_ptr<SampleLoop> &loops);

    protected:

        WaveSmplChunk(const shared_ptr<RiffParser> &parser, const shared_ptr<BinaryReader> &reader);

    };

}
