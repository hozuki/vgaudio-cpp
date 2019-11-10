#pragma once

#include <cstdint>
#include <string>

namespace vgaudio {

    struct IProgressReport {

        virtual void report(int32_t value) = 0;

        virtual void reportAdd(int32_t value) = 0;

        virtual void setTotal(int32_t value) = 0;

        virtual void logMessage(const std::string &message) = 0;

    };

}
