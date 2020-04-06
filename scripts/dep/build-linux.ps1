& mkdir build
cd build
& cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D CMAKE_WORKING_DIR="$pwd" ..
make

cd ..
