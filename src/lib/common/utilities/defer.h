#pragma once

#include "macros.h"

namespace common_lib::utilities {

    template<typename TFunction>
    struct defer final {

    private:

        TFunction _function;
        bool _shouldInvoke;

    public:

        explicit defer(const TFunction &function) noexcept
            : _function(function), _shouldInvoke(true) {
        }

        defer(defer<TFunction> &&other) noexcept
            : _function(other._function), _shouldInvoke(other._shouldInvoke) {
            other._shouldInvoke = false;
        }

        defer(const defer<TFunction> &other) = delete;

        defer<TFunction> &operator=(defer<TFunction> &&other) noexcept = default;

        defer<TFunction> &operator=(const defer<TFunction> &other) = delete;

        ~defer() noexcept {
            if (_shouldInvoke) {
                _function();
            }
        }

    };

    // Usage:
    // auto_defer([&]() {
    //     ...
    // });
#define auto_defer auto CONCAT(_deferred_block, __COUNTER__##__LINE__) = ::common_lib::utilities::defer

}
