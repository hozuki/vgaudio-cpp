[String]$ProjDir = Join-Path $PSScriptRoot "../.." -Resolve
[String]$BinDir = Join-Path $ProjDir "bin"
mkdir $BinDir -ErrorAction SilentlyContinue

function Copy-BuiltBinaries([String]$subDir)
{
    $destination = Join-Path $BinDir $subDir
    mkdir $destination -ErrorAction SilentlyContinue

    $source = Join-Path $ProjDir "build/${subDir}/cmake-build-release/Release" -Resolve
    Copy-Item -Path "${source}/*" -Destination $destination -Recurse
}

Copy-BuiltBinaries('x86')
Copy-BuiltBinaries('x64')

[String]$DistDir = Join-Path $ProjDir "dist"
mkdir $DistDir -ErrorAction SilentlyContinue

[String]$zipPath = Join-Path $DistDir "vgaudio.zip"

& 7z a "${zipPath}" "bin\*.*" -r

Push-AppveyorArtifact $zipPath -FileName "vgaudio.zip" -DeploymentName "Windows builds"
