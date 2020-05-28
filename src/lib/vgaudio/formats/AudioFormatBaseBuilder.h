#pragma once

#include <cstdint>
#include <list>
#include <memory>

#include "../../common/utilities/_cxx17.h"
#include "../../common/utilities/type_sys.h"

namespace vgaudio {
    namespace formats {

        struct IAudioFormat;
        struct AudioTrack;

        struct AudioFormatBaseBuilder {

            DECLARE_ROOT_CLASS(AudioFormatBaseBuilder);

        private:

            bool _looping;
            int32_t _loopStart;
            int32_t _loopEnd;
            int32_t _sampleCount;
            int32_t _sampleRate;
            std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> _tracks;

        protected:

            AudioFormatBaseBuilder();

        public:

            virtual ~AudioFormatBaseBuilder() = default;

            _CXX17_ATTR_NODISCARD
            virtual int32_t getChannelCount() const = 0;

            _CXX17_ATTR_NODISCARD
            decltype(_looping) isLooping() const;

            void setLooping(decltype(_looping) looping);

            _CXX17_ATTR_NODISCARD
            decltype(_loopStart) getLoopStart() const;

            void setLoopStart(decltype(_loopStart) start);

            _CXX17_ATTR_NODISCARD
            decltype(_loopEnd) getLoopEnd() const;

            void setLoopEnd(decltype(_loopEnd) end);

            _CXX17_ATTR_NODISCARD
            decltype(_sampleCount) getSampleCount() const;

            void setSampleCount(decltype(_sampleCount) sampleCount);

            _CXX17_ATTR_NODISCARD
            decltype(_sampleRate) getSampleRate() const;

            void setSampleRate(decltype(_sampleRate) sampleRate);

            _CXX17_ATTR_NODISCARD
            decltype(_tracks) getTracks() const;

            void setTracks(const std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> &tracks);

            virtual std::shared_ptr<IAudioFormat> build() = 0;

            virtual void withLoop(bool loop);

            virtual void withLoop(bool loop, int32_t loopStart, int32_t loopEnd);

            virtual void withTracks(const std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> &tracks);

        };

    }
}
