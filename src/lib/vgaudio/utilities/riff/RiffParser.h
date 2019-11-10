#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <list>
#include <type_traits>

#include "../../../common/utilities/type_sys.h"

namespace common_lib::io {
    struct BinaryReader;
    struct Stream;
}

namespace vgaudio::utilities::riff {

    using namespace std;
    using namespace common_lib::io;

    struct RiffChunk;
    struct RiffSubChunk;
    struct WaveFormatExtensible;

    struct RiffParser final {

        DECLARE_ROOT_CLASS(RiffParser);

    private:

        shared_ptr<RiffChunk> _riffChunk;
        bool _readDataChunk; // = true
        unordered_map<string, shared_ptr<RiffSubChunk>> _subChunks;

        unordered_map<string, function<shared_ptr<RiffSubChunk>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)>> _registeredSubChunks;

        function<shared_ptr<WaveFormatExtensible>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> _formatExtensibleParser;

    public:

        RiffParser();

        ~RiffParser() = default;

        void registerSubChunk(const string &id, const function<shared_ptr<RiffSubChunk>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> &subChunkReader);

        void parseRiff(const shared_ptr<Stream> &file);

        [[nodiscard]]
        vector<shared_ptr<RiffSubChunk>> getAllSubChunks() const;

        shared_ptr<RiffSubChunk> getSubChunk(const string &id);

        template<typename TSubChunk, typename SubChunk = typename std::enable_if<std::is_base_of<RiffSubChunk, TSubChunk>::value, TSubChunk>::type>
        static shared_ptr<SubChunk> getSubChunk(const shared_ptr<RiffParser> &parser, const string &id) {
            auto subChunk = parser->getSubChunk(id);

            if (subChunk == nullptr) {
                return nullptr;
            } else {
                return dynamic_pointer_cast<SubChunk>(subChunk);
            }
        }

        [[nodiscard]]
        shared_ptr<RiffChunk> getRiffChunk() const;

        void setRiffChunk(const shared_ptr<RiffChunk> &chunk);

        [[nodiscard]]
        bool hasReadDataChunk() const;

        void setReadDataChunk(bool read);

        [[nodiscard]]
        function<shared_ptr<WaveFormatExtensible>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> getFormatExtensibleParser() const;

        void setFormatExtensibleParser(const function<shared_ptr<WaveFormatExtensible>(const shared_ptr<RiffParser> &, const shared_ptr<BinaryReader> &)> &p);

    private:

        shared_ptr<RiffSubChunk> parseSubChunk(const shared_ptr<BinaryReader> &reader);

    };

}
