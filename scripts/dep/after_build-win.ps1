[String]$ProjDir = Join-Path $PSScriptRoot "../.." -Resolve
[String]$BinDir = Join-Path $ProjDir "bin"
mkdir $BinDir -ErrorAction SilentlyContinue

function Copy-BuiltBinaries([String]$subDir)
{
    [String]$destination = Join-Path $BinDir $subDir
    mkdir $destination -ErrorAction SilentlyContinue

    [String]$source = Join-Path $ProjDir "build/${subDir}/cmake-build-release/Release" -Resolve
    Copy-Item -Path "${source}/*" -Destination $destination -Recurse
}

Copy-BuiltBinaries('x86')
Copy-BuiltBinaries('x64')

. (Join-Path $PSScriptRoot "after_build-common.ps1" -Resolve)

[String]$DistDir = Join-Path $ProjDir "dist"
mkdir $DistDir -ErrorAction SilentlyContinue

[String]$zipPath = Join-Path $DistDir "vgaudio.zip"

& 7z a "${zipPath}" "bin\*.*" -r

[String]$zipNameVersioned = "vgaudio-vc14.2-${env:APPVEYOR_BUILD_VERSION}_${env:APPVEYOR_REPO_COMMIT}.zip"

Push-AppveyorArtifact $zipPath -FileName $zipNameVersioned -DeploymentName "Windows builds"

[String]$zipNameFixed = "vgaudio-vc14.2-latest.zip"

Push-AppveyorArtifact $zipPath -FileName $zipNameFixed -DeploymentName "Windows builds (fixed naming)"
