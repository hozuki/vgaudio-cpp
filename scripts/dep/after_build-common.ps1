function Copy-MiscFiles()
{
    Copy-Item (Join-Path $ProjDir "README.md" -Resolve) $BinDir
    Copy-Item (Join-Path $ProjDir "LICENSE.txt" -Resolve) $BinDir
}

Copy-MiscFiles