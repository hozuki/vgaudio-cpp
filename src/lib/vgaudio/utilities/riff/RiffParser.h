#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <list>
#include <type_traits>

#include "../../../common/utilities/_cxx17.h"
#include "../../../common/utilities/type_sys.h"

namespace common_lib {
    namespace io {
        struct BinaryReader;
        struct Stream;
    }
}

namespace vgaudio {
    namespace utilities {
        namespace riff {

            struct RiffChunk;
            struct RiffSubChunk;
            struct WaveFormatExtensible;

            struct RiffParser final {

                DECLARE_ROOT_CLASS(RiffParser);

            private:

                std::shared_ptr<RiffChunk> _riffChunk;
                bool _readDataChunk; // = true
                std::unordered_map<std::string, std::shared_ptr<RiffSubChunk>> _subChunks;

                std::unordered_map<std::string, std::function<std::shared_ptr<RiffSubChunk>(const std::shared_ptr<RiffParser> &, const std::shared_ptr<common_lib::io::BinaryReader> &)>> _registeredSubChunks;

                std::function<std::shared_ptr<WaveFormatExtensible>(const std::shared_ptr<RiffParser> &, const std::shared_ptr<common_lib::io::BinaryReader> &)> _formatExtensibleParser;

            public:

                RiffParser();

                ~RiffParser() = default;

                void registerSubChunk(const std::string &id, const std::function<std::shared_ptr<RiffSubChunk>(const std::shared_ptr<RiffParser> &, const std::shared_ptr<common_lib::io::BinaryReader> &)> &subChunkReader);

                void parseRiff(const std::shared_ptr<common_lib::io::Stream> &file);

                _CXX17_ATTR_NODISCARD
                std::vector<std::shared_ptr<RiffSubChunk>> getAllSubChunks() const;

                std::shared_ptr<RiffSubChunk> getSubChunk(const std::string &id);

                template<typename TSubChunk, typename SubChunk = typename std::enable_if<std::is_base_of<RiffSubChunk, TSubChunk>::value, TSubChunk>::type>
                static std::shared_ptr<SubChunk> getSubChunk(const std::shared_ptr<RiffParser> &parser, const std::string &id) {
                    auto subChunk = parser->getSubChunk(id);

                    if (subChunk == nullptr) {
                        return nullptr;
                    } else {
                        return std::dynamic_pointer_cast<SubChunk>(subChunk);
                    }
                }

                _CXX17_ATTR_NODISCARD
                std::shared_ptr<RiffChunk> getRiffChunk() const;

                void setRiffChunk(const std::shared_ptr<RiffChunk> &chunk);

                _CXX17_ATTR_NODISCARD
                bool hasReadDataChunk() const;

                void setReadDataChunk(bool read);

                _CXX17_ATTR_NODISCARD
                std::function<std::shared_ptr<WaveFormatExtensible>(const std::shared_ptr<RiffParser> &, const std::shared_ptr<common_lib::io::BinaryReader> &)> getFormatExtensibleParser() const;

                void setFormatExtensibleParser(const std::function<std::shared_ptr<WaveFormatExtensible>(const std::shared_ptr<RiffParser> &, const std::shared_ptr<common_lib::io::BinaryReader> &)> &p);

            private:

                std::shared_ptr<RiffSubChunk> parseSubChunk(const std::shared_ptr<common_lib::io::BinaryReader> &reader);

            };

        }
    }
}
