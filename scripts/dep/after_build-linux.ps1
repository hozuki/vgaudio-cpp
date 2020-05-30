[String]$ProjDir = Join-Path $PSScriptRoot "../.." -Resolve
[String]$BinDir = Join-Path $ProjDir "bin"

& mkdir "$BinDir"

[String]$source = Join-Path $ProjDir "build/cmake-build-release" -Resolve
Copy-Item -Path "${source}/*" -Destination $BinDir -Recurse

. (Join-Path $PSScriptRoot "after_build-common.ps1" -Resolve)

[String]$DistDir = Join-Path $ProjDir "dist"
& mkdir "$DistDir"

[String]$zipPath = Join-Path $DistDir "vgaudio.zip"

& 7z a "${zipPath}" "bin/*" -r

[String]$zipNameVersioned = "vgaudio-bionic-${env:APPVEYOR_BUILD_VERSION}_${env:APPVEYOR_REPO_COMMIT}.zip"

Push-AppveyorArtifact $zipPath -FileName $zipNameVersioned -DeploymentName "Ubuntu builds"

[String]$zipNameFixed = "vgaudio-bionic-latest.zip"

Push-AppveyorArtifact $zipPath -FileName $zipNameFixed -DeploymentName "Ubuntu builds (fixed naming)"
