mkdir build -ErrorAction SilentlyContinue
cd build
mkdir x86 -ErrorAction SilentlyContinue
cd x86
& cmake -G "Visual Studio 16 2019" -A Win32 -D CMAKE_BUILD_TYPE=Release -D CMAKE_WORKING_DIR="$pwd" ../..
cd ..
mkdir x64
cd x64
& cmake -G "Visual Studio 16 2019" -A x64 -D CMAKE_BUILD_TYPE=Release -D CMAKE_WORKING_DIR="$pwd" ../..
cd ../..

$zlibBuildDir = Join-Path $PSScriptRoot "../../ext/zlib/build" -Resolve
$zlibBinDir = Join-Path $zlibBuildDir "bin"
$zlibBinDll = Join-Path $zlibBinDir "zlib1.dll"

Remove-Item $zlibBuildDir -Recurse
msbuild build\x86\vgaudio.sln /p:Configuration=Release
Copy-Item $zlibBinDll (Join-Path $pwd "build/x86/cmake-build-release/Release" -Resolve)

Remove-Item $zlibBuildDir -Recurse
msbuild build\x64\vgaudio.sln /p:Configuration=Release
Copy-Item $zlibBinDll (Join-Path $pwd "build/x64/cmake-build-release/Release" -Resolve)
