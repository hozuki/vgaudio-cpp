#include <iostream>
#include <typeinfo>
#include <string>
#include <cstdint>
#include <cstdlib>

#include "../../src/api/vgaudio.h"
#include "../../src/lib/common/utilities/runtime_array.h"
#include "../../src/lib/common/utilities/runtime_jagged_array.h"
#include "../../src/lib/common/utilities/runtime_jagged_array.h"
#include "../../src/lib/common/utilities/ArrayHelper.h"
#include "../../src/lib/common/utilities/Guid.h"
#include "../../src/lib/common/utilities/traits.h"
#include "../../src/lib/common/io/MemoryStream.h"
#include "../../src/lib/vgaudio/codecs/crihca/CriHcaParameters.h"
#include "../../src/lib/vgaudio/codecs/crihca/ChannelType.h"

using namespace std;
using namespace common_lib::utilities;
using namespace common_lib::io;
using namespace vgaudio::codecs::crihca;

void test_array_types(const array_ptr<double> &array);

void test_memory_stream();

struct TypeA {

    typedef TypeA MyClass;

};

struct TypeB : public TypeA {

    typedef MyClass MyBase;
    typedef TypeB MyClass;

};

template<typename T>
string type_name(const T &t) {
    char buf[129] = {0};
    vgaDemangleSymbolName(typeid(t).name(), buf, sizeof(buf));
    return string(buf);
}

template<typename T>
string type_name() {
    char buf[129] = {0};
    vgaDemangleSymbolName(typeid(T).name(), buf, sizeof(buf));
    return string(buf);
}

int main() {
    {
#if defined(_MSC_VER)
        auto arr1 = make_array_static_with(1.0, 2.0, 3.0, 4.0, 5.0, 5.0);
#else
        auto arr1 = make_array_static_with(1, 2, 3, 4, 5, 5.0);
#endif
        cout << "Array 1 items: ";

        for (auto elem : *arr1) {
            cout << type_name(elem) << "#" << elem << " ";
        }

        cout << endl;
    }


    {
        auto arr2 = make_array_dynamic_with(1, 2, 3, 4, 5, 6);
        cout << "Array 2 items: ";

        for (auto elem : *arr2) {
            cout << type_name(elem) << "#" << elem << " ";
        }

        cout << endl;
    }

    {
        auto arr3 = make_jagged_array_2_dynamic<double>(4, 5);

        cout << "Array 3 size:" << arr3->size() << endl;

        cout << "Array 3 [0] size:" << arr3->at(0)->size() << endl;

        (*(*arr3)[0])[0] = 1;
        (*(*arr3)[1])[1] = 10;
        (*(*arr3)[2])[2] = 20;

        cout << "Array 3 items (iterated twice):" << endl;

        for (auto i = 0; i < 2; i += 1) {
            for (const auto &a : *arr3) {
                for (auto elem : *a) {
                    cout << type_name(elem) << "#" << elem << " ";
                }

                cout << endl;
            }
        }
    }

    {
        auto arr4 = make_jagged_array_2_static<double, 4, 5>();

        (*(*arr4)[0])[0] = 1;
        (*(*arr4)[1])[1] = 10;
        (*(*arr4)[2])[2] = 20;

        cout << "Array 4 items:" << endl;

        // Do not use `const auto &` because we are dealing with smart pointers
        for (auto a : *arr4) {
            for (auto elem : *a) {
                cout << type_name(elem) << "#" << elem << " ";
            }

            cout << endl;
        }
    }

    {
        auto arr5 = make_array_dynamic_with(ChannelType::StereoPrimary, ChannelType::StereoSecondary, ChannelType::Discrete);

        cout << "Array 5 items: ";

        for (auto elem : *arr5) {
            cout << type_name(elem) << "#" << static_cast<int>(elem) << " ";
        }

        cout << endl;
    }

    {
        auto arr6 = make_jagged_array_dynamic<int, 3>(2, 3, 4);

        (*(*(*arr6)[0])[0])[0] = 2;
        (*(*(*arr6)[0])[1])[0] = 5;
        (*(*(*arr6)[0])[0])[2] = 6;

        cout << "Array 6 items: ";

        for (const auto &a : *arr6) {
            for (const auto &b : *a) {
                for (auto elem : *b) {
                    cout << type_name(elem) << "#" << static_cast<int>(elem) << " ";
                }

                cout << " ; ";
            }

            cout << endl;
        }

        cout << endl;
    }

    {
        cout << "Are these types all numeric? " << all_numeric<int, double, int32_t, uint64_t>::value;

        cout << endl;
    }

    {
        vgaudio::codecs::crihca::CriHcaParameters p;

        cout << "Name: " << type_name<vgaudio::codecs::crihca::CriHcaParameters::MyBase>() << endl;
        cout << "Name: " << type_name<TypeA::MyClass>() << "/" << type_name<TypeB::MyBase>() << "/" << type_name<TypeB::MyClass>() << endl;
    }

    test_array_types(make_array_dynamic_with(1.0, 2.0, 3.0, 4.0, 5.0));
    test_array_types(ArrayHelper::arrayToDynamic(make_array_static_with(1.0, 2.0, 3.0, 4.0, 5.0)));

    test_memory_stream();

    {
        Guid guid1(0x12345678, 0x9012, 0xabcd, 0xef01, static_cast<uint16_t>(0x90abu), static_cast<uint32_t>(0x12345678u));
        Guid guid2(0x12345678, 0x9012, 0xabcd, 0xef01, static_cast<uint32_t>(0x567890abu), static_cast<uint16_t>(0x1234u));
        cout << "GUID example 1: " << guid1.toString() << endl;
        cout << "GUID example 2: " << guid2.toString() << endl;
        cout << "Are they equal? " << (guid1 == guid2) << endl;
        cout << "Direct output test: " << Guid() << endl;
    }

    return 0;
}

void test_array_types(const array_ptr<double> &array) {
    cout << "Array type test: ";
    for (auto p : *array) {
        cout << p << " ";
    }
    cout << endl;
}

void test_memory_stream() {
    char ref[] = "hello, world!";
    auto buf = make_array_dynamic<uint8_t>(reinterpret_cast<uint8_t *>(ref), sizeof(ref), false, false);
    auto mem = make_shared<MemoryStream>(buf, false);

    auto arr = mem->toArray();

    cout << reinterpret_cast<const char *>(arr->data()) << " (" << arr->size() << ")" << endl;
}
