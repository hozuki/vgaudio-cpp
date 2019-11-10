#pragma once

#define DECLARE_ROOT_CLASS(my_class) \
    using MyClass = my_class

#define DECLARE_CLASS(my_class, base_class) \
    using MyBase = base_class; \
    using MyClass = my_class

#define DECLARE_CLASS_TEMPLATE(my_class, base_class, ...) \
    using MyBase = base_class<__VA_ARGS__>; \
    using MyClass = my_class
