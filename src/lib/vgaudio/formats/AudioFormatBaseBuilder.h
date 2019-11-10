#pragma once

#include <cstdint>
#include <list>
#include <memory>

#include "../../common/utilities/type_sys.h"

namespace vgaudio::formats {

    struct IAudioFormat;
    struct AudioTrack;

    using namespace std;

    struct AudioFormatBaseBuilder {

        DECLARE_ROOT_CLASS(AudioFormatBaseBuilder);

    private:

        bool _looping;
        int32_t _loopStart;
        int32_t _loopEnd;
        int32_t _sampleCount;
        int32_t _sampleRate;
        shared_ptr<list<shared_ptr<AudioTrack>>> _tracks;

    protected:

        AudioFormatBaseBuilder();

    public:

        virtual ~AudioFormatBaseBuilder() = default;

        [[nodiscard]]
        virtual int32_t getChannelCount() const = 0;

        [[nodiscard]]
        decltype(_looping) isLooping() const;

        void setLooping(decltype(_looping) looping);

        [[nodiscard]]
        decltype(_loopStart) getLoopStart() const;

        void setLoopStart(decltype(_loopStart) start);

        [[nodiscard]]
        decltype(_loopEnd) getLoopEnd() const;

        void setLoopEnd(decltype(_loopEnd) end);

        [[nodiscard]]
        decltype(_sampleCount) getSampleCount() const;

        void setSampleCount(decltype(_sampleCount) count);

        [[nodiscard]]
        decltype(_sampleRate) getSampleRate() const;

        void setSampleRate(decltype(_sampleRate) sampleRate);

        [[nodiscard]]
        decltype(_tracks) getTracks() const;

        void setTracks(const shared_ptr<list<shared_ptr<AudioTrack>>> &tracks);

        virtual shared_ptr<IAudioFormat> build() = 0;

        virtual void withLoop(bool loop);

        virtual void withLoop(bool loop, int32_t loopStart, int32_t loopEnd);

        virtual void withTracks(const shared_ptr<list<shared_ptr<AudioTrack>>> &tracks);

    };

}
