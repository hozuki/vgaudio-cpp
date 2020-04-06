mkdir build -ErrorAction SilentlyContinue
cd build
& cmake -G "UNIX" -D CMAKE_BUILD_TYPE=Release -D CMAKE_WORKING_DIR="$pwd" ..
make

cd ..
