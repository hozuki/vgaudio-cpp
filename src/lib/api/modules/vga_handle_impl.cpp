#include "vga_handle_impl.h"
#include "../../../api/modules/vga_handle.h"

vga_object::~vga_object() {
    delete ptr;
}

static void destroy_vga_object(vga_object *object) {
    delete object->ptr;
    object->ptr = nullptr;
}

VGA_API_IMPL(vga_bool) vgaIsHandleValid(vga_handle handle) {
    if (handle == nullptr) {
        return false;
    }

    if (handle->type == VgaObjectType::Invalid) {
        return false;
    }

    if (handle->ptr == nullptr) {
        return false;
    }

    return handle->ptr->valid();
}

VGA_API_IMPL(void) vgaCloseHandle(vga_handle handle) {
    if (vgaIsHandleValid(handle)) {
        destroy_vga_object(handle);
    }
}
