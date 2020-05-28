#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "../utilities/_cxx17.h"
#include "Stream.h"

namespace common_lib {
    namespace io {

        struct FileStream : public Stream {

            DECLARE_CLASS(FileStream, Stream);

        private:

            std::fstream _file;

            std::ios_base::openmode _mode;
            std::string _filePath;

        public:

            FileStream(const char *filePath, std::ios_base::openmode mode);

            FileStream(const std::string &filePath, std::ios_base::openmode mode);

            FileStream(const char *filePath, const char *mode);

            FileStream(const std::string &filePath, const char *mode);

            ~FileStream() override = default;

            int32_t read(void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

            int32_t write(const void *buffer, size_t bufferSize, int32_t offset, int32_t count) override;

            void flush() override;

            void seek(int64_t offset, SeekOrigin origin) override;

            int64_t getPosition() override;

            void setPosition(int64_t position) override;

            int64_t getLength() override;

            void setLength(int64_t length) override;

            _CXX17_ATTR_NODISCARD
            bool canRead() const override;

            _CXX17_ATTR_NODISCARD
            bool canWrite() const override;

            _CXX17_ATTR_NODISCARD
            bool canSeek() const override;

            _CXX17_ATTR_NODISCARD
            const std::string &getFilePath() const;

            _CXX17_ATTR_NODISCARD
            std::ios_base::openmode getOpenMode() const;

        private:

            void initialize();

        };

    }
}
