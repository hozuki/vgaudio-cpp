#pragma once

// Rule of five
#define IMPLEMENT_STATIC_CLASS(class_name) \
    class_name() = delete; \
    class_name(const class_name &) = delete; \
    class_name(const class_name &&) noexcept = delete; \
    class_name &operator=(const class_name &) = delete; \
    class_name &operator=(const class_name &&) noexcept = delete
