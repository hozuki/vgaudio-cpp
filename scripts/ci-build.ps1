$projectRoot = Resolve-Path "../"

Push-Location $projectRoot

if ($isWindows)
{
    $scriptPath = [System.IO.Path]::Combine($PSScriptRoot, "dep", "build-win.ps1")
}
elseif ($isLinux)
{
    $scriptPath = [System.IO.Path]::Combine($PSScriptRoot, "dep", "build-linux.ps1")
}

if ($scriptPath -ne $null)
{
    . ($scriptPath)
}
else
{
    Write-Error "Error: Platform not supported"
}

Pop-Location $projectRoot
