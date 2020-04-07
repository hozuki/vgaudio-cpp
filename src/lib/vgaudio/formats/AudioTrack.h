#pragma once

#include <cstdint>
#include <list>
#include <memory>

namespace vgaudio::formats {

    struct AudioTrack {

    private:

        int32_t _channelCount;
        int32_t _channelLeft;
        int32_t _channelRight;
        int32_t _panning;
        int32_t _volume;
        int32_t _surroundPanning;
        int32_t _flags;

    public:

        AudioTrack();

        AudioTrack(int32_t channelCount, int32_t channelLeft, int32_t channelRight, int32_t panning, int32_t volume);

        AudioTrack(int32_t channelCount, int32_t channelLeft, int32_t channelRight);

        [[nodiscard]]
        int32_t getChannelCount() const;

        void setChannelCount(int32_t count);

        [[nodiscard]]
        int32_t getChannelLeft() const;

        void setChannelLeft(int32_t count);

        [[nodiscard]]
        int32_t getChannelRight() const;

        void setChannelRight(int32_t count);

        [[nodiscard]]
        int32_t getPanning() const;

        void setPanning(int32_t panning);

        [[nodiscard]]
        int32_t getVolume() const;

        void setVolume(int32_t volume);

        [[nodiscard]]
        int32_t getSurroundPanning() const;

        void setSurroundPanning(int32_t panning);

        [[nodiscard]]
        int32_t getFlags() const;

        void setFlags(int32_t flags);

        static std::shared_ptr<std::list<std::shared_ptr<AudioTrack>>> getDefaultTrackList(int32_t channelCount);

    };

}
