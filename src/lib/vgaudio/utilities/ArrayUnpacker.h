#pragma once

#include <cstdint>
#include <tuple>

#include "../../common/utilities/static_class.h"
#include "../../common/utilities/runtime_array.h"

namespace vgaudio::utilities {

    using namespace std;
    using namespace common_lib::utilities;

    struct ArrayUnpacker {

        IMPLEMENT_STATIC_CLASS(ArrayUnpacker);

    public:

        enum class TypeIndex {

            Unknown = -1,
            UInt8 = 0,
            Int8 = 1,
            Char = 2, // unused
            Int16 = 3,
            UInt16 = 4,
            Int32 = 5,
            UInt32 = 6,
            Int64 = 7,
            UInt64 = 8,
            Single = 9,
            Double = 10,

        };

        template<typename T = void>
        struct ArrayWrapper {

            // I have no choice...
        public:

            void *_ptr;
            size_t _length;
            size_t _rank;

        public:

            ArrayWrapper()
                : _ptr(nullptr), _length(0), _rank(0) {
            }

            ArrayWrapper(T *ptr, size_t length, size_t rank)
                : _ptr(ptr), _length(length), _rank(rank) {
            }

            ArrayWrapper(const T *ptr, size_t length, size_t rank)
                : _ptr(const_cast<T *>(ptr)), _length(length), _rank(rank) {
            }

            ArrayWrapper(const ArrayWrapper<T> &) = default;

            ArrayWrapper(ArrayWrapper<T> &&) noexcept = default;

            ArrayWrapper<T> &operator=(const ArrayWrapper<T> &) = default;

            ArrayWrapper<T> &operator=(ArrayWrapper<T> &&) = default;

            template<typename U>
            ArrayWrapper(const ArrayWrapper<U> &u)
                : _ptr(u._ptr), _length(u._length), _rank(u._rank) {
            }

            template<typename U>
            ArrayWrapper(ArrayWrapper<U> &&u)
                : _ptr(u._ptr), _length(u._length), _rank(u._rank) {
            }

            template<typename U>
            ArrayWrapper<T> &operator=(const ArrayWrapper<U> &u) {
                _ptr = u._ptr;
                _length = u._length;
                _rank = u._rank;
                return *this;
            }

            template<typename U>
            ArrayWrapper<T> &operator=(ArrayWrapper<U> &&u) noexcept {
                _ptr = u._ptr;
                _length = u._length;
                _rank = u._rank;
                return *this;
            }

            [[nodiscard]]
            size_t getLength() const {
                return _length;
            }

            void *getPointer() {
                return _ptr;
            }

            [[nodiscard]]
            const void *getPointer() const {
                return _ptr;
            }

            T *getTypedPointer() {
                return static_cast<T *>(_ptr);
            }

            [[nodiscard]]
            const T *getTypedPointer() const {
                return static_cast<const T *>(_ptr);
            }

            [[nodiscard]]
            size_t getRank() const {
                return _rank;
            }

        };

        // type index, pointer to data (may be pointer of pointers), sizes of each rank
        static array_ptr<std::tuple<TypeIndex, ArrayWrapper<>>> unpackArray(const void *packedArrays, size_t size);

        static void releaseWrappedArray(TypeIndex typeIndex, ArrayWrapper<> wrapper);

    };

}
