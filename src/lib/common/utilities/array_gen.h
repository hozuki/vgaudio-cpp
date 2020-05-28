#pragma once

#include <cstdint>
#include <functional>

#include "runtime_array.h"

namespace common_lib {
    namespace utilities {

        template<typename T, int32_t start, size_t count, int32_t step>
        narray_ptr<T, count> generateArray(std::function<T(int32_t)> elementGenerator) {
            auto table = std::make_shared<std::array<T, count>>();

            if (count == 0) {
                return table;
            }

            auto value = start;
            size_t i = 0;

            do {
                (*table)[i] = elementGenerator(value);

                value += step;
                i += 1;
            } while (i < count);

            return table;
        }

        template<typename T, int32_t start, size_t count>
        narray_ptr<T, count> generateArray(std::function<T(int32_t)> elementGenerator) {
            return generateArray<T, start, count, 1>(elementGenerator);
        }

        template<typename T, size_t count>
        narray_ptr<T, count> generateArray(std::function<T(int32_t)> elementGenerator) {
            return generateArray<T, 0, count, 1>(elementGenerator);
        }

        template<typename T>
        array_ptr<T> generateArray(std::function<T(int32_t)> elementGenerator, int32_t start, size_t count, int32_t step) {
            auto table = make_array_dynamic<T>(count);

            if (count == 0) {
                return table;
            }

            auto value = start;
            size_t i = 0;

            do {
                (*table)[i] = elementGenerator(value);

                value += step;
                i += 1;
            } while (i < count);

            return table;
        }

        template<typename T>
        array_ptr<T> generateArray(std::function<T(int32_t)> elementGenerator, int32_t start, size_t count) {
            return generateArray(elementGenerator, start, count, 1);
        }

        template<typename T>
        array_ptr<T> generateArray(std::function<T(int32_t)> elementGenerator, size_t count) {
            return generateArray(elementGenerator, 0, count, 1);
        }

    }
}
