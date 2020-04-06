git submodule update --init --recursive
cd ext/zlib
git checkout v1.2.11
cd ../..

if ($isLinux)
{
    sudo apt install zlib1g-dev p7zip-full -y
}
