#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "Stream.h"

namespace common_lib::io {

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

        [[nodiscard]]
        bool canRead() const override;

        [[nodiscard]]
        bool canWrite() const override;

        [[nodiscard]]
        bool canSeek() const override;

        [[nodiscard]]
        const std::string &getFilePath() const;

        [[nodiscard]]
        std::ios_base::openmode getOpenMode() const;

    private:

        void initialize();

    };

}
