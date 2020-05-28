#pragma once

#include <cstdint>
#include <stdexcept>
#include <iterator>
#include <type_traits>

#include "../macros.h"
#include "../_cxx17.h"
#include "runtime_array_decl.h"
#include "../type_sys.h"

namespace common_lib {
    namespace utilities {

        template<typename T>
        struct runtime_array;

        template<typename T>
        struct runtime_array_container : public runtime_array<T> {

            DECLARE_CLASS_TEMPLATE(runtime_array_container, runtime_array, T);

            typedef T value_type;
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            typedef value_type &reference;
            typedef const value_type &const_reference;
            typedef value_type *pointer;
            typedef const value_type *const_pointer;

            using iterator = typename MyBase::iterator;
            using const_iterator = typename MyBase::const_iterator;
            using reverse_iterator = typename MyBase::reverse_iterator;
            using const_reverse_iterator = typename MyBase::const_reverse_iterator;

        private:

            pointer _data;
            size_type _size;
            bool _autoDelete;

            void checkIndex(difference_type index) const noexcept(false) {
                if (index < 0 || index >= _size) {
                    throw std::out_of_range(StrHelper::format("index %zu out of range", index));
                }
            }

        public:

            runtime_array_container(const_pointer data, size_type count, bool copy, bool autoDelete) {
                value_type *dataPtr;

                if (copy) {
                    if (data != nullptr && count > 0) {
                        dataPtr = new value_type[count];
                        // Copy by value; thus has the similar problem of array element assigning in .NET
                        memcpy(dataPtr, data, sizeof(value_type) * count);
                        _size = count;
                    } else {
                        dataPtr = nullptr;
                        _size = 0;
                    }

                    _autoDelete = count > 0;
                } else {
                    if (data == nullptr && count > 0) {
                        dataPtr = new value_type[count];
                        if ((std::is_integral<T>::value || std::is_floating_point<T>::value || common_lib::utilities::is_enum_class<T>::value) || !std::is_default_constructible<T>::value) {
                            memset(dataPtr, 0, sizeof(value_type) * count);
                        }
                    } else {
                        dataPtr = const_cast<pointer>(data);
                    }

                    _autoDelete = autoDelete && count > 0;
                    _size = count;
                }

                _data = dataPtr;
            }

            runtime_array_container()
                : runtime_array_container(static_cast<pointer>(nullptr), 0, false, false) {
            }

            runtime_array_container(pointer data, size_type count, bool copy, bool autoDelete)
                : runtime_array_container(static_cast<const_pointer>(data), count, copy, autoDelete) {
            }

            virtual ~runtime_array_container() {
                if (_autoDelete) {
                    delete[] _data;
                }
            }

            _CXX17_ATTR_NODISCARD
            bool empty() const override {
                return _size == 0;
            }

            const_reference at(difference_type index) const override {
                checkIndex(index);
                return _data[index];
            }

            reference at(difference_type index) override {
                checkIndex(index);
                return _data[index];
            }

            const_pointer data() const override {
                return _data;
            }

            pointer data() override {
                return _data;
            }

            const_reference front() const override {
                return at(0);
            }

            reference front() override {
                return at(0);
            }

            const_reference back() const override {
                return at(size() - 1);
            }

            reference back() override {
                return at(size() - 1);
            }

            _CXX17_ATTR_NODISCARD
            size_type size() const override {
                return _size;
            }

            _CXX17_ATTR_NODISCARD
            size_type max_size() const override {
                return size();
            }

            _CXX17_ATTR_NODISCARD
            constexpr size_type elementSize() const {
                return sizeof(T);
            }

            const_reference operator[](difference_type index) const override {
                return _data[index];
            }

            reference operator[](difference_type index) override {
                return _data[index];
            }

            iterator begin() override {
                return iterator(data());
            }

            iterator end() override {
                return iterator(data() + size());
            }

            const_iterator begin() const override {
                return const_iterator(data());
            }

            const_iterator end() const override {
                return const_iterator(data() + size());
            }

            const_iterator cbegin() const override {
                return const_iterator(data());
            }

            const_iterator cend() const override {
                return const_iterator(data() + size());
            }

            reverse_iterator rbegin() override {
                return reverse_iterator(begin());
            }

            reverse_iterator rend() override {
                return reverse_iterator(end());
            }

        };

    }
}
