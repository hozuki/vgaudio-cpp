#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include <vector>
#include <list>

#include "static_class.h"
#include "runtime_array.h"
#include "runtime_jagged_array.h"
#include "compare.h"

namespace common_lib::utilities {

    struct ArrayHelper {

        IMPLEMENT_STATIC_CLASS(ArrayHelper);

        template<typename T>
        static void copyByValue(const array_ptr<T> &from, const array_ptr<T> &to, int32_t count) {
            return copyByValue(from, 0, to, 0, count);
        }

        template<typename T>
        static void copyByValue(const array_ptr<T> &from, int32_t fromStart, const array_ptr<T> &to, int32_t toStart, int32_t count) {
            assert(fromStart >= 0);
            assert(toStart >= 0);
            assert(count >= 0);

            if (count <= 0) {
                return;
            }

            const auto s = std::max<int32_t>(0, std::min(count, std::min(static_cast<int32_t>(from->size()) - fromStart, static_cast<int32_t>(to->size()) - toStart)));

            if (s > 0) {
                memcpy(to->data() + toStart, from->data() + fromStart, sizeof(T) * s);
            }
        }

        template<typename T>
        static void clearByValue(const array_ptr<T> &array, int32_t startIndex, int32_t count) {
            assert(startIndex >= 0);
            assert(count >= 0);

            if (count <= 0) {
                return;
            }

            const auto s = std::max<int32_t>(0, std::min(static_cast<int32_t>(array->size()) - startIndex, count));

            if (s > 0) {
                memset(array->data() + startIndex, 0, sizeof(T) * s);
            }
        }

        template<typename T, size_t N>
        static void clearByValue(const narray_ptr<T, N> &array, int32_t startIndex, int32_t count) {
            assert(startIndex >= 0);
            assert(count >= 0);

            if (count <= 0) {
                return;
            }

            const auto s = std::max<int32_t>(0, std::min<int32_t>(N - startIndex, count));

            if (s > 0) {
                memset(array->data() + startIndex, 0, sizeof(T) * s);
            }
        }

        template<typename T, size_t N>
        static array_ptr<T> arrayToDynamic(const narray_ptr<T, N> &arr) {
            return std::make_shared<runtime_array_wrapper<T, N>>(arr);
        }

        template<typename T>
        static array_ptr<T> toArray(const std::vector<T> &vector) {
            const auto size = vector.size();
            auto arr = make_array_dynamic<T>(size);

            for (auto i = 0; i < size; i += 1) {
                (*arr)[i] = vector[i];
            }

            return arr;
        }

        template<typename T>
        static array_ptr<T> toArray(const std::list<T> &lst) {
            const auto size = lst.size();
            auto arr = make_array_dynamic<T>(size);

            size_t index = 0;

            for (const auto &value : lst) {
                (*arr)[index] = value;
                index += 1;
            }

            return arr;
        }

        template<typename T>
        static jarray2_ptr<T> toArray(const std::vector<array_ptr<T>> &vector) {
            const auto size = vector.size();
            auto arr = make_jagged_array_2_dynamic<T>(size);

            for (auto i = 0; i < size; i += 1) {
                (*arr)[i] = vector[i];
            }

            return arr;
        }

        template<typename T>
        static jarray2_ptr<T> toArray(const std::list<array_ptr<T>> &lst) {
            const auto size = lst.size();
            auto arr = make_jagged_array_2_dynamic<T>(size);

            size_t index = 0;

            for (const auto &value : lst) {
                (*arr)[index] = value;
                index += 1;
            }

            return arr;
        }

        template<typename T>
        static array_ptr<T> concat(const array_ptr<T> &array1, const array_ptr<T> &array2) {
            const auto s1 = array1->size();
            const auto s2 = array2->size();

            auto arr = make_array_dynamic<T>(s1 + s2);

            for (auto i = 0; i < s1; i += 1) {
                (*arr)[i] = (*array1)[i];
            }

            for (auto i = 0; i < s2; i += 1) {
                (*arr)[i + s1] = (*array2)[i];
            }

            return arr;
        }

        template<typename T>
        static jarray2_ptr<T> concat(const jarray2_ptr<T> &array1, const jarray2_ptr<T> &array2) {
            const auto s1 = array1->size();
            const auto s2 = array2->size();

            auto arr = make_jagged_array_2_dynamic<T>(s1 + s2);

            for (auto i = 0; i < s1; i += 1) {
                (*arr)[i] = (*array1)[i];
            }

            for (auto i = 0; i < s2; i += 1) {
                (*arr)[i + s1] = (*array2)[i];
            }

            return arr;
        }

        template<typename T, size_t N1, size_t N2>
        static array_ptr<T> concat(const narray_ptr<T, N1> &array1, const narray_ptr<T, N2> &array2) {
            auto arr = make_array_static<T, N1 + N2>();

            for (auto i = 0; i < N1; i += 1) {
                (*arr)[i] = (*array1)[i];
            }

            for (auto i = 0; i < N2; i += 1) {
                (*arr)[i + N1] = (*array2)[i];
            }

            return array_to_dynamic(arr);
        }

        // From .NET Array.BinarySearch() implementation
        template<typename T, typename Compare = common_lib::utilities::compare<T>>
        static ptrdiff_t binarySearch(const array_ptr<T> &array, ptrdiff_t index, ptrdiff_t count, const T &value, Compare compare = Compare()) {
            ptrdiff_t lo = index;
            ptrdiff_t hi = index + count - 1;

            while (lo <= hi) {
                const auto i = lo + ((hi - lo) >> 1);
                const auto c = compare((*array)[i], value);

                if (c == 0) {
                    return i;
                } else if (c < 0) {
                    lo = i + 1;
                } else {
                    hi = i - 1;
                }
            }

            return ~lo;
        }

        template<typename T>
        static ptrdiff_t binarySearch(const array_ptr<T> &array, const T &value) {
            return binarySearch(array, 0, array->size(), value);
        }

        template<typename T, size_t Dim, typename Compare = common_lib::utilities::compare<T>>
        static ptrdiff_t binarySearch(const narray_ptr<T, Dim> &array, ptrdiff_t index, ptrdiff_t count, const T &value, Compare compare = Compare()) {
            ptrdiff_t lo = index;
            ptrdiff_t hi = index + count - 1;

            while (lo <= hi) {
                const auto i = lo + ((hi - lo) >> 1);
                const auto c = compare((*array)[i], value);

                if (c == 0) {
                    return i;
                } else if (c < 0) {
                    lo = i + 1;
                } else {
                    hi = i - 1;
                }
            }

            return ~lo;
        }

        template<typename T, size_t Dim>
        static ptrdiff_t binarySearch(const narray_ptr<T, Dim> &array, const T &value) {
            return binarySearch(array, 0, array->size(), value);
        }

    };

}
