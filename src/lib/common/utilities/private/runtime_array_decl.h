#pragma once

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace common_lib {
    namespace utilities {

        template<typename T>
        struct runtime_array {

            typedef T value_type;
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            typedef value_type &reference;
            typedef const value_type &const_reference;
            typedef value_type *pointer;
            typedef const value_type *const_pointer;

            using iterator = value_type *;
            using const_iterator = const value_type *;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        public:

            virtual ~runtime_array() = default;

            [[nodiscard]]
            virtual bool empty() const = 0;

            virtual const_reference at(difference_type index) const = 0;

            virtual reference at(difference_type index) = 0;

            virtual const_pointer data() const = 0;

            virtual pointer data() = 0;

            virtual const_reference front() const = 0;

            virtual reference front() = 0;

            virtual const_reference back() const = 0;

            virtual reference back() = 0;

            [[nodiscard]]
            virtual size_type size() const = 0;

            [[nodiscard]]
            virtual size_type max_size() const = 0;

            virtual const_reference operator[](difference_type index) const = 0;

            virtual reference operator[](difference_type index) = 0;

            virtual iterator begin() = 0;

            virtual iterator end() = 0;

            virtual const_iterator begin() const = 0;

            virtual const_iterator end() const = 0;

            virtual const_iterator cbegin() const = 0;

            virtual const_iterator cend() const = 0;

            virtual reverse_iterator rbegin() = 0;

            virtual reverse_iterator rend() = 0;

            const_reverse_iterator rbegin() const {
                return const_reverse_iterator(begin());
            }

            const_reverse_iterator rend() const {
                return const_reverse_iterator(end());
            }

            const_reverse_iterator crbegin() const {
                return const_reverse_iterator(cbegin());
            }

            const_reverse_iterator crend() const {
                return const_reverse_iterator(cend());
            }

        };

    }
}
