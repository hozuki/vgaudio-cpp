[String]$ProjDir = Join-Path $PSScriptRoot "../.." -Resolve
[String]$BinDir = Join-Path $ProjDir "bin"

& mkdir "$BinDir"

[String]$source = Join-Path $ProjDir "build/cmake-build-release" -Resolve
Copy-Item -Path "${source}/*" -Destination $BinDir -Recurse

[String]$DistDir = Join-Path $ProjDir "dist"
& mkdir "$DistDir"

[String]$zipPath = Join-Path $DistDir "vgaudio.zip"

& 7z a "${zipPath}" "bin/*" -r

[String]$zipName = "vgaudio-bionic-${env:APPVEYOR_BUILD_VERSION}_${env:APPVEYOR_REPO_COMMIT}.zip"

Push-AppveyorArtifact $zipPath -FileName $zipName -DeploymentName "Ubuntu builds"
