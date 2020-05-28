#pragma once

#include <memory>

#include "vga_object_type.h"
#include "../../../api/modules/vga_handle.h"

struct vga_shared_ptr_conv {

    vga_shared_ptr_conv() = default;

    vga_shared_ptr_conv(const vga_shared_ptr_conv &) = delete;

    vga_shared_ptr_conv(vga_shared_ptr_conv &&) = delete;

    vga_shared_ptr_conv &operator=(const vga_shared_ptr_conv &) = delete;

    vga_shared_ptr_conv &operator=(vga_shared_ptr_conv &&) = delete;

    virtual ~vga_shared_ptr_conv() = default;

    virtual void *data() = 0;

    virtual bool valid() = 0;

    virtual void dispose() = 0;

};

struct vga_object {

    VgaObjectType type;
    vga_shared_ptr_conv *ptr;

    ~vga_object();

};

template<typename T>
struct vga_shared_ptr_conv_impl final : public vga_shared_ptr_conv {

private:

    bool _valid;
    std::shared_ptr<T> _ptr;

public:

    explicit vga_shared_ptr_conv_impl(const std::shared_ptr<T> &ptr) {
        _ptr = ptr;
        _valid = true;
    }

    vga_shared_ptr_conv_impl(const vga_shared_ptr_conv_impl<T> &) = delete;

    vga_shared_ptr_conv_impl(vga_shared_ptr_conv_impl<T> &&) = delete;

    vga_shared_ptr_conv_impl &operator=(const vga_shared_ptr_conv_impl<T> &) = delete;

    vga_shared_ptr_conv_impl &operator=(vga_shared_ptr_conv_impl<T> &&) = delete;

    ~vga_shared_ptr_conv_impl() override {
        dispose();
    }

    void *data() override {
        if (valid()) {
            return _ptr.get();
        } else {
            return nullptr;
        }
    }

    bool valid() override {
        return _valid;
    }

    void dispose() override {
        if (!valid()) {
            return;
        }

        _valid = false;
        _ptr.reset();
    }

};

template<typename T>
vga_handle create_vga_object(const std::shared_ptr<T> &ptr, VgaObjectType type) {
    const auto ptrConv = new vga_shared_ptr_conv_impl<T>(ptr);
    const auto object = new vga_object();

    object->type = type;
    object->ptr = ptrConv;

    return object;
}
