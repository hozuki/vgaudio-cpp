mkdir build -ErrorAction SilentlyContinue
cd build
mkdir x86 -ErrorAction SilentlyContinue
cd x86
& cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Release -DCMAKE_WORKING_DIR="$pwd" ../..
cd ..
mkdir x64
cd x64
& cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_WORKING_DIR="$pwd" ../..
cd ../..

$platforms = @("x86", "x64")
$zlibDir = Join-Path $PSScriptRoot "../../ext/zlib" -Resolve
$zlibFiles = @{ }
$zlibFiles.Add((Join-Path $zlibDir "build/bin/zlib1.dll"), "zlib1.dll")
$zlibFiles.Add((Join-Path $zlibDir "build/lib/zlib.lib"), "zlib.lib")
$zlibFiles.Add((Join-Path $zlibDir "build/lib/zlibstatic.lib"), "zlibstatic.lib")
$zlibFiles.Add((Join-Path $zlibDir "README" -Resolve), "README-zlib")

foreach ($platform in $platforms)
{
    $targetDir = Join-Path $pwd "build/$platform/cmake-build-release/Release" -Resolve

    Remove-Item (Join-Path $zlibDir "build") -Recurse
    msbuild "build\$platform\vgaudio.sln" /p:Configuration = Release

    foreach ($srcPath in $zlibFiles.Keys)
    {
        $dstPath = Join-Path $targetDir $zlibFiles[$srcPath] -Resolve
        Copy-Item $srcPath $dstPath
    }
}
