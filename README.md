# vgaudio-cpp

A C++ port of [VGAudio](https://github.com/Thealexbarney/VGAudio).

And some modern C++ practice.

[![AppVeyor](https://img.shields.io/appveyor/ci/hozuki/vgaudio-cpp.svg)](https://ci.appveyor.com/project/hozuki/vgaudio-cpp)
[![GitHub contributors](https://img.shields.io/github/contributors/hozuki/vgaudio-cpp.svg)](https://github.com/hozuki/vgaudio-cpp/graphs/contributors)
[![GitHub (pre-)release](https://img.shields.io/github/release/hozuki/vgaudio-cpp/all.svg)](https://github.com/hozuki/vgaudio-cpp/releases)
[![Github All Releases](https://img.shields.io/github/downloads/hozuki/vgaudio-cpp/total.svg)](https://github.com/hozuki/vgaudio-cpp/releases)

**Downloads:**

| Platform | Latest CI Artifact |
|---|---|
| Windows (VS 2019) | [Link](https://ci.appveyor.com/api/projects/hozuki/vgaudio-cpp/artifacts/vgaudio-vc16-latest.zip?job=Image%3A%20Visual%20Studio%202019) |
| Ubuntu 18.04 Bionic | [Link](https://ci.appveyor.com/api/projects/hozuki/vgaudio-cpp/artifacts/vgaudio-bionic-latest.zip?job=Image%3A%20Ubuntu) |

## Prerequisites

### Compiler

A compiler toolchain that supports C++17.

Supports:

- MSVC 2019 (19.2x)
- Cygwin (GCC 7.4.0)
- MinGW-w64 (GCC 8.1.0)
- GCC 7.4.0 (Linux/WSL)

For MinGW-w64 this project supports both thread models (pthread and win32).
MinGW32 is *not* supported.

Not tested:

- Anything on macOS

### Other

- CMake 3.0+
- Platform dependent
  - Linux/macOS:
    - `zlib-devel` equivalents (`zlib1g-dev`, `zlib`)

## Building

```bash
mkdir build
cd build
cmake -G "<generator name>" ..
cmake --build . --config Release
```

For generator names please read [the manual](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).

CMake options:

| Option | Description | Default |
|---|---|---|
| `VGAUDIO_APPS_DYNAMIC_LINKING` | Use dynamic linking instead of static linking for applications | `FALSE` |

## License

MIT
